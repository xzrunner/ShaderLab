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

class PARM_NODE_CLASS : public Node
{
public:
	PARM_NODE_CLASS()
        : Node(XSTR(PARM_NODE_CLASS))
	{
        Init(XSTR(PARM_NODE_NAME));
	}

#define PARAM_INFO(id, type, name, member, default_val) \
    type member = type##default_val;
#ifndef NO_PARM_FILEPATH
#include XSTR(PARM_FILEPATH)
#endif // NO_PARM_FILEPATH
#undef  PARAM_INFO

	RTTR_ENABLE(Node)
};

#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS

#undef NO_PARM_FILEPATH
