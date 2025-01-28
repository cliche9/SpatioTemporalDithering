add_plugin(RayTransparency)

target_sources(RayTransparency PRIVATE
    RayTransparency.cpp
    RayTransparency.h
    RayTransparency.rt.slang
)

target_copy_shaders(RayTransparency RenderPasses/RayTransparency)

target_source_group(RayTransparency "RenderPasses")
