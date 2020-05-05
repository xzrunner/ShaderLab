#define XSTR(s) STR(s)
#define STR(s) #s

#ifndef PARM_NODE_CLASS
#error "You must define PARM_NODE_CLASS macro before include this file"
#endif

#ifndef PARM_NODE_NAME
#error "You must define PARM_NODE_NAME macro before include this file"
#endif

#define Bool    bool
#define Int     int
#define Int2    sm::ivec2
#define Int3    sm::ivec3
#define Int4    sm::ivec4
#define Float   float
#define Float2  sm::vec2
#define Float3  sm::vec3
#define Float4  sm::vec4
#define Matrix4 sm::mat4
#define String  std::string
#define Array   std::vector

class PARM_NODE_CLASS : public Node
{
public:
	PARM_NODE_CLASS()
        : Node(XSTR(PARM_NODE_CLASS))
	{
        InitPins(XSTR(PARM_NODE_NAME));
	}

	RTTR_ENABLE(Node)
};

#undef Bool
#undef Int
#undef Int2
#undef Int3
#undef Int4
#undef Float
#undef Float2
#undef Float3
#undef Float4
#undef Matrix4
#undef String
#undef Array

#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
