#include "shaderlab/WxGraphPage.h"
#include "shaderlab/MessageID.h"
#include "shaderlab/ShaderAdapter.h"

#include <ee0/WxStageCanvas.h>

#include <boost/filesystem.hpp>

namespace shaderlab
{

WxGraphPage::WxGraphPage(const ur::Device& dev, wxWindow* parent, const ee0::GameObj& root,
                         const ee0::SubjectMgrPtr& preview_sub_mgr)
    : bp::WxGraphPage<shadergraph::Variant>(parent, root, preview_sub_mgr, MSG_SHADER_CHANGED, "shadergraph", "shaderlab")
{
    SetFront2BackCB([&](const bp::Node& front, dag::Node<shadergraph::Variant>& back)
    {
        auto dir = boost::filesystem::path(m_filepath).parent_path().string();
        auto& dev = GetImpl().GetCanvas()->GetRenderDevice();
        ShaderAdapter::Front2Back(front, back, dir, dev);
    });
}

void WxGraphPage::SetCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas)
{
    GetImpl().SetCanvas(canvas);
}

}