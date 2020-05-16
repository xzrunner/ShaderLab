#include "shaderlab/Node.h"
#include "shaderlab/PinType.h"
#include "shaderlab/ShaderAdapter.h"

#include <blueprint/Pin.h>
#include <blueprint/BackendAdapter.h>
#include <blueprint/PropDescImpl.h>

#include <shadergraph/VarType.h>
#include <shadergraph/Variant.h>
#include <shadergraph/ValueImpl.h>
#include <shadergraph/Block.h>

namespace
{

auto back2front = [](const dag::Node<shadergraph::Variant>::Port& back) -> bp::PinDesc
{
    bp::PinDesc front;

    front.type = shaderlab::ShaderAdapter::TypeBackToFront(back.var.type.type, 1);
    const_cast<shadergraph::Block::Port&>(back).var.full_name = back.var.type.name;
    front.name = back.var.full_name;

    return front;
};

}

namespace shaderlab
{

Node::Node(const std::string& title)
    : bp::Node(title)
{
}

void Node::Init(const shadergraph::Block& block)
{
    Clear();

    // pins
    bp::BackendAdapter<shadergraph::Variant>
        trans("shadergraph", back2front);
    trans.InitNodePins(*this, block);

    // props
    InitProps(block.GetUniforms());
}

void Node::Init(const std::string& name)
{
    Clear();

    // pins
    bp::BackendAdapter<shadergraph::Variant>
        trans("shadergraph", back2front);
    trans.InitNodePins(*this, name);

    // props
    rttr::type t = rttr::type::get_by_name("shadergraph::" + name);
    if (t.is_valid())
    {
        rttr::variant var = t.create();
        assert(var.is_valid());

        auto method_uniforms = t.get_method("GetUniforms");
        assert(method_uniforms.is_valid());
        auto var_uniforms = method_uniforms.invoke(var);
        assert(var_uniforms.is_valid()
            && var_uniforms.is_type<std::vector<shadergraph::Variant>>());
        auto& uniforms = var_uniforms.get_value<std::vector<shadergraph::Variant>>();
        InitProps(uniforms);
    }
}

void Node::InitProps(const std::vector<shadergraph::Variant>& uniforms)
{
    for (auto& u : uniforms)
    {
        bp::Variant var;
        var.name = u.name;
        assert(u.type == shadergraph::VarType::Uniform);
        auto u_var = std::static_pointer_cast<shadergraph::UniformVal>(u.val)->var;
        switch (u_var.type)
        {
        case shadergraph::VarType::Bool:
            var.type = bp::VarType::Bool;
            var.b = std::static_pointer_cast<shadergraph::BoolVal>(u_var.val)->x;
            break;
        case shadergraph::VarType::Int:
            var.type = bp::VarType::Int;
            var.i = std::static_pointer_cast<shadergraph::IntVal>(u_var.val)->x;
            break;
        case shadergraph::VarType::Float:
            var.type = bp::VarType::Float;
            var.f = std::static_pointer_cast<shadergraph::FloatVal>(u_var.val)->x;
            break;
        case shadergraph::VarType::Float2:
        {
            var.type = bp::VarType::Float2;
            auto src = std::static_pointer_cast<shadergraph::Float2Val>(u_var.val);
            memcpy(var.f2, src->xy, sizeof(var.f2));
        }
            break;
        case shadergraph::VarType::Float3:
        {
            var.type = bp::VarType::Float3;
            auto src = std::static_pointer_cast<shadergraph::Float3Val>(u_var.val);
            memcpy(var.f3, src->xyz, sizeof(var.f3));
        }
            break;
        case shadergraph::VarType::Float4:
        {
            var.type = bp::VarType::Float4;
            auto src = std::static_pointer_cast<shadergraph::Float4Val>(u_var.val);
            memcpy(var.f4, src->xyzw, sizeof(var.f4));
        }
            break;
        default:
            assert(0);
        }

        bp::Node::Property prop;
        prop.var = var;

        for (auto& d : std::static_pointer_cast<shadergraph::UniformVal>(u.val)->desc)
        {
            switch (d->GetType())
            {
            case shadergraph::ParserProp::Type::Enum:
            {
                auto src = std::static_pointer_cast<shadergraph::PropEnum>(d);
                auto dst = std::make_shared<bp::PropEnum>();
                dst->types = src->types;
                prop.descs.push_back(dst);
            }
                break;
            }
        }

        m_props.push_back(prop);
    }
}

}