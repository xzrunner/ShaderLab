#pragma once

#include <blueprint/Node.h>

namespace shaderlab
{

class Node : public bp::Node
{
public:
    Node(const std::string& title);

public:
    static constexpr char* const STR_PROP_DISPLAY = "Display";

protected:
    void Init(const std::string& name);

private:
    void InitPins(const std::string& name);
    void InitProps(const std::string& name);

    RTTR_ENABLE(bp::Node)

}; // Node

}