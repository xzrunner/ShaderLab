#include "shaderlab/HeightViewer.h"

#include <unirender/Device.h>
#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Uniform.h>
#include <unirender/IndexBuffer.h>
#include <unirender/VertexBuffer.h>
#include <unirender/VertexArray.h>
#include <unirender/ComponentDataType.h>
#include <unirender/DrawState.h>
#include <unirender/VertexInputAttribute.h>
#include <unirender/TextureDescription.h>
#include <unirender/Framebuffer.h>
#include <unirender/Factory.h>
#include <unirender/ClearState.h>
#include <shadertrans/ShaderTrans.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>

//#define ONLY_DRAW_HEIGHTMAP

namespace
{

const char* vs = R"(

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform float u_height_scale;
uniform vec2 u_inv_res;

uniform sampler2D u_heightmap;

out VS_OUT {
    vec3 fragpos;
    vec3 normal;
} vs_out;

vec3 ComputeNormalCentralDifference(vec2 position, float heightExaggeration)
{
    float leftHeight = texture(u_heightmap, position - vec2(1.0, 0.0) * u_inv_res).r * heightExaggeration;
    float rightHeight = texture(u_heightmap, position + vec2(1.0, 0.0) * u_inv_res).r * heightExaggeration;
    float bottomHeight = texture(u_heightmap, position - vec2(0.0, 1.0) * u_inv_res).r * heightExaggeration;
    float topHeight = texture(u_heightmap, position + vec2(0.0, 1.0) * u_inv_res).r * heightExaggeration;
    return normalize(vec3(leftHeight - rightHeight, 2.0, bottomHeight - topHeight));
}

void main()
{
    float h_scale = u_height_scale;

	vec4 pos = vec4(position, 1.0);
	pos.z = texture(u_heightmap, texcoord).r * h_scale;

    vs_out.fragpos = vec3(u_model * pos);
    vs_out.normal = ComputeNormalCentralDifference(texcoord, 500);

	gl_Position = u_projection * u_view * u_model * pos;
}

)";

const char* fs = R"(

#version 330 core
out vec4 FragColor;

uniform vec3 u_light_pos;

in VS_OUT {
    vec3 fragpos;
    vec3 normal;
} fs_in;

void main()
{
    vec3 N = fs_in.normal;

    vec3 light_dir = normalize(u_light_pos - fs_in.fragpos);
    float diff = max(dot(N, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
	FragColor = vec4(diffuse, 1.0);
}

)";

const char* vs_img = R"(
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

const char* fs_img = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, TexCoord);
}
)";

const int HEIGHT_FIELD_SIZE = 512;

}

namespace shaderlab
{

HeightViewer::HeightViewer(const ur::Device& dev)
    : m_heightmap(dev, HEIGHT_FIELD_SIZE, HEIGHT_FIELD_SIZE)
    , m_img_viewer(dev)
{
    InitShader(dev);
    InitVertBuf(dev);
}

void HeightViewer::Draw(ur::Context& ctx, const void* scene) const
{
#ifdef ONLY_DRAW_HEIGHTMAP
    ctx.SetTexture(0, m_heightmap.GetTexture());

    ur::DrawState ds;
    ds.program = m_img_shader;
    ds.render_state = ur::DefaultRenderState2D();
    ds.vertex_array = m_img_viewer.GetVertexArray();
    ctx.Draw(ur::PrimitiveType::Triangles, ds, nullptr);
#else
    ctx.SetTexture(0, m_heightmap.GetTexture());

    ur::DrawState ds;
    ds.program = m_shader;
    ds.render_state.depth_test.enabled = false;
    ds.render_state.facet_culling.enabled = false;
    ds.vertex_array = m_va;
    ctx.Draw(ur::PrimitiveType::Triangles, ds, scene);
#endif // ONLY_DRAW_HEIGHTMAP
}

void HeightViewer::Update(ur::Context& ctx, const std::shared_ptr<ur::ShaderProgram>& shader,
                          const std::vector<std::pair<std::string, ur::TexturePtr>>& textures)
{
    m_heightmap.Update(ctx, shader, textures, m_img_viewer);
    m_img_viewer.Update(ctx, shader, textures);
}

void HeightViewer::InitShader(const ur::Device& dev)
{
    std::vector<unsigned int> _vs, _fs;
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::VertexShader, vs, _vs);
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::PixelShader, fs, _fs);
    m_shader = dev.CreateShaderProgram(_vs, _fs);

    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::VertexShader, vs_img, _vs);
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::PixelShader, fs_img, _fs);
    m_img_shader = dev.CreateShaderProgram(_vs, _fs);

    m_shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*m_shader, "u_model"));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*m_shader, "u_view"));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*m_shader, "u_projection"));

    m_shader->QueryUniform("u_height_scale")->SetValue(&m_height_scale);

    const float inv_res[2] = { 1.0f / HEIGHT_FIELD_SIZE, 1.0f / HEIGHT_FIELD_SIZE };
    auto u_inv_res = m_shader->QueryUniform("u_inv_res");
    u_inv_res->SetValue(inv_res, 2);

    auto u_light_pos = m_shader->QueryUniform("u_light_pos");
    float light_pos[3] = { 1000, 1000, 0 };
    u_light_pos->SetValue(light_pos, 3);
}

void HeightViewer::InitVertBuf(const ur::Device& dev)
{
    m_va = dev.CreateVertexArray();

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
    m_va->SetVertexBuffer(vbuf);

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
    m_va->SetIndexBuffer(ibuf);

    m_va->SetVertexBufferAttrs({
        // pos
        std::make_shared<ur::VertexInputAttribute>(0, ur::ComponentDataType::Float, 3, 0, 20),
        // uv
        std::make_shared<ur::VertexInputAttribute>(1, ur::ComponentDataType::Float, 2, 12, 20)
    });

}

//////////////////////////////////////////////////////////////////////////
// class HeightViewer::HeightMap
//////////////////////////////////////////////////////////////////////////

HeightViewer::HeightMap::
HeightMap(const ur::Device& dev, int width, int height)
{
    // tex
    ur::TextureDescription desc;
    desc.target = ur::TextureTarget::Texture2D;
    desc.width  = width;
    desc.height = height;
    desc.format = ur::TextureFormat::R16;
    m_tex = dev.CreateTexture(desc);

    // rt
    m_fbo = dev.CreateFramebuffer();
    m_fbo->SetAttachment(ur::AttachmentType::Color0, ur::TextureTarget::Texture2D, m_tex, nullptr);

    // va
    m_va = dev.CreateVertexArray();

    auto usage = ur::BufferUsageHint::StaticDraw;

    auto ibuf = dev.CreateIndexBuffer(usage, 0);
    auto ibuf_sz = sizeof(unsigned short) * 6;
    const unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };
    ibuf->Reset(ibuf_sz);
    ibuf->ReadFromMemory(indices, ibuf_sz, 0);
    m_va->SetIndexBuffer(ibuf);

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

    m_va->SetVertexBufferAttrs({
        // pos
        std::make_shared<ur::VertexInputAttribute>(0, ur::ComponentDataType::Float, 2, 0, 16),
        // uv
        std::make_shared<ur::VertexInputAttribute>(1, ur::ComponentDataType::Float, 2, 8, 16)
    });

    m_va->SetVertexBuffer(vbuf);
}

void HeightViewer::HeightMap::
Update(ur::Context& ctx, const std::shared_ptr<ur::ShaderProgram>& shader,
       const std::vector<std::pair<std::string, ur::TexturePtr>>& textures, ImageViewer& img_viewer)
{
    int x, y, w, h;
    ctx.GetViewport(x, y, w, h);

    ctx.SetFramebuffer(m_fbo);
    ctx.SetViewport(0, 0, HEIGHT_FIELD_SIZE, HEIGHT_FIELD_SIZE);

    ur::ClearState clear;
    clear.buffers = ur::ClearBuffers::ColorBuffer;
    clear.color.FromRGBA(0x88000088);
    ctx.Clear(clear);

    img_viewer.Update(ctx, shader, textures);
    img_viewer.Draw(ctx);

    ctx.SetFramebuffer(nullptr);
    ctx.SetViewport(x, y, w, h);
}

}