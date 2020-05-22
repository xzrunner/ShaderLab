#pragma once

#include "shaderlab/PreviewViewer.h"

namespace shaderlab
{

class ImageViewer : public PreviewViewer
{
public:
    ImageViewer(const ur::Device& dev);

    virtual void Draw(ur::Context& ctx,
        const void* scene = nullptr) const override;
    virtual void Update(ur::Context& ctx, const std::shared_ptr<ur::ShaderProgram>& shader,
        const std::vector<std::pair<std::string, ur::TexturePtr>>& textures) override;

}; // ImageViewer

}