#pragma once

#include <blueprint/typedef.h>
#include <blueprint/BackendGraph.h>

#include <unirender/typedef.h>
#include <shadergraph/Variant.h>
#include <shadergraph/Evaluator.h>

namespace ur { class Device; class ShaderProgram; }

namespace shaderlab
{

class Evaluator
{
public:
    Evaluator() {}

    void SetFrontEval(const std::shared_ptr<bp::BackendGraph<shadergraph::Variant>>& front_eval) {
        m_front_eval = front_eval;
    }

    std::shared_ptr<ur::ShaderProgram>
        BuildShader(const ur::Device& dev, const std::string& vs, const std::vector<bp::NodePtr>& nodes,
            std::vector<std::pair<std::string, ur::TexturePtr>>& textures, std::vector<shadergraph::Variant>& input_vars);
    void UpdateUniforms();
    static void UpdateUniforms(const shadergraph::Evaluator& back_eval,
        const std::shared_ptr<ur::ShaderProgram>& shader);

    static void BuildShaderCode(const bp::BackendGraph<shadergraph::Variant>& front_eval, const std::vector<bp::NodePtr>& nodes, 
        shadergraph::Evaluator& back_eval_vs, shadergraph::Evaluator& back_eval_fs, std::vector<std::pair<std::string, ur::TexturePtr>>& textures,
        std::vector<shadergraph::Variant>& input_vars);

private:
    static void ResolveTextures(const shadergraph::Evaluator& back_eval, const bp::BackendGraph<shadergraph::Variant>& front_eval,
        const std::vector<bp::NodePtr>& nodes, std::map<ur::TexturePtr, std::string>& tex2name);

private:
    std::shared_ptr<bp::BackendGraph<shadergraph::Variant>> m_front_eval = nullptr;
    shadergraph::Evaluator m_back_eval_vs, m_back_eval_fs;

    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

}; // Evaluator

}