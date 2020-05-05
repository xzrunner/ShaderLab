#pragma once

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
        m_eval = eval;
    }

protected:
    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;
    virtual void DrawForeground2D() const override;

private:
    void InitRenderer(const ur::Device& dev);

    void RebuildShader();

private:
    std::shared_ptr<bp::BackendGraph<shadergraph::Variant>> m_eval = nullptr;

    const WxGraphPage* m_graph_page = nullptr;

    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;
    std::shared_ptr<ur::VertexArray>   m_va = nullptr;

}; // WxPreviewCanvas


}