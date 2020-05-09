#pragma once

#include <blueprint/typedef.h>
#include <blueprint/BackendGraph.h>

#include <shadergraph/Variant.h>
#include <shadergraph/Evaluator.h>

namespace ur { class Device; class ShaderProgram; }

namespace shaderlab
{

class Evaluator
{
public:
    static std::shared_ptr<ur::ShaderProgram>
        BuildShader(const ur::Device& dev, const std::string& vs,
            const std::vector<bp::NodePtr>& nodes, const bp::BackendGraph<shadergraph::Variant>& eval);

}; // Evaluator

}