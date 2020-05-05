#include "shaderlab/WxGraphPage.h"
#include "shaderlab/MessageID.h"

#include <ee0/WxStageCanvas.h>

#include <boost/filesystem.hpp>

namespace shaderlab
{

WxGraphPage::WxGraphPage(const ur::Device& dev, wxWindow* parent, const ee0::GameObj& root,
                         const ee0::SubjectMgrPtr& preview_sub_mgr)
    : bp::WxGraphPage<shadergraph::Variant>(parent, root, preview_sub_mgr, MSG_SHADER_CHANGED, "shadergraph", "shaderlab")
{
}

void WxGraphPage::SetCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas)
{
    GetImpl().SetCanvas(canvas);
}

void WxGraphPage::OnEvalChangeed()
{
    //std::vector<bp::NodePtr> nodes;
    //Traverse([&](const ee0::GameObj& obj)->bool
    //{
    //    if (obj->HasUniqueComp<bp::CompNode>())
    //    {
    //        auto bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
    //        if (bp_node) {
    //            nodes.push_back(bp_node);
    //        }
    //    }
    //    return true;
    //});

    //auto str = m_front_eval.Rebuild(nodes, *m_eval);

}

}