#include "shaderlab/RegistNodes.h"

#include "shaderlab/node/CustomBlock.h"

#include <ee0/ReflectPropTypes.h>

RTTR_REGISTRATION
{

// base

rttr::registration::class_<shaderlab::Node>("shaderlab::node")
.property("name", &shaderlab::Node::GetName, &shaderlab::Node::SetName)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))
)
;

#define EXE_FILEPATH "shaderlab/node_rttr_gen.h"
#define SKIP_FILE_NODE
#include "shaderlab/node_regist_cfg.h"
#undef SKIP_FILE_NODE
#undef EXE_FILEPATH


rttr::registration::class_<shaderlab::node::CustomBlock>("shaderlab::custom_block")
.constructor<>()
.property("code", &shaderlab::node::CustomBlock::GetCode, &shaderlab::node::CustomBlock::SetCode)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Code")),
    rttr::metadata(ee0::PropEditCodeTag(), true)
)
;

}

namespace shaderlab
{

void nodes_regist_rttr()
{
}

}