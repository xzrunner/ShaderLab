#include "shaderlab/NodePreview.h"
#include "shaderlab/Node.h"

#include <blueprint/NodeHelper.h>

#include <unirender/Device.h>
#include <unirender/Context.h>
#include <unirender/Factory.h>

#include <unirender/DrawState.h>
#include <painting2/RenderTargetMgr.h>
#include <painting2/RenderSystem.h>
#include <renderpipeline/RenderMgr.h>
#include <node2/RenderSystem.h>

namespace
{

const size_t TEX_SIZE = 1024;

}

namespace shaderlab
{

void NodePreview::Draw(const ur::Device& dev, ur::Context& ctx, const bp::Node& front_node,
                       const shadergraph::BlockPtr& back_node, const n2::RenderParams& rp)
{
    auto fbo = pt2::RenderTargetMgr::Instance()->Fetch(dev);

    rp::RenderMgr::Instance()->Flush(dev, ctx);

    ctx.SetFramebuffer(fbo);
    int x, y, w, h;
    ctx.GetViewport(x, y, w, h);
    ctx.SetViewport(0, 0, TEX_SIZE, TEX_SIZE);
    bool succ = DrawToRT(dev, ctx, front_node, back_node);
    ctx.SetViewport(x, y, w, h);
    ctx.SetFramebuffer(nullptr);

    if (succ) {
        auto tex = pt2::RenderTargetMgr::Instance()->GetBindedTex(fbo);
        assert(tex);
        DrawFromRT(dev, ctx, front_node, rp, tex);
    }

    pt2::RenderTargetMgr::Instance()->Return(fbo);
}

bool NodePreview::DrawToRT(const ur::Device& dev, ur::Context& ctx, const bp::Node& front_node,
                           const shadergraph::BlockPtr& back_node)
{
    auto shader = static_cast<const Node&>(front_node).GetPreviewShader();
    if (!shader) {
        return false;
    }

    ur::DrawState ds;
    ds.program = shader;
    ds.render_state = ur::DefaultRenderState2D();
    ds.vertex_array = dev.GetVertexArray(ur::Device::PrimitiveType::Quad, ur::VertexLayoutType::PosTex);
    ctx.Draw(ur::PrimitiveType::TriangleStrip, ds, nullptr);
    return true;
}

void NodePreview::DrawFromRT(const ur::Device& dev, ur::Context& ctx, const bp::Node& front_node,
                             const n2::RenderParams& rp, const ur::TexturePtr& tex)
{
    auto mt4 = sm::mat4(bp::NodeHelper::CalcPreviewMat(front_node, rp.GetMatrix()));
    const auto scale = mt4.GetScale();
    const float hw = scale.x * 0.5f;
    const float hh = scale.y * 0.5f;
    const auto pos = mt4.GetTranslate();
    const float vertices[] = {
        pos.x - hw, pos.y - hh,
        pos.x + hw, pos.y - hh,
        pos.x + hw, pos.y + hh,
        pos.x - hw, pos.y + hh,
    };
    const float texcoords[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1
    };

    auto rs = ur::DefaultRenderState2D();
    pt2::RenderSystem::DrawTexQuad(dev, ctx, rs, vertices, texcoords, tex, 0xffffffff);
}

}