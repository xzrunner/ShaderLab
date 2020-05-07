#include "shaderlab/Evaluator.h"
#include "shaderlab/Node.h"

#include <shadergraph/typedef.h>
#include <shadergraph/Evaluator.h>
#include <shadergraph/block/FragmentShader.h>

namespace shaderlab
{

std::string Evaluator::Rebuild(const std::vector<bp::NodePtr>& nodes,
                               const bp::BackendGraph<shadergraph::Variant>& eval,
                               std::vector<shadergraph::Evaluator::Uniform>& uniforms)
{
    // build frag shader
    for (auto& node : nodes)
    {
        auto back_node = eval.QueryBackNode(*node);
        if (!back_node) {
            continue;
        }
        assert(back_node);
        auto block = std::static_pointer_cast<shadergraph::Block>(back_node);
        if (block->get_type() == rttr::type::get<shadergraph::block::FragmentShader>())
        {
            shadergraph::Evaluator eval(block);
            uniforms = eval.GetUniformValues();
            return eval.GenShaderCode();
        }
    }

    return "";
}

}