#pragma once

#include <memory>

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
    virtual void Draw(ur::Context& ctx,
        const void* scene = nullptr) const = 0;
    virtual void Update(ur::Context& ctx,
        const std::shared_ptr<ur::ShaderProgram>& shader) = 0;

    auto GetShader() const { return m_shader; }
    auto GetVertexArray() const { return m_va; }

protected:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;
    std::shared_ptr<ur::VertexArray>   m_va = nullptr;

}; // PreviewViewer

}