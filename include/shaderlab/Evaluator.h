#pragma once

#include <blueprint/typedef.h>
#include <blueprint/BackendGraph.h>

#include <shadergraph/Variant.h>
#include <shadergraph/Evaluator.h>

namespace shaderlab
{

class Evaluator
{
public:
    static std::string Rebuild(const std::vector<bp::NodePtr>& nodes,
        const bp::BackendGraph<shadergraph::Variant>& eval,
        std::vector<shadergraph::Evaluator::Uniform>& uniforms);

}; // Evaluator

}