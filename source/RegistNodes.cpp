#include "shaderlab/RegistNodes.h"

#include "shaderlab/node/CustomBlock.h"
#include "shaderlab/node/Texture2DAsset.h"
#include "shaderlab/node/SubGraph.h"

#include <ee0/ReflectPropTypes.h>

#include <js/RTTR.h>

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

rttr::registration::class_<shaderlab::node::SubGraph>("shaderlab::sub_graph")
    .constructor<>()
    .property("inputs", &shaderlab::node::SubGraph::GetInputVars, &shaderlab::node::SubGraph::SetInputVars)
    (
        rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Inputs"))
    )
    .property("outputs", &shaderlab::node::SubGraph::GetOutputVars, &shaderlab::node::SubGraph::SetOutputVars)
    (
        rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Outputs"))
    )
;

rttr::registration::class_<shaderlab::node::Texture2DAsset>("shaderlab::texture2d_asset")
    .constructor<>()
    .property("filepath", &shaderlab::node::Texture2DAsset::GetFilepath, &shaderlab::node::Texture2DAsset::SetFilepath)
    (
	    rttr::metadata(js::RTTR::FilePathTag(), true),
	    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),
	    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.png"))
    )
;

}

namespace shaderlab
{

void nodes_regist_rttr()
{
}

}