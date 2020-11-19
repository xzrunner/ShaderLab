#include "shaderlab/ShaderAdapter.h"
#include "shaderlab/PinType.h"
#include "shaderlab/node/CustomBlock.h"
#include "shaderlab/node/Texture2DAsset.h"
#include "shaderlab/node/SubGraph.h"
#include "shaderlab/RegistNodes.h"
#include "shaderlab/Evaluator.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>

#include <dag/Graph.h>
#include <shadergraph/Block.h>
#include <shadergraph/ValueImpl.h>
#include <shadergraph/Evaluator.h>
#include <shadergraph/block/CustomBlock.h>
#include <shadergraph/block/Texture2DAsset.h>
#include <shadergraph/block/SubGraph.h>
#include <shadergraph/block/FragmentShader.h>
#include <shadergraph/block/VertexShader.h>
#include <shadergraph/block/Time.h>
#include <shadergraph/block/ModelMatrix.h>
#include <shadergraph/block/ViewMatrix.h>
#include <shadergraph/block/ProjectionMatrix.h>

#include <js/RapidJsonHelper.h>
#include <ns/CompFactory.h>
#include <node0/CompAsset.h>
#include <node0/CompComplex.h>
#include <node0/SceneNode.h>
#include <unirender/ShaderProgram.h>
#include <painting0/TimeUpdater.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>

#include <boost/filesystem.hpp>

#include <assert.h>

namespace shaderlab
{

int ShaderAdapter::TypeBackToFront(shadergraph::VarType type, int count)
{
    int ret = -1;

    switch (type)
    {
    case shadergraph::VarType::Invalid:
        ret = PIN_INVALID;
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
    for (auto& s_p : src)
    {
        auto& s = s_p.var;
        for (auto& d : dst)
        {
            if (s.name != d.name) {
                continue;
            }

            assert(d.type == shadergraph::VarType::Uniform);
            auto& u_var = std::static_pointer_cast<shadergraph::UniformVal>(d.val)->var;
            if (!u_var.val)
            {
            switch (u_var.type)
            {
            case shadergraph::VarType::Bool:
                u_var.val = std::make_shared<shadergraph::BoolVal>();
                break;
            case shadergraph::VarType::Int:
                u_var.val = std::make_shared<shadergraph::IntVal>();
                break;
            case shadergraph::VarType::Float:
                u_var.val = std::make_shared<shadergraph::FloatVal>();
                break;
            case shadergraph::VarType::Float2:
                u_var.val = std::make_shared<shadergraph::Float2Val>();
                break;
            case shadergraph::VarType::Float3:
                u_var.val = std::make_shared<shadergraph::Float3Val>();
                break;
            case shadergraph::VarType::Float4:
                u_var.val = std::make_shared<shadergraph::Float4Val>();
                break;
            case shadergraph::VarType::Matrix2:
                u_var.val = std::make_shared<shadergraph::Matrix2Val>();
                break;
            case shadergraph::VarType::Matrix3:
                u_var.val = std::make_shared<shadergraph::Matrix3Val>();
                break;
            case shadergraph::VarType::Matrix4:
                u_var.val = std::make_shared<shadergraph::Matrix4Val>();
                break;
            case shadergraph::VarType::Array:
                // todo
                break;
            default:
                assert(0);
            }
            }

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
            case shadergraph::VarType::Matrix2:
                memcpy(std::static_pointer_cast<shadergraph::Matrix2Val>(u_var.val)->m, s.m2, sizeof(s.m2));
                break;
            case shadergraph::VarType::Matrix3:
                memcpy(std::static_pointer_cast<shadergraph::Matrix3Val>(u_var.val)->m, s.m3, sizeof(s.m3));
                break;
            case shadergraph::VarType::Matrix4:
                memcpy(std::static_pointer_cast<shadergraph::Matrix4Val>(u_var.val)->m, s.m4, sizeof(s.m4));
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
    }
    else if (type == rttr::type::get<node::Texture2DAsset>())
    {
        auto& src = static_cast<const node::Texture2DAsset&>(front);
        auto& dst = static_cast<shadergraph::block::Texture2DAsset&>(back);
        const_cast<node::Texture2DAsset&>(src).UpdateTexture(dev);
    }
    else if (type == rttr::type::get<node::SubGraph>())
    {
        auto& src = static_cast<const node::SubGraph&>(front);
        auto& dst = static_cast<shadergraph::block::SubGraph&>(back);
        dst.Setup(src.GetBackGraph(), src.GetInputVars(), src.GetOutputVars());
    }
}

void ShaderAdapter::BuildShaderCode(const std::string& filepath, const ur::Device& dev, std::string& vs, std::string& fs,
                                    std::vector<std::pair<std::string, ur::TexturePtr>>& textures, 
                                    std::vector<std::pair<shadergraph::VarType, std::string>>& input_vars,
                                    uint32_t& updaters)
{
    rapidjson::Document doc;
    js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

    auto dir = boost::filesystem::path(filepath).parent_path().string();
    n0::CompAssetPtr casset = ns::CompFactory::Instance()->CreateAsset(dev, doc, dir);
    if (!casset) {
        return;
    }

    assert(casset->TypeID() == n0::GetAssetUniqueTypeID<n0::CompComplex>());

    bp::BackendGraph<shadergraph::Variant> front_eval("shadergraph", "shaderlab", [&](const bp::Node& front, dag::Node<shadergraph::Variant>& back) {
        ShaderAdapter::Front2Back(front, back, dir, dev);
    });

    std::vector<bp::NodePtr> nodes;
    casset->Traverse([&](const n0::SceneNodePtr& node)->bool
    {
        if (!node->HasUniqueComp<bp::CompNode>()) {
            return true;
        }

        auto& cnode = node->GetUniqueComp<bp::CompNode>();
        auto bp_node = cnode.GetNode();
        front_eval.OnAddNode(*bp_node);

        nodes.push_back(bp_node);

        return true;
    });
    front_eval.OnRebuildConnection();

    for (auto& node : nodes) 
    {
        auto node_type = node->get_type();
        if (node_type == rttr::type::get<node::Time>()) {
            updaters |= TIME_UPDATER_ID;
        } else if (node_type == rttr::type::get<node::ModelMatrix>()) {
            updaters |= MODEL_MAT_UPDATER_ID;
        } else if (node_type == rttr::type::get<node::ViewMatrix>()) {
            updaters |= VIEW_MAT_UPDATER_ID;
        } else if (node_type == rttr::type::get<node::ProjectionMatrix>()) {
            updaters |= PROJ_MAT_UPDATER_ID;
        }
    }

    shadergraph::Evaluator back_eval_vs, back_eval_fs;
    Evaluator::BuildShaderCode(front_eval, nodes, back_eval_vs, back_eval_fs, textures, input_vars);
    vs = back_eval_vs.GenShaderCode(shadergraph::Evaluator::ShaderType::Vert);
    fs = back_eval_fs.GenShaderCode(shadergraph::Evaluator::ShaderType::Frag);
}

void ShaderAdapter::InitShaderUpdaters(ur::ShaderProgram& prog, uint32_t updaters)
{
    if (updaters & TIME_UPDATER_ID)
    {
        auto up = std::make_shared<pt0::TimeUpdater>(prog,
            shadergraph::block::Time::TIME_STR,
            shadergraph::block::Time::SIN_TIME_STR,
            shadergraph::block::Time::COS_TIME_STR,
            shadergraph::block::Time::DELTA_TIME_STR);
        prog.AddUniformUpdater(up);
    }
    if (updaters & MODEL_MAT_UPDATER_ID)
    {
        prog.AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(
            prog, shadergraph::block::ModelMatrix::VAR_NAME));
    }
    if (updaters & VIEW_MAT_UPDATER_ID)
    {
        prog.AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(
            prog, shadergraph::block::ViewMatrix::VAR_NAME));
    }
    if (updaters & PROJ_MAT_UPDATER_ID)
    {
        prog.AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(
            prog, shadergraph::block::ProjectionMatrix::VAR_NAME));
    }
}

}