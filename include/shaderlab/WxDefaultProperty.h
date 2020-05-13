#pragma once

#include <wx/panel.h>

class wxPropertyGrid;
class wxPropertyGridEvent;

namespace shaderlab
{

class WxPreviewCanvas;

class WxDefaultProperty : public wxPanel
{
public:
    WxDefaultProperty(wxWindow* parent,
        const std::shared_ptr<WxPreviewCanvas>& preview_canvas);

private:
    void InitLayout();
    void InitProps();

    void OnPropertyGridChanged(wxPropertyGridEvent& event);

private:
    wxPropertyGrid* m_pg;

    std::shared_ptr<WxPreviewCanvas> m_preview_canvas = nullptr;

}; // WxDefaultProperty

}