#include "shaderlab/PreviewPage.h"
#include "shaderlab/WxPreviewCanvas.h"

#include <ee0/WxStagePage.h>
#include <ee3/CameraDriveOP.h>

namespace shaderlab
{

PreviewPage::PreviewPage(const ur::Device& dev, ee0::WxStagePage& stage_page,
                         const ee0::RenderContext& rc)
    : m_stage_page(stage_page)
{
    Init(dev, rc);
}

void PreviewPage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
}

void PreviewPage::Init(const ur::Device& dev, const ee0::RenderContext& rc)
{
    auto canvas = std::make_shared<WxPreviewCanvas>(dev, &m_stage_page, ECS_WORLD_VAR rc);
    m_stage_page.GetImpl().SetCanvas(canvas);

    auto op = std::make_shared<ee3::CameraDriveOP>(
        canvas->GetCamera(), canvas->GetViewport(), m_stage_page.GetSubjectMgr()
    );
    m_stage_page.GetImpl().SetEditOP(op);
}

}