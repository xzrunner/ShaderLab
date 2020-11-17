#pragma once

#include <dag/Node.h>
#include <unirender/typedef.h>
#include <shadergraph/VarType.h>
#include <shadergraph/Variant.h>

namespace bp { class Node; }
namespace ur { class Device; class ShaderProgram; }
namespace shadergraph { class Block; }

namespace shaderlab
{

class ShaderAdapter
{
public:
    static int TypeBackToFront(shadergraph::VarType type, int count);

    static void Front2Back(const bp::Node& front, dag::Node<shadergraph::Variant>& back,
        const std::string& dir, const ur::Device& dev);

    static const uint32_t TIME_UPDATER_ID      = 0x0001;
    static const uint32_t MODEL_MAT_UPDATER_ID = 0x0002;
    static const uint32_t VIEW_MAT_UPDATER_ID  = 0x0004;
    static const uint32_t PROJ_MAT_UPDATER_ID  = 0x0008;
    static void BuildShaderCode(const std::string& filepath, const ur::Device& dev, std::string& vs, 
        std::string& fs, std::vector<std::pair<std::string, ur::TexturePtr>>& textures, uint32_t& updaters);

    static void InitShaderUpdaters(ur::ShaderProgram& prog, uint32_t updaters);

}; // ShaderAdapter

}