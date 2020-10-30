#include "shaderlab/WxGraphPage.h"
#include "shaderlab/MessageID.h"
#include "shaderlab/ShaderAdapter.h"
#include "shaderlab/RegistNodes.h"
#include "shaderlab/node/SubGraph.h"

#include <ee0/WxStageCanvas.h>

#include <boost/filesystem.hpp>

namespace shaderlab
{

WxGraphPage::WxGraphPage(const ur::Device& dev, wxWindow* parent, const ee0::GameObj& root,
                         const ee0::SubjectMgrPtr& preview_sub_mgr)
    : bp::WxGraphPage<shadergraph::Variant>(parent, root, preview_sub_mgr, MSG_SHADER_CHANGED, "shadergraph", "shaderlab",
        [&](const bp::Node& front, dag::Node<shadergraph::Variant>& back) {
    auto dir = boost::filesystem::path(m_filepath).parent_path().string();
    auto& dev = GetImpl().GetCanvas()->GetRenderDevice();
    ShaderAdapter::Front2Back(front, back, dir, dev);
})
{
}

void WxGraphPage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
    bp::WxGraphPage<shadergraph::Variant>::OnNotify(msg, variants);

    switch (msg)
    {
    case ee0::MSG_SCENE_NODE_INSERT:
    case ee0::MSG_SCENE_NODE_DELETE:
        if (IsNodeInputOrOutput(variants)) {
            SetupSubGraphPorts();
        }
        break;
    case ee0::MSG_SCENE_NODE_CLEAR:
    case bp::MSG_BP_NODE_PROP_CHANGED:
        SetupSubGraphPorts();
        break;
    }
}

void WxGraphPage::SetCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas)
{
    GetImpl().SetCanvas(canvas);
}

bool WxGraphPage::IsNodeInputOrOutput(const ee0::VariantSet& variants) const
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");
    if (!(*obj)->HasUniqueComp<bp::CompNode>()) {
        return false;
    }

    auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();
    auto type = bp_node->get_type();
    return type == rttr::type::get<node::Input>() 
        || type == rttr::type::get<node::Output>();
}

void WxGraphPage::SetupSubGraphPorts()
{
    auto root = GetSceneTree()->GetCurrNode();
    assert(root);
    if (!root->HasUniqueComp<bp::CompNode>()) {
        return;
    }
    auto& bp_node = root->GetUniqueComp<bp::CompNode>().GetNode();
    if (bp_node->get_type() != rttr::type::get<node::SubGraph>()) {
        return;
    }

    std::vector<shadergraph::Variant> inputs, outputs;
    Traverse([&](const ee0::GameObj& obj)->bool
	{
        if (obj->HasUniqueComp<bp::CompNode>())
        {
            auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
            auto type = bp_node->get_type();
            if (type == rttr::type::get<node::Input>()) 
            {
                auto input = std::static_pointer_cast<node::Input>(bp_node);
                inputs.push_back({ input->m_var_type, input->m_var_name });
            } 
            else if (type == rttr::type::get<node::Output>()) 
            {
                auto output = std::static_pointer_cast<node::Output>(bp_node);
                outputs.push_back({ output->m_var_type, output->m_var_name });
            }
        }
		return true;
	});

    auto subgraph = std::static_pointer_cast<node::SubGraph>(bp_node);
    if (subgraph->Setup(inputs, outputs)) 
    {
        auto& caabb = root->GetUniqueComp<n2::CompBoundingBox>();
        auto& style = bp_node->GetStyle();
        caabb.SetSize(*root, sm::rect(style.width, style.height));
    }
}

}