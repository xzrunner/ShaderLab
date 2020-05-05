#ifndef PIN_INFO
#error "You must define PIN_INFO macro before include this file"
#endif

PIN_INFO(ANY, (255, 255, 255), "")

PIN_INFO(BOOL,  (255, 0, 0), "b")
PIN_INFO(BOOL2, (255, 0, 0), "b2")
PIN_INFO(BOOL3, (255, 0, 0), "b3")
PIN_INFO(BOOL4, (255, 0, 0), "b4")

PIN_INFO(UINT, (255, 0, 0), "ui")

PIN_INFO(INT,  (255, 0, 0), "i")
PIN_INFO(INT2, (255, 0, 0), "i2")
PIN_INFO(INT3, (255, 0, 0), "i3")
PIN_INFO(INT4, (255, 0, 0), "i4")

PIN_INFO(FLOAT,  (255, 0, 0), "f")
PIN_INFO(FLOAT2, (255, 0, 0), "f2")
PIN_INFO(FLOAT3, (255, 0, 0), "f3")
PIN_INFO(FLOAT4, (255, 0, 0), "f4")

PIN_INFO(MAT2, (255, 0, 0), "m2")
PIN_INFO(MAT3, (255, 0, 0), "m3")
PIN_INFO(MAT4, (255, 0, 0), "m4")

PIN_INFO(SAMPLER_2D,   (255, 0, 0), "t2")
PIN_INFO(SAMPLER_CUBE, (255, 0, 0), "tc")

PIN_INFO(ARRAY,    (255, 0, 0), "A")
PIN_INFO(STRUCT,   (255, 0, 0), "S")
PIN_INFO(FUNCTION, (255, 0, 0), "F")
