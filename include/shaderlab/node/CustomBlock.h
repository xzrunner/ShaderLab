#pragma once

#include "shaderlab/Node.h"

namespace shaderlab
{
namespace node
{

class CustomBlock : public Node
{
public:
    CustomBlock()
        : Node("CustomBlock")
    {
        Init("custom_block");
    }

    auto& GetCode() const { return m_code; }
    void  SetCode(const std::string& code) { m_code = code; }

private:
    std::string m_code;

    RTTR_ENABLE(Node)

}; // CustomBlock

}
}