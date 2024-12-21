add_plugin(RayMSAA)

target_sources(RayMSAA PRIVATE
    RayMSAA.cpp
    RayMSAA.h
    Edges.cs.slang
    Ray.rt.slang
    ColorResolve.cs.slang
)

target_copy_shaders(RayMSAA RenderPasses/RayMSAA)

target_source_group(RayMSAA "RenderPasses")
