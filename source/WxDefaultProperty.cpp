#include "shaderlab/WxDefaultProperty.h"
#include "shaderlab/PreviewViewer.h"

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>

namespace shaderlab
{

WxDefaultProperty::WxDefaultProperty(wxWindow* parent, const std::shared_ptr<PreviewViewer>& preview_viewer)
    : wxPanel(parent)
    , m_preview_viewer(preview_viewer)
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
    viewer_choices.push_back("Rect2D");
    viewer_choices.push_back("Rect3D");
    viewer_choices.push_back("Grids3D");

    auto viewer_prop = new wxEnumProperty("Vertex", wxPG_LABEL, viewer_choices);
    viewer_prop->SetValue(static_cast<int>(PreviewViewer::VertexArray::Rect2D));
    m_pg->Append(viewer_prop);
}

void WxDefaultProperty::OnPropertyGridChanged(wxPropertyGridEvent& event)
{
    wxPGProperty* property = event.GetProperty();
    auto key = property->GetName();
    wxAny val = property->GetValue();

    if (key == "Vertex")
    {
        auto idx = wxANY_AS(val, int);
        m_preview_viewer->SetVertexArray(static_cast<PreviewViewer::VertexArray>(idx));
    }
}

}