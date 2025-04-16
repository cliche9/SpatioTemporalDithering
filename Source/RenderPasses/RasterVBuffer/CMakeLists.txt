add_plugin(RasterVBuffer)

target_sources(RasterVBuffer PRIVATE
    RasterVBuffer.cpp
    RasterVBuffer.h
    RasterVBuffer.3D.slang
)

target_copy_shaders(RasterVBuffer RenderPasses/RasterVBuffer)

target_source_group(RasterVBuffer "RenderPasses")
