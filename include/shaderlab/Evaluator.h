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
        BuildShader(const ur::Device& dev, const std::string& vs,
            const std::vector<bp::NodePtr>& nodes);
    void UpdateUniforms();
    static void UpdateUniforms(const shadergraph::Evaluator& back_eval,
        const std::shared_ptr<ur::ShaderProgram>& shader);

    std::vector<std::pair<std::string, ur::TexturePtr>>
        QueryTextures(const std::vector<bp::NodePtr>& nodes) const;

private:
    std::shared_ptr<bp::BackendGraph<shadergraph::Variant>> m_front_eval = nullptr;
    shadergraph::Evaluator m_back_eval;

    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

}; // Evaluator

}