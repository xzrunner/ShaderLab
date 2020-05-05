#include "shaderlab/Node.h"
#include "shaderlab/PinType.h"

#include <blueprint/Pin.h>
#include <blueprint/BackendAdapter.h>

#include <shadergraph/VarType.h>
#include <shadergraph/Variant.h>

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

        if (back.var.type.type == shadergraph::VarType::Invalid) {
            front.type = bp::PIN_ANY_VAR;
        } else if (back.var.type.type == shadergraph::VarType::Bool) {
            front.type = PIN_BOOL;
        } else if (back.var.type.type == shadergraph::VarType::Bool2) {
            front.type = PIN_BOOL2;
        } else if (back.var.type.type == shadergraph::VarType::Bool3) {
            front.type = PIN_BOOL3;
        } else if (back.var.type.type == shadergraph::VarType::Bool4) {
            front.type = PIN_BOOL4;
        } else if (back.var.type.type == shadergraph::VarType::UInt) {
            front.type = PIN_UINT;
        } else if (back.var.type.type == shadergraph::VarType::Int) {
            front.type = PIN_INT;
        } else if (back.var.type.type == shadergraph::VarType::Int2) {
            front.type = PIN_INT2;
        } else if (back.var.type.type == shadergraph::VarType::Int3) {
            front.type = PIN_INT3;
        } else if (back.var.type.type == shadergraph::VarType::Int4) {
            front.type = PIN_INT4;
        } else if (back.var.type.type == shadergraph::VarType::Float) {
            front.type = PIN_FLOAT;
        } else if (back.var.type.type == shadergraph::VarType::Float2) {
            front.type = PIN_FLOAT2;
        } else if (back.var.type.type == shadergraph::VarType::Float3) {
            front.type = PIN_FLOAT3;
        } else if (back.var.type.type == shadergraph::VarType::Float4) {
            front.type = PIN_FLOAT4;
        } else if (back.var.type.type == shadergraph::VarType::Matrix2) {
            front.type = PIN_MAT2;
        } else if (back.var.type.type == shadergraph::VarType::Matrix3) {
            front.type = PIN_MAT3;
        } else if (back.var.type.type == shadergraph::VarType::Matrix4) {
            front.type = PIN_MAT4;
        } else if (back.var.type.type == shadergraph::VarType::Sampler2D) {
            front.type = PIN_SAMPLER_2D;
        } else if (back.var.type.type == shadergraph::VarType::SamplerCube) {
            front.type = PIN_SAMPLER_CUBE;
        } else if (back.var.type.type == shadergraph::VarType::Array) {
            front.type = PIN_ARRAY;
        } else if (back.var.type.type == shadergraph::VarType::Struct) {
            front.type = PIN_STRUCT;
        } else if (back.var.type.type == shadergraph::VarType::Function) {
            front.type = PIN_FUNCTION;
        } else {
            assert(0);
        }

        front.name = back.var.full_name;

        return front;
    };

    bp::BackendAdapter<shadergraph::Variant> trans("shadergraph", back2front);
    trans.InitNodePins(*this, name);
}

}