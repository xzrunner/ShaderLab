#pragma once

#include "shaderlab/Node.h"

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