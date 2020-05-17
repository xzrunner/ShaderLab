#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

//////////////////////////////////////////////////////////////////////////
// channel
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Channel

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Combine
#define PARM_NODE_NAME combine
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Split
#define PARM_NODE_NAME split
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// input
//////////////////////////////////////////////////////////////////////////

// basic

#define NODES_GROUP_NAME Input-Basic

#define PARM_NODE_CLASS Float
#define PARM_NODE_NAME float1
#include EXE_FILEPATH

#define PARM_NODE_CLASS Float2
#define PARM_NODE_NAME float2
#include EXE_FILEPATH

#define PARM_NODE_CLASS Float3
#define PARM_NODE_NAME float3
#include EXE_FILEPATH

#define PARM_NODE_CLASS Float4
#define PARM_NODE_NAME float4
#include EXE_FILEPATH

#define PARM_NODE_CLASS Int
#define PARM_NODE_NAME int1
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Time
#define PARM_NODE_NAME time
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

// geometry

#define NODES_GROUP_NAME Input-Geometry

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS TexCoords
#define PARM_NODE_NAME tex_coords
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS UV
#define PARM_NODE_NAME uv
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// master
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Master

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Raymarching
#define PARM_NODE_NAME raymarching
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// math
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Math

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Add
#define PARM_NODE_NAME add
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Subtract
#define PARM_NODE_NAME sub
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Multiply
#define PARM_NODE_NAME mul
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Divide
#define PARM_NODE_NAME div
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// procedural
//////////////////////////////////////////////////////////////////////////

// noise

#define NODES_GROUP_NAME Procedural-Noise

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS fBm
#define PARM_NODE_NAME fbm
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS GradientNoise
#define PARM_NODE_NAME gradient_noise
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS SimpleNoise
#define PARM_NODE_NAME simple_noise
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Voronoi
#define PARM_NODE_NAME voronoi
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS WorleyNoise
#define PARM_NODE_NAME worley_noise
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

// sdf

#define NODES_GROUP_NAME Procedural-SDF

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS SDF_Primitives
#define PARM_NODE_NAME sdf_primitives
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

// shapes

#define NODES_GROUP_NAME Procedural-Shapes

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Checkerboard
#define PARM_NODE_NAME checkerboard
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Ellipse
#define PARM_NODE_NAME ellipse
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Polygon
#define PARM_NODE_NAME polygon
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Rectangle
#define PARM_NODE_NAME rectangle
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS RoundedPolygon
#define PARM_NODE_NAME rounded_polygon
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS RoundedRectangle
#define PARM_NODE_NAME rounded_rectangle
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// utility
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Utility

// Illumination

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Phong
#define PARM_NODE_NAME phong
#include EXE_FILEPATH

// shader

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS FragmentShader
#define PARM_NODE_NAME frag_shader
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// uv
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME UV

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Flipbook
#define PARM_NODE_NAME flipbook
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS PolarCoordinates
#define PARM_NODE_NAME polar_coordinates
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS RadialShear
#define PARM_NODE_NAME radial_shear
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Rotate
#define PARM_NODE_NAME rotate
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS Spherize
#define PARM_NODE_NAME spherize
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_CLASS TilingAndOffset
#define PARM_NODE_NAME tiling_and_offset
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define ENABLE_PREVIEW
#define PARM_NODE_CLASS Twirl
#define PARM_NODE_NAME twirl
#include EXE_FILEPATH

#undef NODES_GROUP_NAME
