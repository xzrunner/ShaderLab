#pragma once

#include "shaderlab/PreviewViewer.h"
#include "shaderlab/ImageViewer.h"

#include <SM_Vector.h>
#include <unirender/typedef.h>
#include <painting0/Camera.h>

#include <memory>

namespace pt3 { class WindowContext; }
namespace ur { class Framebuffer; }

namespace shaderlab
{

class ModelViewer : public PreviewViewer
{
public:
    ModelViewer(const ur::Device& dev);

    virtual void Draw(ur::Context& ctx, const pt0::CameraPtr& cam,
        const void* scene = nullptr) const override;
    virtual void Update(ur::Context& ctx, const pt0::CameraPtr& cam, 
        const std::shared_ptr<ur::ShaderProgram>& shader,
        const std::vector<std::pair<std::string, ur::TexturePtr>>& textures) override;

    virtual std::string GetVertShaderCode() const;

private:
    void InitShader(const ur::Device& dev);
    void InitVertBuf(const ur::Device& dev);

private:
    sm::vec3 m_light_pos = sm::vec3(1000, 1000, 0);

}; // ModelViewer

}