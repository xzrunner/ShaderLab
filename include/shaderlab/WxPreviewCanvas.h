#pragma once

#include "shaderlab/Evaluator.h"
#include "shaderlab/ImageViewer.h"
#include "shaderlab/HeightViewer.h"

#include <ee3/WxStageCanvas.h>
#include <blueprint/BackendGraph.h>

#include <shadergraph/Variant.h>

namespace ee0 { class WxStagePage; }
namespace ur { class Device; class VertexArray; }

namespace shaderlab
{

class WxGraphPage;

class WxPreviewCanvas : public ee3::WxStageCanvas
{
public:
    WxPreviewCanvas(const ur::Device& dev, ee0::WxStagePage* stage,
        ECS_WORLD_PARAM const ee0::RenderContext& rc);
    virtual ~WxPreviewCanvas();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    void SetGraphPage(const WxGraphPage* graph_page) {
        m_graph_page = graph_page;
    }

    void SetEval(const std::shared_ptr<bp::BackendGraph<shadergraph::Variant>>& eval) {
        m_front_eval = eval;
        m_eval.SetFrontEval(eval);
    }

protected:
    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;
    virtual void DrawForeground2D() const override;

    virtual void OnTimer() override;

private:
    void RebuildShader();

private:
    //ImageViewer m_viewer;
    HeightViewer m_viewer;

    std::shared_ptr<bp::BackendGraph<shadergraph::Variant>> m_front_eval = nullptr;
    Evaluator m_eval;

    const WxGraphPage* m_graph_page = nullptr;

}; // WxPreviewCanvas


}