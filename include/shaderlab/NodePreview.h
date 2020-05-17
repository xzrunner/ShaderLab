#pragma once

#include <SM_Matrix2D.h>
#include <unirender/typedef.h>
#include <shadergraph/typedef.h>

namespace ur { class Device; class Context; class ShaderProgram; }
namespace bp { class Node; }
namespace n2 { class RenderParams; }

namespace shaderlab
{

class NodePreview
{
public:
    static void Draw(const ur::Device& dev, ur::Context& ctx,
        const bp::Node& front_node, const shadergraph::BlockPtr& back_node,
        const n2::RenderParams& rp);

private:
    static bool DrawToRT(const ur::Device& dev, ur::Context& ctx, const bp::Node& front_node,
        const shadergraph::BlockPtr& back_node);
    static void DrawFromRT(const ur::Device& dev, ur::Context& ctx,
        const bp::Node& front_node, const n2::RenderParams& rp, const ur::TexturePtr& tex);

}; // NodePreview

}