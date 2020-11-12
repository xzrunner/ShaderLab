#include "shaderlab/Node.h"
#include "shaderlab/PinType.h"
#include "shaderlab/ShaderAdapter.h"

#include <blueprint/Pin.h>
#include <blueprint/PropDescImpl.h>

#include <shadergraph/VarType.h>
#include <shadergraph/Variant.h>
#include <shadergraph/ValueImpl.h>
#include <shadergraph/Block.h>

namespace shaderlab
{

Node::Node(const std::string& title)
    : bp::Node(title)
{
}

void Node::Init(const std::string& name)
{
    // pins
    bp::BackendAdapter<shadergraph::Variant>
        trans("shadergraph", BackToFront);
    trans.InitNodePins(*this, name);

    // props
    rttr::type t = rttr::type::get_by_name("shadergraph::" + name);
    if (t.is_valid())
    {
        rttr::variant var = t.create();
        assert(var.is_valid());

        auto method_vars = t.get_method("GetVariants");
        assert(method_vars.is_valid());
        auto var_vars = method_vars.invoke(var);
        assert(var_vars.is_valid()
            && var_vars.is_type<std::vector<shadergraph::Variant>>());
        auto& vars = var_vars.get_value<std::vector<shadergraph::Variant>>();
        InitProps(vars);
    }
}

void Node::InitProps(const std::vector<shadergraph::Variant>& vars)
{
    for (auto& v : vars)
    {
        if (v.type != shadergraph::VarType::Uniform) {
            continue;
        }

        bp::Variant var;
        var.name = v.name;
        auto u_var = std::static_pointer_cast<shadergraph::UniformVal>(v.val)->var;
        // todo
        if (u_var.type == shadergraph::VarType::Array) {
            continue;
        }
        switch (u_var.type)
        {
        case shadergraph::VarType::Bool:
            var.type = bp::VarType::Bool;
            if (u_var.val) {
                var.b = std::static_pointer_cast<shadergraph::BoolVal>(u_var.val)->x;
            }
            break;
        case shadergraph::VarType::Int:
            var.type = bp::VarType::Int;
            if (u_var.val) {
                var.i = std::static_pointer_cast<shadergraph::IntVal>(u_var.val)->x;
            }
            break;
        case shadergraph::VarType::Float:
            var.type = bp::VarType::Float;
            if (u_var.val) {
                var.f = std::static_pointer_cast<shadergraph::FloatVal>(u_var.val)->x;
            }
            break;
        case shadergraph::VarType::Float2:
        {
            var.type = bp::VarType::Float2;
            if (u_var.val) {
                auto src = std::static_pointer_cast<shadergraph::Float2Val>(u_var.val);
                memcpy(var.f2, src->xy, sizeof(var.f2));
            }
        }
            break;
        case shadergraph::VarType::Float3:
        {
            var.type = bp::VarType::Float3;
            if (u_var.val) {
                auto src = std::static_pointer_cast<shadergraph::Float3Val>(u_var.val);
                memcpy(var.f3, src->xyz, sizeof(var.f3));
            }
        }
            break;
        case shadergraph::VarType::Float4:
        {
            var.type = bp::VarType::Float4;
            if (u_var.val) {
                auto src = std::static_pointer_cast<shadergraph::Float4Val>(u_var.val);
                memcpy(var.f4, src->xyzw, sizeof(var.f4));
            }
        }
            break;
        case shadergraph::VarType::Matrix2:
        {
            var.type = bp::VarType::Matrix2;
            if (u_var.val) {
                auto src = std::static_pointer_cast<shadergraph::Matrix2Val>(u_var.val);
                memcpy(var.m2, src->m, sizeof(var.m2));
            }
        }
            break;
        case shadergraph::VarType::Matrix3:
        {
            var.type = bp::VarType::Matrix3;
            if (u_var.val) {
                auto src = std::static_pointer_cast<shadergraph::Matrix3Val>(u_var.val);
                memcpy(var.m3, src->m, sizeof(var.m3));
            }
        }
            break;
        case shadergraph::VarType::Matrix4:
        {
            var.type = bp::VarType::Matrix4;
            if (u_var.val) {
                auto src = std::static_pointer_cast<shadergraph::Matrix4Val>(u_var.val);
                memcpy(var.m4, src->m, sizeof(var.m4));
            }
        }
            break;
        default:
            assert(0);
        }

        bp::Node::Property prop;
        prop.var = var;

        for (auto& d : std::static_pointer_cast<shadergraph::UniformVal>(v.val)->desc)
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

bp::PinDesc Node::BackToFront(const dag::Node<shadergraph::Variant>::Port& back)
{
    bp::PinDesc front;

    front.type = shaderlab::ShaderAdapter::TypeBackToFront(back.var.type.type, 1);
    const_cast<shadergraph::Block::Port&>(back).var.full_name = back.var.type.name;
    front.name = back.var.full_name;

    return front;
}

}