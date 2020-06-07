#pragma once

#include <blueprint/node/SubGraph.h>

#include <blueprint/BackendGraph.h>

#include <node0/typedef.h>
#include <shadergraph/Variant.h>

namespace shaderlab
{
namespace node
{

class SubGraph : public bp::node::SubGraph
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

private:
    std::vector<shadergraph::Variant> m_in_vars, m_out_vars;

    std::shared_ptr<bp::BackendGraph<shadergraph::Variant>> m_front_eval = nullptr;

    RTTR_ENABLE(bp::node::SubGraph)

}; // SubGraph

}
}