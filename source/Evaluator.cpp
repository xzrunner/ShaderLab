#include "shaderlab/Evaluator.h"
#include "shaderlab/Node.h"
#include "shaderlab/node/Texture2DAsset.h"

#include "shaderlab/RegistNodes.h"

#include <unirender/Device.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Uniform.h>
#include <shadertrans/ShaderTrans.h>
#include <shadergraph/typedef.h>
#include <shadergraph/Evaluator.h>
#include <shadergraph/ValueImpl.h>
#include <shadergraph/block/FragmentShader.h>
#include <shadergraph/block/VertexShader.h>
#include <shadergraph/block/Time.h>
#include <shadergraph/block/Texture2DAsset.h>
#include <shadergraph/block/VertToFrag.h>
#include <shadergraph/block/VertexAttribute.h>
#include <shadergraph/block/ModelMatrix.h>
#include <shadergraph/block/ViewMatrix.h>
#include <shadergraph/block/ProjectionMatrix.h>
#include <painting0/TimeUpdater.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>

namespace shaderlab
{

std::shared_ptr<ur::ShaderProgram>
Evaluator::BuildShader(const ur::Device& dev, const std::string& vs, const std::vector<bp::NodePtr>& nodes,
                       std::vector<std::pair<std::string, ur::TexturePtr>>& textures)
{
    assert(m_front_eval);

    // prepare nodes
    shadergraph::BlockPtr vert_node = nullptr;
    shadergraph::BlockPtr frag_node = nullptr;
    std::vector<shadergraph::BlockPtr> vert_attr_nodes, vert2frag_nodes;
    for (auto& node : nodes)
    {
        auto back_node = m_front_eval->QueryBackNode(*node);
        if (!back_node) {
            continue;
        }

        auto block = std::static_pointer_cast<shadergraph::Block>(back_node);
        auto type = block->get_type();
        if (type == rttr::type::get<shadergraph::block::FragmentShader>()) {
            frag_node = block;
        } else if (type == rttr::type::get<shadergraph::block::VertexShader>()) {
            vert_node = block;
        } else if (type == rttr::type::get<shadergraph::block::VertexAttribute>()) {
            vert_attr_nodes.push_back(block);
        } else if (type == rttr::type::get<shadergraph::block::VertToFrag>()) {
            vert2frag_nodes.push_back(block);
        }
    }

    // build shader code
    std::string vs_code, fs_code;
    if (vert_node) 
    {
        m_back_eval_vs.Rebuild(vert_node);
        for (auto& b : vert_attr_nodes) {
            m_back_eval_vs.AddBlock(b);
        }
        for (auto& b : vert2frag_nodes) {
            m_back_eval_vs.AddBlock(b);
        }
        vs_code = m_back_eval_vs.GenShaderCode(shadergraph::Evaluator::ShaderType::Vert);
    }
    if (frag_node)
    {
        std::vector<std::string> used_symbols;
        m_back_eval_vs.GetTextureSymbols(used_symbols);

        m_back_eval_fs.Rebuild(frag_node, used_symbols);
        fs_code = m_back_eval_fs.GenShaderCode(shadergraph::Evaluator::ShaderType::Frag);
    }

    if (fs_code.empty()) {
        return nullptr;
    }

    if (vs_code.empty()) {
        vs_code = vs;
    }

    std::vector<unsigned int> _vs, _fs;
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::VertexShader, vs_code, _vs);
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::PixelShader, fs_code, _fs);
    m_shader = dev.CreateShaderProgram(_vs, _fs);
    if (!m_shader || !m_shader->CheckStatus()) {
        return nullptr;
    }

    // vs
    for (auto& node : nodes)
    {
        auto back_node = m_front_eval->QueryBackNode(*node);
        if (!back_node || !m_back_eval_vs.HasBlock(std::static_pointer_cast<shadergraph::Block>(back_node))) {
            continue;
        }

        auto block_type = std::static_pointer_cast<shadergraph::Block>(back_node)->get_type();
        if (block_type == rttr::type::get<shadergraph::block::Time>())
        {
            auto up = std::make_shared<pt0::TimeUpdater>(*m_shader,
                shadergraph::block::Time::TIME_STR,
                shadergraph::block::Time::SIN_TIME_STR,
                shadergraph::block::Time::COS_TIME_STR,
                shadergraph::block::Time::DELTA_TIME_STR);
            m_shader->AddUniformUpdater(up);
        }
        else if (block_type == rttr::type::get<shadergraph::block::Texture2DAsset>())
        {
            auto tex = std::static_pointer_cast<node::Texture2DAsset>(node)->GetTexture();
            if (tex) {
                auto name = m_back_eval_vs.QueryRealName(&back_node->GetExports()[0].var.type);
                textures.push_back({ name, tex });
            }
        }
        else if (block_type == rttr::type::get<shadergraph::block::ModelMatrix>())
        {
            m_shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(
                *m_shader, shadergraph::block::ModelMatrix::VAR_NAME));
        }
        else if (block_type == rttr::type::get<shadergraph::block::ViewMatrix>())
        {
            m_shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(
                *m_shader, shadergraph::block::ViewMatrix::VAR_NAME));
        }
        else if (block_type == rttr::type::get<shadergraph::block::ProjectionMatrix>())
        {
            m_shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(
                *m_shader, shadergraph::block::ProjectionMatrix::VAR_NAME));
        }
    }
    // fs
    for (auto& node : nodes)
    {
        auto back_node = m_front_eval->QueryBackNode(*node);
        if (!back_node || !m_back_eval_fs.HasBlock(std::static_pointer_cast<shadergraph::Block>(back_node))) {
            continue;
        }

        auto block_type = std::static_pointer_cast<shadergraph::Block>(back_node)->get_type();
        if (block_type == rttr::type::get<shadergraph::block::Time>())
        {
            auto up = std::make_shared<pt0::TimeUpdater>(*m_shader,
                shadergraph::block::Time::TIME_STR,
                shadergraph::block::Time::SIN_TIME_STR,
                shadergraph::block::Time::COS_TIME_STR,
                shadergraph::block::Time::DELTA_TIME_STR);
            m_shader->AddUniformUpdater(up);
        }
        else if (block_type == rttr::type::get<shadergraph::block::Texture2DAsset>())
        {
            auto tex = std::static_pointer_cast<node::Texture2DAsset>(node)->GetTexture();
            if (tex) {
                auto name = m_back_eval_fs.QueryRealName(&back_node->GetExports()[0].var.type);
                textures.push_back({ name, tex });
            }
        }
    }

    UpdateUniforms(m_back_eval_vs, m_shader);
    UpdateUniforms(m_back_eval_fs, m_shader);

    return m_shader;
}

void Evaluator::UpdateUniforms()
{
    if (m_shader) {
        UpdateUniforms(m_back_eval_vs, m_shader);
        UpdateUniforms(m_back_eval_fs, m_shader);
    }
}

void Evaluator::UpdateUniforms(const shadergraph::Evaluator& back_eval,
                               const std::shared_ptr<ur::ShaderProgram>& shader)
{
    auto uniforms = back_eval.CalcUniformValues();
    for (auto& u : uniforms)
    {
        auto uniform = shader->QueryUniform(u.name);
        if (!uniform) {
            continue;
        }

        auto unif = std::static_pointer_cast<shadergraph::UniformVal>(u.val);
        if (!unif->var.val) {
            continue;
        }
        switch (unif->var.type)
        {
        case shadergraph::VarType::Invalid:
            break;

        case shadergraph::VarType::Bool:
        {
            auto b = std::static_pointer_cast<shadergraph::BoolVal>(unif->var.val)->x;
            const int i = b ? 1 : 0;
            uniform->SetValue(&i, 1);
        }
            break;

        case shadergraph::VarType::Int:
        {
            const int i = std::static_pointer_cast<shadergraph::IntVal>(unif->var.val)->x;
            uniform->SetValue(&i, 1);
        }
            break;

        case shadergraph::VarType::Float:
        {
            const float f = std::static_pointer_cast<shadergraph::FloatVal>(unif->var.val)->x;
            uniform->SetValue(&f, 1);
        }
            break;

        case shadergraph::VarType::Float2:
        {
            const auto f2 = std::static_pointer_cast<shadergraph::Float2Val>(unif->var.val)->xy;
            switch (uniform->GetType())
            {
            case ur::UniformType::Float1:
                uniform->SetValue(f2, 1);
                break;
            case ur::UniformType::Float2:
                uniform->SetValue(f2, 2);
                break;
            default:
                assert(0);
            }
        }
            break;

        case shadergraph::VarType::Float3:
        {
            const auto f3 = std::static_pointer_cast<shadergraph::Float3Val>(unif->var.val)->xyz;
            switch (uniform->GetType())
            {
            case ur::UniformType::Float1:
                uniform->SetValue(f3, 1);
                break;
            case ur::UniformType::Float2:
                uniform->SetValue(f3, 2);
                break;
            case ur::UniformType::Float3:
                uniform->SetValue(f3, 3);
                break;
            default:
                assert(0);
            }
        }
            break;

        case shadergraph::VarType::Float4:
        {
            const auto f4 = std::static_pointer_cast<shadergraph::Float4Val>(unif->var.val)->xyzw;
            switch (uniform->GetType())
            {
            case ur::UniformType::Float1:
                uniform->SetValue(f4, 1);
                break;
            case ur::UniformType::Float2:
                uniform->SetValue(f4, 2);
                break;
            case ur::UniformType::Float3:
                uniform->SetValue(f4, 3);
                break;
            case ur::UniformType::Float4:
                uniform->SetValue(f4, 4);
                break;
            default:
                assert(0);
            }
        }
            break;

        case shadergraph::VarType::Matrix4:
            assert(uniform->GetType() == ur::UniformType::Matrix44);
            break;

        default:
            assert(0);
        }
    }
}

}