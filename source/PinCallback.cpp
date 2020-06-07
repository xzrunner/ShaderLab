#include "shaderlab/PinCallback.h"
#include "shaderlab/PinType.h"
#include "shaderlab/node/SubGraph.h"

#include <painting0/Color.h>
#include <blueprint/Pin.h>
#include <blueprint/Node.h>

#include <string>

#include <assert.h>

namespace
{

const pt0::Color PIN_COLORS[] = {
#define PIN_INFO(type, color, desc) \
    pt0::Color##color,
#include "shaderlab/pin_cfg.h"
#undef PIN_INFO
};

const std::string PIN_DESCS[] = {
#define PIN_INFO(type, color, desc) \
    desc,
#include "shaderlab/pin_cfg.h"
#undef PIN_INFO
};

std::string get_desc_func(const std::string& name, int type)
{
    assert(type >= 0 && type < sizeof(PIN_COLORS) / sizeof(pt0::Color));
    return name + " (" + PIN_DESCS[type] + ")";
}

const pt0::Color& get_color_func(int type)
{
    assert(type >= 0 && type < sizeof(PIN_COLORS) / sizeof(pt0::Color));
    return PIN_COLORS[type];
}

bool can_type_cast_func(int type_from, int type_to)
{
    if (type_from == type_to) {
        return true;
    }
    if (type_from == shaderlab::PIN_ANY ||
        type_to == shaderlab::PIN_ANY) {
        return true;
    }

    if (type_from >= shaderlab::PIN_BOOL && type_from <= shaderlab::PIN_BOOL4 &&
        type_to >= shaderlab::PIN_BOOL && type_to <= shaderlab::PIN_BOOL4) {
        return true;
    }
    if (type_from >= shaderlab::PIN_INT && type_from <= shaderlab::PIN_INT4 &&
        type_to >= shaderlab::PIN_INT && type_to <= shaderlab::PIN_INT4) {
        return true;
    }
    if (type_from >= shaderlab::PIN_FLOAT && type_from <= shaderlab::PIN_FLOAT4 &&
        type_to >= shaderlab::PIN_FLOAT && type_to <= shaderlab::PIN_FLOAT4) {
        return true;
    }

    return false;
}

}

namespace shaderlab
{

void InitPinCallback()
{
    bp::Pin::SetExtendFuncs({
        get_desc_func,
        get_color_func,
        can_type_cast_func
    });
}

}