#pragma once

#include <wx/panel.h>

class wxPropertyGrid;
class wxPropertyGridEvent;

namespace shaderlab
{

class PreviewViewer;

class WxDefaultProperty : public wxPanel
{
public:
    WxDefaultProperty(wxWindow* parent, 
        const std::shared_ptr<PreviewViewer>& preview_viewer);

private:
    void InitLayout();
    void InitProps();

    void OnPropertyGridChanged(wxPropertyGridEvent& event);

private:
    wxPropertyGrid* m_pg;

    std::shared_ptr<PreviewViewer> m_preview_viewer = nullptr;

}; // WxDefaultProperty

}