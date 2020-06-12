#include "shaderlab/ConnectPinOP.h"
#include "shaderlab/PinType.h"
#include "shaderlab/WxGraphPage.h"
#include "shaderlab/node/SubGraph.h"

#include <blueprint/Node.h>
#include <blueprint/Pin.h>

#include <ns/NodeFactory.h>
#include <node2/CompTransform.h>
#include <shadergraph/ValueImpl.h>
#include <shadergraph/block/SubGraph.h>

namespace shaderlab
{
ConnectPinOP::ConnectPinOP(const std::shared_ptr<pt0::Camera>& cam,
                           ee0::WxStagePage& stage, const std::vector<bp::NodePtr>& nodes)
    : bp::ConnectPinOP(cam, stage, nodes)
{
    m_is_port_matched = [](const bp::Node& node, const bp::Pin& pin) -> bool
    {
        if (pin.IsInput() && pin.GetType() == PIN_FUNCTION &&
            node.get_type() == rttr::type::get<node::SubGraph>()) {
            return true;
        } else {
            return false;
        }
    };
}

void ConnectPinOP::BeforeConnect(const std::shared_ptr<bp::Pin>& pin,
                                 const bp::Node& bp_node, const n0::SceneNode& node)
{
    if (pin->IsInput() && pin->GetType() == PIN_FUNCTION &&
        bp_node.get_type() == rttr::type::get<node::SubGraph>()) {
        SetupSubGraph(pin, bp_node, node);
    }
}

void ConnectPinOP::SetupSubGraph(const std::shared_ptr<bp::Pin>& pin,
                                 const bp::Node& bp_node, const n0::SceneNode& node)
{
    assert(node.HasSharedComp<n0::CompComplex>());
    auto& ccomplex = node.GetSharedComp<n0::CompComplex>();

    auto& from_front = static_cast<const node::SubGraph&>(bp_node);
    auto eval = static_cast<WxGraphPage&>(m_stage).GetSceneTree()->GetCurrEval();
    auto from_back = eval->QueryBackNode(from_front);
    assert(from_back);
    auto& to_front = pin->GetParent();
    auto& to_back = eval->QueryBackNode(to_front);

    auto& port = to_back->GetImports()[pin->GetPosIdx()].var.type;
    assert(port.type == shadergraph::VarType::Function);
    auto f_val = std::static_pointer_cast<shadergraph::FunctionVal>(port.val);
    assert(f_val);
    shadergraph::Variant v_func;
    v_func.type = shadergraph::VarType::Function;
    v_func.name = port.name;
    auto& sg_front = const_cast<node::SubGraph&>(from_front);
    sg_front.Setup(f_val->inputs, { f_val->output, v_func });

    auto& children = sg_front.GetChildren();
    for (int i = 0, n = children.size(); i < n; ++i)
    {
        auto& bp_node = children[i];
        auto node = ns::NodeFactory::Create2D();

        auto& cnode = node->AddUniqueComp<bp::CompNode>();
        cnode.SetNode(bp_node);

        auto& caabb = node->GetUniqueComp<n2::CompBoundingBox>();
        auto& style = bp_node->GetStyle();
        caabb.SetSize(*node, sm::rect(style.width, style.height));

        auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
        const float x = i < f_val->inputs.size() ? -200 : 200;
        const float y = -50 * i;
        ctrans.SetPosition(*node, sm::vec2(x, y));

        n0::NodeFlagsHelper::SetFlag<n0::NodeNotVisible>(*node, true);

        ccomplex.AddChild(node);
    }

    auto sg_back = std::static_pointer_cast<shadergraph::block::SubGraph>(from_back);
    sg_back->Setup(sg_front.GetBackGraph(), f_val->inputs, { f_val->output, v_func });
}

}