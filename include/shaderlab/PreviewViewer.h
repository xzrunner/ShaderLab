#pragma once

#include <unirender/typedef.h>
#include <painting0/Camera.h>

#include <memory>
#include <vector>

namespace ur {
    class Device;
    class Context;
    class ShaderProgram;
    class VertexArray;
}

namespace shaderlab
{

class PreviewViewer
{
public:
    virtual void Draw(ur::Context& ctx, const pt0::CameraPtr& cam,
        const void* scene = nullptr) const = 0;
    virtual void Update(ur::Context& ctx, const pt0::CameraPtr& cam, 
        const std::shared_ptr<ur::ShaderProgram>& shader,
        const std::vector<std::pair<std::string, ur::TexturePtr>>& textures) = 0;

    virtual std::string GetVertShaderCode() const = 0;

    auto GetShader() const { return m_shader; }
    auto GetVertexArray() const { return m_va; }

protected:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;
    std::shared_ptr<ur::VertexArray>   m_va = nullptr;

    std::vector<std::pair<size_t, ur::TexturePtr>> m_textures;

}; // PreviewViewer

}