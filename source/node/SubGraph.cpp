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
    : bp::node::SubGraph<shadergraph::Variant>("SubGraph", "shadergraph", "shaderlab", nullptr)
{
}

void SubGraph::LoadFromJson(const std::string& dir, const rapidjson::Value& val)
{
    Node::LoadFromJson(dir, val);

    SetupPorts();
}

void SubGraph::Setup(const std::vector<shadergraph::Variant>& inputs,
                     const std::vector<shadergraph::Variant>& outputs)
{
    // init vars
    m_in_vars = inputs;
    m_out_vars = outputs;

    // setup ports
    SetupPorts();

    // setup graph
    m_children.clear();
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
        i->m_var_type = var.type;
        i->m_var_name = var.name;
        i->SetName(var.name);
        bp_node = i;
    }
    else
    {
        auto o = std::make_shared<node::Output>();
        o->m_var_type = var.type;
        bp_node = o;
    }
    m_graph->OnAddNode(*bp_node);

    m_children.push_back(bp_node);
}

void SubGraph::SetupPorts()
{
    m_all_input.clear();
    m_all_input.reserve(m_in_vars.size());
    for (size_t i = 0, n = m_in_vars.size(); i < n; ++i) {
        auto type = ShaderAdapter::TypeBackToFront(m_in_vars[i].type, 1);
        m_all_input.push_back(std::make_shared<bp::Pin>(true, i, type, m_in_vars[i].name, *this));
    }

    m_all_output.clear();
    m_all_output.reserve(m_out_vars.size());
    for (size_t i = 0, n = m_out_vars.size(); i < n; ++i) {
        auto type = ShaderAdapter::TypeBackToFront(m_out_vars[i].type, 1);
        m_all_output.push_back(std::make_shared<bp::Pin>(false, i, type, m_out_vars[i].name, *this));
    }

    bp::NodeLayout::UpdateNodeStyle(*this);
}

}
}