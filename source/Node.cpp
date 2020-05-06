#include "shaderlab/Node.h"
#include "shaderlab/PinType.h"
#include "shaderlab/ShaderAdapter.h"

#include <blueprint/Pin.h>
#include <blueprint/BackendAdapter.h>

#include <shadergraph/VarType.h>
#include <shadergraph/Variant.h>
#include <shadergraph/Block.h>

namespace shaderlab
{

Node::Node(const std::string& title)
    : bp::Node(title)
{
}

void Node::InitPins(const std::string& name)
{
    auto back2front = [](const dag::Node<shadergraph::Variant>::Port& back) -> bp::PinDesc
    {
        bp::PinDesc front;

        front.type = ShaderAdapter::TypeBackToFront(back.var.type.type, 1);
        const_cast<shadergraph::Block::Port&>(back).var.full_name = back.var.type.name;
        front.name = back.var.full_name;

        return front;
    };

    bp::BackendAdapter<shadergraph::Variant>
        trans("shadergraph", back2front);
    trans.InitNodePins(*this, name);
}

}