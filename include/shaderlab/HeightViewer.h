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

class HeightViewer : public PreviewViewer
{
public:
    HeightViewer(const ur::Device& dev);

    virtual void Draw(ur::Context& ctx,
        const void* scene = nullptr) const override;
    virtual void Update(ur::Context& ctx, const std::shared_ptr<ur::ShaderProgram>& shader,
        const std::vector<std::pair<std::string, ur::TexturePtr>>& textures) override;

    void SetHeightScale(float scale) {
        m_height_scale = scale;
    }

private:
    void InitShader(const ur::Device& dev);
    void InitVertBuf(const ur::Device& dev);

private:
    struct Vertex
    {
        Vertex() {}
        Vertex(size_t ix, size_t iz, size_t size_x, size_t size_z)
        {
            //position = sm::vec3(
            //    ix / static_cast<float>(size_x),
            //    0,
            //    iz / static_cast<float>(size_z)
            //);
            position = sm::vec3(
                ix / static_cast<float>(size_x),
                iz / static_cast<float>(size_z),
                0
            );
            texcoords = sm::vec2(
                ix / static_cast<float>(size_x),
                iz / static_cast<float>(size_z)
            );
        }

        sm::vec3 position;
        sm::vec2 texcoords;
    };

    class HeightMap
    {
    public:
        HeightMap(const ur::Device& dev,
            int width, int height);

        void Update(ur::Context& ctx, const std::shared_ptr<ur::ShaderProgram>& shader,
            const std::vector<std::pair<std::string, ur::TexturePtr>>& textures, ImageViewer& img_viewer);

        auto GetTexture() const { return m_tex; }

    private:
        ur::TexturePtr m_tex = nullptr;
        std::shared_ptr<ur::Framebuffer> m_fbo = nullptr;

        std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;
        std::shared_ptr<ur::VertexArray>   m_va     = nullptr;

    }; // HeightMap

private:
    float m_height_scale = 0.2f;

    sm::vec3 m_light_pos = sm::vec3(1000, 1000, 0);

    HeightMap m_heightmap;

    ImageViewer m_img_viewer;
    std::shared_ptr<ur::ShaderProgram> m_img_shader = nullptr;

}; // HeightViewer

}