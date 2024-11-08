add_plugin(MultiResTransparencyTracing)

target_sources(MultiResTransparencyTracing PRIVATE
    MultiResTransparencyTracing.cpp
    MultiResTransparencyTracing.h
    MultiResTransparencyTracing.rt.slang
    Pull.cs.slang
)

target_source_group(MultiResTransparencyTracing "RenderPasses")
