#pragma once

#include "shaderlab/Node.h"

#include <shadergraph/block/Blend.h>
#include <shadergraph/block/NormalBlend.h>
#include <shadergraph/block/ColorspaceConversion.h>
#include <shadergraph/block/Swizzle.h>
#include <shadergraph/block/Constant.h>

namespace shaderlab
{

void nodes_regist_rttr();

namespace node
{

#define EXE_FILEPATH "shaderlab/node_def_gen.h"
#include "shaderlab/node_regist_cfg.h"
#undef EXE_FILEPATH

}

}