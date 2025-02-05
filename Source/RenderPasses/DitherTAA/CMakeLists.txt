add_plugin(DitherTAA)

target_sources(DitherTAA PRIVATE
    DitherTAA.cpp
    DitherTAA.h
    DitherTaa.ps.slang
)

target_copy_shaders(DitherTAA RenderPasses/DitherTAA)

target_source_group(DitherTAA "RenderPasses")
