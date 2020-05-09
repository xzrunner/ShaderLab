#include "shaderlab/WxPreviewCanvas.h"
#include "shaderlab/MessageID.h"
#include "shaderlab/WxGraphPage.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>

#include <unirender/Device.h>
#include <unirender/Context.h>
#include <unirender/IndexBuffer.h>
#include <unirender/VertexBuffer.h>
#include <unirender/VertexArray.h>
#include <unirender/VertexBufferAttribute.h>
#include <unirender/ComponentDataType.h>
#include <unirender/DrawState.h>
#include <unirender/Factory.h>

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

WxPreviewCanvas::WxPreviewCanvas(const ur::Device& dev, ee0::WxStagePage* stage,
                                 ECS_WORLD_PARAM const ee0::RenderContext& rc)
    : ee3::WxStageCanvas(dev, stage, ECS_WORLD_VAR &rc, nullptr, true)
{
    auto sub_mgr = stage->GetSubjectMgr();
    sub_mgr->RegisterObserver(MSG_SHADER_CHANGED, this);

    InitRenderer(dev);
}

WxPreviewCanvas::~WxPreviewCanvas()
{
    if (m_graph_page)
    {
        auto sub_mgr = m_graph_page->GetSubjectMgr();
        sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    }

    auto sub_mgr = m_stage->GetSubjectMgr();
    sub_mgr->UnregisterObserver(MSG_SHADER_CHANGED, this);
}

void WxPreviewCanvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
    ee3::WxStageCanvas::OnNotify(msg, variants);

	switch (msg)
	{
    case MSG_SHADER_CHANGED:
        RebuildShader();
        SetDirty();
        break;
	}
}

void WxPreviewCanvas::DrawBackground3D() const
{
//    ee3::WxStageCanvas::DrawBackgroundGrids(10.0f, 0.2f);
//    ee3::WxStageCanvas::DrawBackgroundCross();
}

void WxPreviewCanvas::DrawForeground3D() const
{
    if (!m_graph_page) {
        return;
    }
}

void WxPreviewCanvas::DrawForeground2D() const
{
    ur::DrawState ds;
    ds.program = m_shader;
    ds.render_state = ur::DefaultRenderState2D();
    ds.vertex_array = m_va;

    GetRenderContext().ur_ctx->Draw(ur::PrimitiveType::Triangles, ds, nullptr);
}

void WxPreviewCanvas::InitRenderer(const ur::Device& dev)
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

void WxPreviewCanvas::RebuildShader()
{
    std::vector<bp::NodePtr> nodes;
    m_graph_page->Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (obj->HasUniqueComp<bp::CompNode>())
        {
            auto bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
            if (bp_node) {
                nodes.push_back(bp_node);
            }
        }
        return true;
    });

    m_shader = Evaluator::BuildShader(m_dev, vs, nodes, *m_eval);
}

}