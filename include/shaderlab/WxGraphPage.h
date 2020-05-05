#pragma once

#include "shaderlab/Evaluator.h"

#include <blueprint/WxGraphPage.h>

#include <shadergraph/Variant.h>

namespace shaderlab
{

class WxGraphPage : public bp::WxGraphPage<shadergraph::Variant>
{
public:
    WxGraphPage(const ur::Device& dev, wxWindow* parent, const ee0::GameObj& root,
        const ee0::SubjectMgrPtr& preview_sub_mgr);

    void SetFilepath(const std::string& filepath) {
        m_filepath = filepath;
    }

    void SetCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

protected:
    virtual void OnEvalChangeed() override;

private:
    //Evaluator m_front_eval;

    std::string m_filepath;

}; // WxGraphPage

}