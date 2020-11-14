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
    enum class VertexArray
    {
        Rect2D,
        Rect3D,
        Grids3D,
    };

    enum class RenderState
    {
        _2D,
        _3D,
    };

public:
    PreviewViewer(const ur::Device& dev);

    void Draw(ur::Context& ctx, const pt0::CameraPtr& cam,
        const void* scene = nullptr) const;
    void Update(ur::Context& ctx, const pt0::CameraPtr& cam,
        const std::shared_ptr<ur::ShaderProgram>& shader,
        const std::vector<std::pair<std::string, ur::TexturePtr>>& textures);

    std::string GetVertShaderCode() const;

    void SetVertexArray(VertexArray va) { m_va = va; }
    void SetRenderState(RenderState rs) { m_rs = rs; }

private:
    void InitRect2dVA(const ur::Device& dev);
    void InitRect3dVA(const ur::Device& dev);
    void InitGrids3dVA(const ur::Device& dev);

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    std::shared_ptr<ur::VertexArray> m_rect_2d_va = nullptr;
    std::shared_ptr<ur::VertexArray> m_rect_3d_va = nullptr;
    std::shared_ptr<ur::VertexArray> m_grids_3d_va = nullptr;

    std::vector<std::pair<size_t, ur::TexturePtr>> m_textures;

    VertexArray m_va = VertexArray::Rect2D;
    RenderState m_rs = RenderState::_2D;

}; // PreviewViewer

}