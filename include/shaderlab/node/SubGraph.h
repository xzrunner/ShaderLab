#pragma once

#include <blueprint/node/SubGraph.h>

#include <shadergraph/Variant.h>

namespace shaderlab
{
namespace node
{

class SubGraph : public bp::node::SubGraph<shadergraph::Variant>
{
public:
    SubGraph();

    virtual void LoadFromJson(const std::string& dir, const rapidjson::Value& val) override;

    void Setup(const std::vector<shadergraph::Variant>& inputs,
        const std::vector<shadergraph::Variant>& outputs);

    auto& GetInputVars() const { return m_in_vars; }
    void SetInputVars(const std::vector<shadergraph::Variant>& vars) { m_in_vars = vars; }
    auto& GetOutputVars() const { return m_out_vars; }
    void SetOutputVars(const std::vector<shadergraph::Variant>& vars) { m_out_vars = vars; }

private:
    void Insert(const shadergraph::Variant& var, bool is_input);

    void SetupPorts();

private:
    std::vector<shadergraph::Variant> m_in_vars, m_out_vars;

    RTTR_ENABLE(bp::node::SubGraph<shadergraph::Variant>)

}; // SubGraph

}
}