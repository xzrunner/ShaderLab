#pragma once

#include <blueprint/Node.h>

namespace shadergraph { class Block; struct Variant; }

namespace shaderlab
{

class Node : public bp::Node
{
public:
    Node(const std::string& title);

    void Init(const shadergraph::Block& block);

public:
    static constexpr char* const STR_PROP_DISPLAY = "Display";

protected:
    void Init(const std::string& name);

private:
    void InitProps(const std::vector<shadergraph::Variant>& uniforms);

    RTTR_ENABLE(bp::Node)

}; // Node

}