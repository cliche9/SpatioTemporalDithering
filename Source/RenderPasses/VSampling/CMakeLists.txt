add_plugin(VSampling)

target_sources(VSampling PRIVATE
    VSampling.cpp
    VSampling.h
    VBuffer.rt.slang
)

target_copy_shaders(VSampling RenderPasses/VSampling)

target_source_group(VSampling "RenderPasses")
