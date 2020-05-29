#include "shaderlab/ShaderAdapter.h"
#include "shaderlab/PinType.h"
#include "shaderlab/node/CustomBlock.h"
#include "shaderlab/node/Texture2DAsset.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>

#include <shadergraph/Block.h>
#include <shadergraph/ValueImpl.h>
#include <shadergraph/block/CustomBlock.h>
#include <shadergraph/block/Texture2DAsset.h>

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

void ShaderAdapter::Front2Back(const bp::Node& front, dag::Node<shadergraph::Variant>& back,
                               const std::string& dir, const ur::Device& dev)
{
    // update uniforms
    auto& src = front.GetProps();
    auto& dst = static_cast<shadergraph::Block&>(back).GetVariants();
    for (auto& s_p : src) {
        auto& s = s_p.var;
        for (auto& d : dst) {
            if (s.name != d.name) {
                continue;
            }

            assert(d.type == shadergraph::VarType::Uniform);
            auto u_var = std::static_pointer_cast<shadergraph::UniformVal>(d.val)->var;
            switch (u_var.type)
            {
            case shadergraph::VarType::Bool:
                std::static_pointer_cast<shadergraph::BoolVal>(u_var.val)->x = s.b;
                break;
            case shadergraph::VarType::Int:
                std::static_pointer_cast<shadergraph::IntVal>(u_var.val)->x = s.i;
                break;
            case shadergraph::VarType::Float:
                std::static_pointer_cast<shadergraph::FloatVal>(u_var.val)->x = s.f;
                break;
            case shadergraph::VarType::Float2:
                memcpy(std::static_pointer_cast<shadergraph::Float2Val>(u_var.val)->xy, s.f2, sizeof(float) * 2);
                break;
            case shadergraph::VarType::Float3:
                memcpy(std::static_pointer_cast<shadergraph::Float3Val>(u_var.val)->xyz, s.f3, sizeof(float) * 3);
                break;
            case shadergraph::VarType::Float4:
                memcpy(std::static_pointer_cast<shadergraph::Float4Val>(u_var.val)->xyzw, s.f4, sizeof(float) * 4);
                break;
            case shadergraph::VarType::Array:
                // todo
                break;
            default:
                assert(0);
            }

            break;
        }
    }

    auto type = front.get_type();
    if (type == rttr::type::get<node::CustomBlock>())
    {
        auto& src = static_cast<const node::CustomBlock&>(front);
        auto& dst = static_cast<shadergraph::block::CustomBlock&>(back);
        dst.SetCode(src.GetCode());
        const_cast<node::CustomBlock&>(src).Init(dst);
    }
    else if (type == rttr::type::get<node::Texture2DAsset>())
    {
        auto& src = static_cast<const node::Texture2DAsset&>(front);
        auto& dst = static_cast<shadergraph::block::Texture2DAsset&>(back);
        const_cast<node::Texture2DAsset&>(src).UpdateTexture(dev);
    }
}

}