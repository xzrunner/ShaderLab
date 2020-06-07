#include "shaderlab/node/SubGraph.h"
#include "shaderlab/PinType.h"
#include "shaderlab/RegistNodes.h"
#include "shaderlab/ShaderAdapter.h"

#include <blueprint/NodeLayout.h>

namespace shaderlab
{
namespace node
{

SubGraph::SubGraph()
    : bp::node::SubGraph("SubGraph")
{
    m_front_eval = std::make_shared<bp::BackendGraph<shadergraph::Variant>>("shadergraph", "shaderlab");
}

void SubGraph::LoadFromJson(const std::string& dir, const rapidjson::Value& val)
{
    Node::LoadFromJson(dir, val);

    Setup(m_in_vars, m_out_vars);
}

void SubGraph::Setup(const std::vector<shadergraph::Variant>& inputs,
                     const std::vector<shadergraph::Variant>& outputs)
{
    // prepare
    m_in_vars = inputs;
    m_out_vars = outputs;

    // update ports
    m_all_input.clear();
    m_all_input.reserve(inputs.size());
    for (size_t i = 0, n = inputs.size(); i < n; ++i) {
        auto type = ShaderAdapter::TypeBackToFront(inputs[i].type, 1);
        m_all_input.push_back(std::make_shared<bp::Pin>(true, i, type, inputs[i].name, *this));
    }

    m_all_output.clear();
    m_all_output.reserve(outputs.size());
    for (size_t i = 0, n = outputs.size(); i < n; ++i) {
        auto type = ShaderAdapter::TypeBackToFront(outputs[i].type, 1);
        m_all_output.push_back(std::make_shared<bp::Pin>(false, i, type, outputs[i].name, *this));
    }
    bp::NodeLayout::UpdateNodeStyle(*this);

    // children scene nodes
    for (int i = 0, n = inputs.size(); i < n; ++i) {
        Insert(inputs[i], true);
    }
    for (int i = 0, n = outputs.size(); i < n; ++i) {
        Insert(outputs[i], false);
    }
}

void SubGraph::Insert(const shadergraph::Variant& var, bool is_input)
{
    bp::NodePtr bp_node = nullptr;
    if (is_input)
    {
        auto i = std::make_shared<node::Input>();
        i->m_type = var.type;
        i->m_name = var.name;
        bp_node = i;
    }
    else
    {
        auto o = std::make_shared<node::Output>();
        o->m_type = var.type;
        bp_node = o;
    }

    bp_node->SetName(var.name);

    m_front_eval->OnAddNode(*bp_node);

    AddChild(bp_node);
}

}
}