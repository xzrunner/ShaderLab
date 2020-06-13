#pragma once

#include <blueprint/Node.h>
#include <blueprint/BackendAdapter.h>

#include <shadergraph/Variant.h>

namespace shadergraph { class Block; }
namespace ur { class ShaderProgram; }

namespace shaderlab
{

class Node : public bp::Node
{
public:
    Node(const std::string& title);

    bool GetPreview() const { return m_preview; }
    void SetPreview(bool preview) { m_preview = preview; }

    std::shared_ptr<ur::ShaderProgram> GetPreviewShader() const { return m_preview_shader; }
    void SetPreviewShader(const std::shared_ptr<ur::ShaderProgram>& shader) {
        m_preview_shader = shader;
    }

public:
    static constexpr char* const STR_PROP_DISPLAY = "Display";

protected:
    void Init(const std::string& name);
    void InitProps(const std::vector<shadergraph::Variant>& vars);

    static bp::PinDesc BackToFront(const dag::Node<shadergraph::Variant>::Port& back);

private:
    bool m_preview = false;
    std::shared_ptr<ur::ShaderProgram> m_preview_shader = nullptr;

    RTTR_ENABLE(bp::Node)

}; // Node

}