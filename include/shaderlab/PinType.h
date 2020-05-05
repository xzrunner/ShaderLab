#pragma once

namespace shaderlab
{

enum PinType
{
#define PIN_INFO(type, color, desc) \
    PIN_##type,
#include "shaderlab/pin_cfg.h"
#undef PIN_INFO
};

}