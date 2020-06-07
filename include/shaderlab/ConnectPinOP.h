#pragma once

#include <blueprint/ConnectPinOP.h>

namespace shaderlab
{

class ConnectPinOP : public bp::ConnectPinOP
{
public:
    ConnectPinOP(const std::shared_ptr<pt0::Camera>& cam, ee0::WxStagePage& stage,
        const std::vector<bp::NodePtr>& nodes);

protected:
    virtual void BeforeConnect(const std::shared_ptr<bp::Pin>& pin,
        const bp::Node& bp_node, const n0::SceneNode& node) override;

private:
    void SetupSubGraph(const std::shared_ptr<bp::Pin>& pin,
        const bp::Node& bp_node, const n0::SceneNode& node);

}; // ConnectPinOP

}