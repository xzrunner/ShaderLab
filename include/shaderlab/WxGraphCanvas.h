#pragma once

#include <ee2/WxStageCanvas.h>

namespace shaderlab
{

class WxGraphPage;

class WxGraphCanvas : public ee2::WxStageCanvas
{
public:
    WxGraphCanvas(const ur::Device& dev, WxGraphPage* stage,
        ECS_WORLD_PARAM const ee0::RenderContext& rc);

protected:
    virtual void DrawForeground() const;

    virtual bool OnUpdate() override;

private:
    WxGraphPage* m_stage = nullptr;

}; // WxGraphCanvas

}