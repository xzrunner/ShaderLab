#include "shaderlab/Evaluator.h"
#include "shaderlab/Node.h"

#include <unirender/Device.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Uniform.h>
#include <shadergraph/typedef.h>
#include <shadergraph/Evaluator.h>
#include <shadergraph/ValueImpl.h>
#include <shadergraph/block/FragmentShader.h>
#include <shadergraph/block/Time.h>
#include <painting0/TimeUpdater.h>

namespace shaderlab
{

void Evaluator::BuildShader(const ur::Device& dev, const std::string& vs,
                            const std::vector<bp::NodePtr>& nodes)
{
    assert(m_front_eval);

    std::string code;

    // build frag m_shader
    for (auto& node : nodes)
    {
        auto back_node = m_front_eval->QueryBackNode(*node);
        if (!back_node) {
            continue;
        }
        assert(back_node);
        auto block = std::static_pointer_cast<shadergraph::Block>(back_node);
        if (block->get_type() == rttr::type::get<shadergraph::block::FragmentShader>())
        {
            m_back_eval.Rebuild(block);
            code = m_back_eval.GenShaderCode();
            break;
        }
    }

    if (code.empty()) {
        return;
    }

    m_shader = dev.CreateShaderProgram(vs, code);
    if (!m_shader->CheckStatus()) {
        return;
    }

    for (auto& node : nodes)
    {
        auto back_node = m_front_eval->QueryBackNode(*node);
        if (!back_node) {
            continue;
        }
        assert(back_node);
        auto block = std::static_pointer_cast<shadergraph::Block>(back_node);
        if (block->get_type() == rttr::type::get<shadergraph::block::Time>())
        {
            auto up = std::make_shared<pt0::TimeUpdater>(*m_shader,
                shadergraph::block::Time::TIME_STR,
                shadergraph::block::Time::SIN_TIME_STR,
                shadergraph::block::Time::COS_TIME_STR,
                shadergraph::block::Time::DELTA_TIME_STR);
            m_shader->AddUniformUpdater(up);
        }
    }

    UpdateUniforms();
}

void Evaluator::UpdateUniforms()
{
    auto uniforms = m_back_eval.CalcUniformValues();
    for (auto& u : uniforms)
    {
        auto uniform = m_shader->QueryUniform(u.name);
        if (!uniform) {
            continue;
        }

        auto unif = std::static_pointer_cast<shadergraph::UniformVal>(u.val);
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
        default:
            assert(0);
        }
    }
}

}