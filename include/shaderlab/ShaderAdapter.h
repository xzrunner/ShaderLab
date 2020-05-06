#pragma once

#include <dag/Node.h>
#include <shadergraph/VarType.h>
#include <shadergraph/Variant.h>

namespace bp { class Node; }

namespace shaderlab
{

class ShaderAdapter
{
public:
    static int TypeBackToFront(shadergraph::VarType type, int count);

    static void Front2Back(const bp::Node& front,
        dag::Node<shadergraph::Variant>& back, const std::string& dir);

}; // ShaderAdapter

}