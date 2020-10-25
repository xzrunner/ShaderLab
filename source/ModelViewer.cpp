#include "shaderlab/ModelViewer.h"

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

namespace
{

const char* vs = R"(

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

out vec2 TexCoord;

layout(std140) uniform UBO_VS
{
    mat4 projection;
    mat4 view;
    mat4 model;
} ubo_vs;

void main()
{
    TexCoord = vec2(texcoord.x, texcoord.y);

	vec4 pos = vec4(position, 1.0);
	gl_Position = ubo_vs.projection * ubo_vs.view * ubo_vs.model * pos;
}

)";

const char* fs = R"(

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;

void main()
{
	FragColor = texture(texture0, TexCoord);
}

)";

}

namespace shaderlab
{

ModelViewer::ModelViewer(const ur::Device& dev)
{
    InitShader(dev);
    InitVertBuf(dev);
}

void ModelViewer::Draw(ur::Context& ctx, const void* scene) const
{
    ur::DrawState ds;
    ds.program = m_shader;
    ds.render_state.depth_test.enabled = false;
    ds.render_state.facet_culling.enabled = false;
    ds.vertex_array = m_va;

    for (auto& t : m_textures) {
        ctx.SetTexture(t.first, t.second);
    }

    ctx.Draw(ur::PrimitiveType::Triangles, ds, scene);
}

void ModelViewer::Update(ur::Context& ctx, const std::shared_ptr<ur::ShaderProgram>& shader,
                         const std::vector<std::pair<std::string, ur::TexturePtr>>& textures)
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

    m_shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*m_shader, "ubo_vs.model"));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*m_shader, "ubo_vs.view"));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*m_shader, "ubo_vs.projection"));
}

const char* ModelViewer::GetVertShaderCode() const
{
    return vs;
}

void ModelViewer::InitShader(const ur::Device& dev)
{
    std::vector<unsigned int> _vs, _fs;
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::VertexShader, vs, _vs);
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::PixelShader, fs, _fs);
    m_shader = dev.CreateShaderProgram(_vs, _fs);
    if (!m_shader || !m_shader->CheckStatus()) {
        return;
    }

    m_shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*m_shader, "ubo_vs.model"));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*m_shader, "ubo_vs.view"));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*m_shader, "ubo_vs.projection"));

    //m_shader->QueryUniform("ubo_vs.height_scale")->SetValue(&m_height_scale);

    //const float inv_res[2] = { 1.0f / HEIGHT_FIELD_SIZE, 1.0f / HEIGHT_FIELD_SIZE };
    //auto u_inv_res = m_shader->QueryUniform("ubo_vs.inv_res");
    //u_inv_res->SetValue(inv_res, 2);

    //auto u_light_pos = m_shader->QueryUniform("ubo_fs.light_pos");
    //float light_pos[3] = { 1000, 1000, 0 };
    //u_light_pos->SetValue(light_pos, 3);
}

void ModelViewer::InitVertBuf(const ur::Device& dev)
{
    m_va = dev.CreateVertexArray();

    auto usage = ur::BufferUsageHint::StaticDraw;

    auto ibuf = dev.CreateIndexBuffer(usage, 0);
    auto ibuf_sz = sizeof(unsigned short) * 6;
    const unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };
    ibuf->Reset(ibuf_sz);
    ibuf->ReadFromMemory(indices, ibuf_sz, 0);
    m_va->SetIndexBuffer(ibuf);

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

    m_va->SetVertexBufferAttrs({
        // pos
        std::make_shared<ur::VertexInputAttribute>(0, ur::ComponentDataType::Float, 3, 0, 20),
        // uv
        std::make_shared<ur::VertexInputAttribute>(1, ur::ComponentDataType::Float, 2, 12, 20)
    });

    m_va->SetVertexBuffer(vbuf);
}

}