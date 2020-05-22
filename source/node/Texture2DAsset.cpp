#include "shaderlab/node/Texture2DAsset.h"

#include <blueprint/NodeHelper.h>

#include <unirender/RenderState.h>
#include <unirender/Factory.h>
#include <painting2/RenderSystem.h>
#include <node2/RenderSystem.h>
#include <facade/ImageLoader.h>

namespace shaderlab
{
namespace node
{

void Texture2DAsset::Draw(const ur::Device& dev, ur::Context& ctx,
                          const n2::RenderParams& rp) const
{
    bp::Node::Draw(dev, ctx, rp);

    if (m_tex)
    {
        auto model_mat = bp::NodeHelper::CalcPreviewMat(*this, rp.GetMatrix());
        auto rs = ur::DefaultRenderState2D();
        pt2::RenderSystem::DrawTexture(dev, ctx, rs, m_tex, sm::rect(1, 1), model_mat);
    }
}

void Texture2DAsset::UpdateTexture(const ur::Device& dev)
{
    if (!m_filepath.empty())
    {
        facade::ImageLoader loader(m_filepath);
        loader.Load(dev);

        m_tex = loader.GetTexture();
    }
}

}
}