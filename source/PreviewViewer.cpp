#include "shaderlab/PreviewViewer.h"

#include <SM_Vector.h>
#include <unirender/Device.h>
#include <unirender/Context.h>
#include <unirender/IndexBuffer.h>
#include <unirender/VertexBuffer.h>
#include <unirender/VertexArray.h>
#include <unirender/VertexInputAttribute.h>
#include <unirender/ComponentDataType.h>
#include <unirender/DrawState.h>
#include <unirender/Factory.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Uniform.h>
#include <shadergraph/VarNames.h>
#include <shadergraph/Variant.h>
#include <shadergraph/ValueImpl.h>
#include <shadertrans/ShaderTrans.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/PerspCam.h>

namespace
{

const char* vs2d = R"(

#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

)";

const char* vs3d = R"(

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

out vec2 TexCoord;
out vec3 #frag_pos#;

layout(std140) uniform UBO_VS
{
    mat4 projection;
    mat4 view;
    mat4 model;
} ubo_vs;

void main()
{
    TexCoord = vec2(texcoord.x, texcoord.y);

	#frag_pos# = position;
	gl_Position = ubo_vs.projection * ubo_vs.view * ubo_vs.model * vec4(position, 1.0);
}

)";

struct Vertex
{
    Vertex() {}
    Vertex(size_t ix, size_t iz, size_t size_x, size_t size_z)
    {
        //position = sm::vec3(
        //    ix / static_cast<float>(size_x),
        //    0,
        //    iz / static_cast<float>(size_z)
        //);
        position = sm::vec3(
            ix / static_cast<float>(size_x),
            iz / static_cast<float>(size_z),
            0
        );
        texcoords = sm::vec2(
            ix / static_cast<float>(size_x),
            iz / static_cast<float>(size_z)
        );
    }

    sm::vec3 position;
    sm::vec2 texcoords;
};

const int HEIGHT_FIELD_SIZE = 512;

}

namespace shaderlab
{

PreviewViewer::PreviewViewer(const ur::Device& dev)
{
    InitRect2dVA(dev);
    InitRect3dVA(dev);
    InitGrids3dVA(dev);
}

void PreviewViewer::Draw(ur::Context& ctx, const pt0::CameraPtr& cam, 
	                      const void* scene) const
{
    if (m_shader)
    {
        auto model_updater = m_shader->QueryUniformUpdater(ur::GetUpdaterTypeID<pt0::ModelMatUpdater>());
        if (model_updater) {
            std::static_pointer_cast<pt0::ModelMatUpdater>(model_updater)->Update(sm::mat4());
        }

        auto u_cam_pos = m_shader->QueryUniform(shadergraph::VarNames::PositionUniforms::cam_pos);
        if (u_cam_pos)
        {
            if (cam->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
            {
                auto persp = std::static_pointer_cast<pt3::PerspCam>(cam);
                u_cam_pos->SetValue(persp->GetPos().xyz, 3);
            }
        }

        for (auto& v : m_input_vars)
        {
            if (!v.val) {
                continue;
            }

            auto uniform = m_shader->QueryUniform(v.name);
            if (!uniform) {
                continue;
            }

            switch (v.type)
            {
            case shadergraph::VarType::Float2:
            {
                auto f2 = std::static_pointer_cast<shadergraph::Float2Val>(v.val);
                uniform->SetValue(f2->xy, 2);
            }
                break;
            }
        }
    }

    ur::DrawState ds;
    ds.program = m_shader;

    switch (m_va)
    {
    case VertexArray::Rect2D:
        ds.vertex_array = m_rect_2d_va;
        break;
    case VertexArray::Rect3D:
        ds.vertex_array = m_rect_3d_va;
        break;
    case VertexArray::Grids3D:
        ds.vertex_array = m_grids_3d_va;
        break;
    default:
        assert(0);
    }

    ds.render_state.depth_test.enabled = false;
    ds.render_state.facet_culling.enabled = false;

    for (auto& t : m_textures) {
        ctx.SetTexture(t.first, t.second);
    }

    ctx.Draw(ur::PrimitiveType::Triangles, ds, scene);
}

void PreviewViewer::Update(ur::Context& ctx, const pt0::CameraPtr& cam, const std::shared_ptr<ur::ShaderProgram>& shader, 
	                       const std::vector<std::pair<std::string, ur::TexturePtr>>& textures, const std::vector<shadergraph::Variant>& input_vars)
{
    if (m_shader == shader) {
        return;
    }

    m_shader = shader;

    m_textures.clear();
    if (shader)
    {
        for (auto& tex : textures)
        {
            auto slot = shader->QueryTexSlot(tex.first);
            if (slot >= 0) {
                m_textures.push_back({ slot, tex.second });
            }
        }
    }

    m_input_vars = input_vars;
}

std::string PreviewViewer::GetVertShaderCode() const
{
    return vs2d;
}

void PreviewViewer::InitRect2dVA(const ur::Device& dev)
{
    m_rect_2d_va = dev.CreateVertexArray();

    auto usage = ur::BufferUsageHint::StaticDraw;

    auto ibuf = dev.CreateIndexBuffer(usage, 0);
    auto ibuf_sz = sizeof(unsigned short) * 6;
    const unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };
    ibuf->Reset(ibuf_sz);
    ibuf->ReadFromMemory(indices, ibuf_sz, 0);
    m_rect_2d_va->SetIndexBuffer(ibuf);

    auto vbuf = dev.CreateVertexBuffer(ur::BufferUsageHint::StaticDraw, 0);
    auto vbuf_sz = sizeof(float) * 4 * 4;
    vbuf->Reset(vbuf_sz);
    sm::vec2 min(-1, -1);
    sm::vec2 max(1, 1);
    float vertices[] = {
        // positions  // texture Coords
        min.x, min.y, 0.0f, 0.0f,
        max.x, min.y, 1.0f, 0.0f,
        max.x, max.y, 1.0f, 1.0f,
        min.x, max.y, 0.0f, 1.0f,
    };
    vbuf->ReadFromMemory(vertices, vbuf_sz, 0);

    m_rect_2d_va->SetVertexBufferAttrs({
        // pos
        std::make_shared<ur::VertexInputAttribute>(0, ur::ComponentDataType::Float, 2, 0, 16),
        // uv
        std::make_shared<ur::VertexInputAttribute>(1, ur::ComponentDataType::Float, 2, 8, 16)
    });

    m_rect_2d_va->SetVertexBuffer(vbuf);
}

void PreviewViewer::InitRect3dVA(const ur::Device& dev)
{
    m_rect_3d_va = dev.CreateVertexArray();

    auto usage = ur::BufferUsageHint::StaticDraw;

    auto ibuf = dev.CreateIndexBuffer(usage, 0);
    auto ibuf_sz = sizeof(unsigned short) * 6;
    const unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };
    ibuf->Reset(ibuf_sz);
    ibuf->ReadFromMemory(indices, ibuf_sz, 0);
    m_rect_3d_va->SetIndexBuffer(ibuf);

    auto vbuf = dev.CreateVertexBuffer(ur::BufferUsageHint::StaticDraw, 0);
    auto vbuf_sz = sizeof(float) * 5 * 4;
    vbuf->Reset(vbuf_sz);
    sm::vec2 min(-1, -1);
    sm::vec2 max(1, 1);
    float vertices[] = {
        // positions  // texture Coords
        min.x, 0.0f, min.y, 0.0f, 0.0f,
        max.x, 0.0f, min.y, 1.0f, 0.0f,
        max.x, 0.0f, max.y, 1.0f, 1.0f,
        min.x, 0.0f, max.y, 0.0f, 1.0f,
    };
    vbuf->ReadFromMemory(vertices, vbuf_sz, 0);

    m_rect_3d_va->SetVertexBufferAttrs({
        // pos
        std::make_shared<ur::VertexInputAttribute>(0, ur::ComponentDataType::Float, 3, 0, 20),
        // uv
        std::make_shared<ur::VertexInputAttribute>(1, ur::ComponentDataType::Float, 2, 12, 20)
    });

    m_rect_3d_va->SetVertexBuffer(vbuf);
}

void PreviewViewer::InitGrids3dVA(const ur::Device& dev)
{
    m_grids_3d_va = dev.CreateVertexArray();

    auto usage = ur::BufferUsageHint::StaticDraw;

    const size_t num_vert = HEIGHT_FIELD_SIZE * HEIGHT_FIELD_SIZE;
    Vertex* vertices = new Vertex[num_vert];
    auto vert_ptr = vertices;
    for (size_t y = 0; y < HEIGHT_FIELD_SIZE; ++y) {
        for (size_t x = 0; x < HEIGHT_FIELD_SIZE; ++x) {
            *vert_ptr++ = Vertex(x, y, HEIGHT_FIELD_SIZE, HEIGHT_FIELD_SIZE);
        }
    }

    auto vbuf = dev.CreateVertexBuffer(usage, 0);
    auto vbuf_sz = sizeof(Vertex) * num_vert;
    vbuf->Reset(vbuf_sz);
    vbuf->ReadFromMemory(vertices, vbuf_sz, 0);
    m_grids_3d_va->SetVertexBuffer(vbuf);

    const size_t num_idx = (HEIGHT_FIELD_SIZE - 1) * (HEIGHT_FIELD_SIZE - 1) * 6;
    unsigned int* indices = new unsigned int[num_idx];
    auto index_ptr = indices;
    for (size_t y = 0; y < HEIGHT_FIELD_SIZE - 1; ++y) {
        for (size_t x = 0; x < HEIGHT_FIELD_SIZE - 1; ++x) {
            size_t ll = y * HEIGHT_FIELD_SIZE + x;
            size_t rl = ll + 1;
            size_t lh = ll + HEIGHT_FIELD_SIZE;
            size_t rh = lh + 1;
            *index_ptr++ = ll;
            *index_ptr++ = lh;
            *index_ptr++ = rh;
            *index_ptr++ = ll;
            *index_ptr++ = rh;
            *index_ptr++ = rl;
        }
    }

    auto ibuf = dev.CreateIndexBuffer(usage, 0);
    auto ibuf_sz = sizeof(unsigned int) * num_idx;
    ibuf->Reset(ibuf_sz);
    ibuf->ReadFromMemory(indices, ibuf_sz, 0);
    ibuf->SetDataType(ur::IndexBufferDataType::UnsignedInt);
    m_grids_3d_va->SetIndexBuffer(ibuf);

    m_grids_3d_va->SetVertexBufferAttrs({
        // pos
        std::make_shared<ur::VertexInputAttribute>(0, ur::ComponentDataType::Float, 3, 0, 20),
        // uv
        std::make_shared<ur::VertexInputAttribute>(1, ur::ComponentDataType::Float, 2, 12, 20)
    });
}

}