#include "shaderlab/WxPreviewCanvas.h"
#include "shaderlab/MessageID.h"
#include "shaderlab/WxGraphPage.h"
#include "shaderlab/ImageViewer.h"
#include "shaderlab/HeightViewer.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>

#include <unirender/ShaderProgram.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>

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

}

namespace shaderlab
{

WxPreviewCanvas::WxPreviewCanvas(const ur::Device& dev, ee0::WxStagePage* stage,
                                 ECS_WORLD_PARAM const ee0::RenderContext& rc)
    : ee3::WxStageCanvas(dev, stage, ECS_WORLD_VAR &rc, nullptr, true)
{
    m_viewers[VIEWER_IMAGE] = std::make_shared<ImageViewer>(dev);
    m_viewers[VIEWER_HEIGHT] = std::make_shared<HeightViewer>(dev);

    auto sub_mgr = stage->GetSubjectMgr();
    sub_mgr->RegisterObserver(MSG_SHADER_CHANGED, this);
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
    auto shader = m_viewers[m_viewer_type]->GetShader();
    auto model_updater = shader->QueryUniformUpdater(ur::GetUpdaterTypeID<pt0::ModelMatUpdater>());
    if (model_updater) {
        std::static_pointer_cast<pt0::ModelMatUpdater>(model_updater)->Update(sm::mat4());
    }
    m_viewers[m_viewer_type]->Draw(*GetRenderContext().ur_ctx, GetWidnowContext().wc3.get());
}

void WxPreviewCanvas::OnTimer()
{
    m_eval.UpdateUniforms();

    SetDirty();
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

    auto shader = m_eval.BuildShader(m_dev, vs, nodes);
    for (auto& viewer : m_viewers) {
        viewer->Update(*GetRenderContext().ur_ctx, shader);
    }
}

}