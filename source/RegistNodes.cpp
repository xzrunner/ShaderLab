#include "shaderlab/RegistNodes.h"

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

}

namespace shaderlab
{

void nodes_regist_rttr()
{
}

}