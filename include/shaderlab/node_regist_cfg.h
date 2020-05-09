#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

//////////////////////////////////////////////////////////////////////////
// channel
//////////////////////////////////////////////////////////////////////////

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Combine
#define PARM_NODE_NAME combine
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Split
#define PARM_NODE_NAME split
#include EXE_FILEPATH

//////////////////////////////////////////////////////////////////////////
// input
//////////////////////////////////////////////////////////////////////////

// basic

#define PARM_NODE_CLASS Float
#define PARM_NODE_NAME float1
#include EXE_FILEPATH

#define PARM_NODE_CLASS Int
#define PARM_NODE_NAME int1
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Time
#define PARM_NODE_NAME time
#include EXE_FILEPATH

// geometry

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS TexCoords
#define PARM_NODE_NAME tex_coords
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS UV
#define PARM_NODE_NAME uv
#include EXE_FILEPATH

//////////////////////////////////////////////////////////////////////////
// math
//////////////////////////////////////////////////////////////////////////

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Multiply
#define PARM_NODE_NAME mul
#include EXE_FILEPATH

//////////////////////////////////////////////////////////////////////////
// noise
//////////////////////////////////////////////////////////////////////////

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS fBm
#define PARM_NODE_NAME fbm
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Worley
#define PARM_NODE_NAME worley
#include EXE_FILEPATH

//////////////////////////////////////////////////////////////////////////
// utility
//////////////////////////////////////////////////////////////////////////

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS FragmentShader
#define PARM_NODE_NAME frag_shader
#include EXE_FILEPATH
