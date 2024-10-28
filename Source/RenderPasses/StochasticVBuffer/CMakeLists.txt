add_plugin(StochasticVBuffer)

target_sources(StochasticVBuffer PRIVATE
    StochasticVBuffer.cpp
    StochasticVBuffer.h
)

target_source_group(StochasticVBuffer "RenderPasses")
