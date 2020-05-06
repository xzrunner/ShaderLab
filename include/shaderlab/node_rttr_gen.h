#define XSTR(s) STR(s)
#define STR(s) #s

#ifndef PARM_NODE_CLASS
#error "You must define PARM_NODE_CLASS macro before include this file"
#endif

#ifndef PARM_NODE_NAME
#error "You must define PARM_NODE_NAME macro before include this file"
#endif

#ifndef PARM_FILEPATH
#define PARM_FILEPATH shadergraph/block/##PARM_NODE_CLASS##.parm.h
#endif

#define RTTR_NAME shaderlab::##PARM_NODE_NAME

rttr::registration::class_<shaderlab::node::PARM_NODE_CLASS>(XSTR(RTTR_NAME))
.constructor<>()

#define PARAM_INFO(id, type, name, member, default_val)        \
.property(#name, &shaderlab::node::##PARM_NODE_CLASS::member) \
(                                                              \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(#id)) \
)
#ifndef NO_PARM_FILEPATH
#include XSTR(PARM_FILEPATH)
#endif // NO_PARM_FILEPATH
#undef PARAM_INFO

;

#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS

#undef NO_PARM_FILEPATH