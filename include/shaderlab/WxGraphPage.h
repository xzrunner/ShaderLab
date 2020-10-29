#pragma once

#include <blueprint/WxGraphPage.h>

#include <shadergraph/Variant.h>

namespace shaderlab
{

class WxGraphPage : public bp::WxGraphPage<shadergraph::Variant>
{
public:
    WxGraphPage(const ur::Device& dev, wxWindow* parent, const ee0::GameObj& root,
        const ee0::SubjectMgrPtr& preview_sub_mgr);

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    void SetFilepath(const std::string& filepath) {
        m_filepath = filepath;
    }

    void SetCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

private:
    bool IsNodeInputOrOutput(const ee0::VariantSet& variants) const;
    void SetupSubGraphPorts();

private:
    std::string m_filepath;

}; // WxGraphPage

}