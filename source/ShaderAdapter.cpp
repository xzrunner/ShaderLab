#include "shaderlab/ShaderAdapter.h"
#include "shaderlab/PinType.h"

#include <blueprint/Pin.h>

#include <assert.h>

namespace shaderlab
{

int ShaderAdapter::TypeBackToFront(shadergraph::VarType type, int count)
{
    int ret = -1;

    switch (type)
    {
    case shadergraph::VarType::Invalid:
        ret = bp::PIN_ANY_VAR;
        break;
    case shadergraph::VarType::Dynamic:
        ret = PIN_DYNAMIC;
        break;
    case shadergraph::VarType::Bool:
        ret = PIN_BOOL;
        break;
    case shadergraph::VarType::Bool2:
        ret = PIN_BOOL2;
        break;
    case shadergraph::VarType::Bool3:
        ret = PIN_BOOL3;
        break;
    case shadergraph::VarType::Bool4:
        ret = PIN_BOOL4;
        break;
    case shadergraph::VarType::UInt:
        ret = PIN_UINT;
        break;
    case shadergraph::VarType::Int:
        ret = PIN_INT;
        break;
    case shadergraph::VarType::Int2:
        ret = PIN_INT2;
        break;
    case shadergraph::VarType::Int3:
        ret = PIN_INT3;
        break;
    case shadergraph::VarType::Int4:
        ret = PIN_INT4;
        break;
    case shadergraph::VarType::Float:
        ret = PIN_FLOAT;
        break;
    case shadergraph::VarType::Float2:
        ret = PIN_FLOAT2;
        break;
    case shadergraph::VarType::Float3:
        ret = PIN_FLOAT3;
        break;
    case shadergraph::VarType::Float4:
        ret = PIN_FLOAT4;
        break;
    case shadergraph::VarType::Matrix2:
        ret = PIN_MAT2;
        break;
    case shadergraph::VarType::Matrix3:
        ret = PIN_MAT3;
        break;
    case shadergraph::VarType::Matrix4:
        ret = PIN_MAT4;
        break;
    case shadergraph::VarType::Sampler2D:
        ret = PIN_SAMPLER_2D;
        break;
    case shadergraph::VarType::SamplerCube:
        ret = PIN_SAMPLER_CUBE;
        break;
    case shadergraph::VarType::Array:
        ret = PIN_ARRAY;
        break;
    case shadergraph::VarType::Struct:
        ret = PIN_STRUCT;
        break;
    case shadergraph::VarType::Function:
        ret = PIN_FUNCTION;
        break;
    default:
        assert(0);
    }

    return ret;
}

void ShaderAdapter::Front2Back(const bp::Node& front, dag::Node<shadergraph::Variant>& back, const std::string& dir)
{
}

}