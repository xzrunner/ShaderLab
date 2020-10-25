#include "shaderlab/WxDefaultProperty.h"
#include "shaderlab/WxPreviewCanvas.h"

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>

namespace shaderlab
{

WxDefaultProperty::WxDefaultProperty(wxWindow* parent, const std::shared_ptr<WxPreviewCanvas>& preview_canvas)
    : wxPanel(parent)
    , m_preview_canvas(preview_canvas)
{
    InitLayout();
    InitProps();
}

void WxDefaultProperty::InitLayout()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

	m_pg = new wxPropertyGrid(this, -1, wxDefaultPosition, wxSize(500, -1),
		wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED
	);
    Connect(m_pg->GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler(
        WxDefaultProperty::OnPropertyGridChanged));
	sizer->Add(m_pg, wxEXPAND);

	SetSizer(sizer);
}

void WxDefaultProperty::InitProps()
{
    wxArrayString viewer_choices;
    viewer_choices.push_back("Image");
    viewer_choices.push_back("Height");
    viewer_choices.push_back("Model");

    auto viewer_prop = new wxEnumProperty("Viewer", wxPG_LABEL, viewer_choices);
    viewer_prop->SetValue(WxPreviewCanvas::VIEWER_IMAGE);
    m_pg->Append(viewer_prop);
}

void WxDefaultProperty::OnPropertyGridChanged(wxPropertyGridEvent& event)
{
    wxPGProperty* property = event.GetProperty();
    auto key = property->GetName();
    wxAny val = property->GetValue();

    if (key == "Viewer")
    {
        auto idx = wxANY_AS(val, int);
        m_preview_canvas->SetViewer(
            static_cast<WxPreviewCanvas::VIEWER_TYPE>(idx)
        );
    }
}

}