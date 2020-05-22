#pragma once

#include "shaderlab/Node.h"

#include <unirender/typedef.h>

namespace shaderlab
{
namespace node
{

class Texture2DAsset : public Node
{
public:
    Texture2DAsset()
        : Node("Texture2DAsset")
    {
        Init("texture2d_asset");
    }

    virtual void Draw(const ur::Device& dev, ur::Context& ctx,
        const n2::RenderParams& rp) const override;

    void SetFilepath(const std::string& filepath) { m_filepath = filepath; }
    const std::string& GetFilepath() const { return m_filepath; }

    void UpdateTexture(const ur::Device& dev);

    auto GetTexture() const { return m_tex; }

private:
    std::string m_filepath;

    ur::TexturePtr m_tex = nullptr;

    RTTR_ENABLE(Node)

}; // Texture2DAsset

}
}