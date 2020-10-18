#include "shaderlab/ReflectPropTypes.h"

#include <unirender/Device.h>

#include <rttr/registration.h>

#define REGIST_ENUM_ITEM(type, name, label) \
    rttr::value(name, type),                \
    rttr::metadata(type, label)             \

RTTR_REGISTRATION
{

rttr::registration::enumeration<ur::Device::TextureSamplerType>("ur_tex_sampler")
(
    REGIST_ENUM_ITEM(ur::Device::TextureSamplerType::NearestClamp,  "nearest_clamp",  "Nearest Clamp"),
    REGIST_ENUM_ITEM(ur::Device::TextureSamplerType::LinearClamp,   "linear_clamp",   "Linear Clamp"),
    REGIST_ENUM_ITEM(ur::Device::TextureSamplerType::NearestRepeat, "nearest_repeat", "Nearest Repeat"),
    REGIST_ENUM_ITEM(ur::Device::TextureSamplerType::LinearRepeat,  "linear_repeat",  "Linear Repeat")
);

}

namespace shaderlab
{
void prop_types_regist_rttr()
{
}
}