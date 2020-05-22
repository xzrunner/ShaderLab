#include "shaderlab/ImageViewer.h"

#include <SM_Vector.h>
#include <unirender/Device.h>
#include <unirender/Context.h>
#include <unirender/IndexBuffer.h>
#include <unirender/VertexBuffer.h>
#include <unirender/VertexArray.h>
#include <unirender/VertexBufferAttribute.h>
#include <unirender/ComponentDataType.h>
#include <unirender/DrawState.h>
#include <unirender/Factory.h>
#include <unirender/ShaderProgram.h>

namespace
{

const char* vs = R"(
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

const char* fs = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
//	FragColor = texture(texture1, TexCoord);
	FragColor = vec4(TexCoord, 0, 1);
}
)";

}

namespace shaderlab
{

ImageViewer::ImageViewer(const ur::Device& dev)
{
    m_shader = dev.CreateShaderProgram(vs, fs);
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
        std::make_shared<ur::VertexBufferAttribute>(ur::ComponentDataType::Float, 2, 0, 16),
        // uv
        std::make_shared<ur::VertexBufferAttribute>(ur::ComponentDataType::Float, 2, 8, 16)
    });

    m_va->SetVertexBuffer(vbuf);
}

void ImageViewer::Draw(ur::Context& ctx, const void* scene) const
{
    ur::DrawState ds;
    ds.program      = m_shader;
    ds.render_state = ur::DefaultRenderState2D();
    ds.vertex_array = m_va;

    for (auto& t : m_textures) {
        ctx.SetTexture(t.first, t.second);
    }

    ctx.Draw(ur::PrimitiveType::Triangles, ds, nullptr);
}

void ImageViewer::Update(ur::Context& ctx, const std::shared_ptr<ur::ShaderProgram>& shader,
                         const std::vector<std::pair<std::string, ur::TexturePtr>>& textures)
{
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
}

}