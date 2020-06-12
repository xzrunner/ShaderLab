#include "shaderlab/WxPreviewCanvas.h"
#include "shaderlab/MessageID.h"
#include "shaderlab/WxGraphPage.h"
#include "shaderlab/ImageViewer.h"
#include "shaderlab/HeightViewer.h"
#include "shaderlab/Node.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/Node.h>

#include <unirender/Device.h>
#include <unirender/ShaderProgram.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>
#include <shadergraph/block/FragmentShader.h>

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
    if (shader)
    {
        auto model_updater = shader->QueryUniformUpdater(ur::GetUpdaterTypeID<pt0::ModelMatUpdater>());
        if (model_updater) {
            std::static_pointer_cast<pt0::ModelMatUpdater>(model_updater)->Update(sm::mat4());
        }
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

    auto root = m_graph_page->GetSceneTree()->GetRootNode();
    assert(root->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
    nodes.reserve(ccomplex.GetAllChildren().size());
    for (auto& c : ccomplex.GetAllChildren()) {
        if (c->HasUniqueComp<bp::CompNode>()) {
            nodes.push_back(c->GetUniqueComp<bp::CompNode>().GetNode());
        }
    }

    auto textures = m_eval.QueryTextures(nodes);

    auto shader = m_eval.BuildShader(m_dev, vs, nodes);
    for (auto& viewer : m_viewers) {
        viewer->Update(*GetRenderContext().ur_ctx, shader, textures);
    }

    for (auto& bp_node : nodes) {
        BuildNodePreviewShader(bp_node);
    }
}

void WxPreviewCanvas::BuildNodePreviewShader(const bp::NodePtr& bp_node) const
{
    if (!bp_node->get_type().is_derived_from<shaderlab::Node>()) {
        return;
    }

    auto node = std::static_pointer_cast<Node>(bp_node);
    if (!node->GetPreview()) {
        return;
    }

    auto back_node = m_front_eval->QueryBackNode(*node);
    if (!back_node) {
        return;
    }


    auto& src_outputs = back_node->GetExports();
    if (src_outputs.empty()) {
        return;
    }

    int conn_fs_idx = -1;
    switch (src_outputs[0].var.type.type)
    {
    case shadergraph::VarType::Float4:
        conn_fs_idx = static_cast<int>(shadergraph::block::FragmentShader::Input::RGBA);
        break;
    case shadergraph::VarType::Float3:
        conn_fs_idx = static_cast<int>(shadergraph::block::FragmentShader::Input::RGB);
        break;
    case shadergraph::VarType::Float2:
        conn_fs_idx = static_cast<int>(shadergraph::block::FragmentShader::Input::RG);
        break;
    case shadergraph::VarType::Float:
        conn_fs_idx = static_cast<int>(shadergraph::block::FragmentShader::Input::Grey);
        break;
    }
    if (conn_fs_idx < 0) {
        return;
    }

    auto fs_out = std::make_shared<shadergraph::block::FragmentShader>();
    dag::make_connecting<shadergraph::Variant>({ back_node, 0 }, { fs_out, conn_fs_idx });

    shadergraph::Evaluator back_eval;
    back_eval.Rebuild(fs_out);

    back_eval.Rebuild(fs_out);
    auto fs = back_eval.GenShaderCode();
    if (fs.empty()) {
        return;
    }

    auto shader = m_dev.CreateShaderProgram(vs, fs);
    if (!shader->CheckStatus()) {
        return;
    }

    dag::disconnect<shadergraph::Variant>({ back_node, 0 }, { fs_out, conn_fs_idx });

    node->SetPreviewShader(shader);
}

}