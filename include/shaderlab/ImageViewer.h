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

class ImageViewer
{
public:
    ImageViewer(const ur::Device& dev);

    void Draw(ur::Context& ctx) const;

    void SetShader(const std::shared_ptr<ur::ShaderProgram>& shader) {
        m_shader = shader;
    }

    auto GetVertexArray() const { return m_va; }

private:
    std::shared_ptr<ur::ShaderProgram> m_default_shader = nullptr;

    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;
    std::shared_ptr<ur::VertexArray>   m_va = nullptr;

}; // ImageViewer

}