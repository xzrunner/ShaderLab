#include "shaderlab/WxGraphCanvas.h"
#include "shaderlab/WxGraphPage.h"
#include "shaderlab/Node.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/CompNode.h>
#include <blueprint/Node.h>

#include <node0/SceneNode.h>
#include <node2/UpdateSystem.h>
#include <node2/RenderSystem.h>
#include <node2/CompTransform.h>

namespace shaderlab
{

WxGraphCanvas::WxGraphCanvas(const ur::Device& dev, WxGraphPage* stage,
                             ECS_WORLD_PARAM const ee0::RenderContext& rc)
    : ee2::WxStageCanvas(dev, stage, ECS_WORLD_VAR &rc)
    , m_stage(stage)
{
}

void WxGraphCanvas::DrawForeground() const
{
    ee2::WxStageCanvas::DrawForeground();

    auto eval = m_stage->GetEval();
    if (!eval) {
        return;
    }

    auto screen_region = CalcScreenRegion();
	m_stage->Traverse([&](const ee0::GameObj& obj)->bool
	{
        if (screen_region.IsValid() && ScreenCullingTest(screen_region, obj)) {
            return true;
        }

        if (!obj->HasUniqueComp<bp::CompNode>()) {
            return true;
        }

        auto& front_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
        assert(front_node);
        if (!front_node->get_type().is_derived_from<Node>()) {
            return true;
        }

		return true;
	});
}

bool WxGraphCanvas::OnUpdate()
{
	bool dirty = false;
	m_stage->Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (n2::UpdateSystem::Instance()->Update(obj)) {
			dirty = true;
		}
		return true;
	});

	if (dirty) {
		m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_UPDATE_NODES);
	}
	return dirty;
}

}