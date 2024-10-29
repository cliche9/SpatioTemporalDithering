add_plugin(StochasticVBuffer)

target_sources(StochasticVBuffer PRIVATE
    StochasticVBuffer.cpp
    StochasticVBuffer.h
    StochasticVBuffer.rt.slang
)

target_source_group(StochasticVBuffer "RenderPasses")
