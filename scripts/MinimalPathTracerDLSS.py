from pathlib import WindowsPath, PosixPath
from falcor import *

def render_graph_MinimalPathTracer():
    g = RenderGraph('MinimalPathTracer')
    g.create_pass('AccumulatePass', 'AccumulatePass', {'enabled': False, 'outputSize': 'Default', 'autoReset': True, 'precisionMode': 'Single', 'maxFrameCount': 0, 'overflowMode': 'Stop'})
    g.create_pass('ToneMapper', 'ToneMapper', {'outputSize': 'Default', 'useSceneMetadata': True, 'exposureCompensation': 0.0, 'autoExposure': False, 'filmSpeed': 100.0, 'whiteBalance': False, 'whitePoint': 6500.0, 'operator': 'Aces', 'clamp': True, 'whiteMaxLuminance': 1.0, 'whiteScale': 11.199999809265137, 'fNumber': 1.0, 'shutter': 1.0, 'exposureMode': 'AperturePriority'})
    g.create_pass('MinimalPathTracer', 'MinimalPathTracer', {'maxBounces': 1, 'computeDirect': True, 'useImportanceSampling': True})
    g.create_pass('VBufferRT', 'VBufferRT', {'outputSize': 'Default', 'samplePattern': 'Center', 'sampleCount': 16, 'useAlphaTest': False, 'adjustShadingNormals': True, 'forceCullMode': False, 'cull': 'Back', 'useTraceRayInline': False, 'useDOF': True})
    g.create_pass('DLSSPass', 'DLSSPass', {'enabled': True, 'outputSize': 'Default', 'profile': 'Balanced', 'motionVectorScale': 'Relative', 'isHDR': True, 'sharpness': 0.0, 'exposure': 0.0})
    g.add_edge('AccumulatePass.output', 'ToneMapper.src')
    g.add_edge('VBufferRT.vbuffer', 'MinimalPathTracer.vbuffer')
    g.add_edge('VBufferRT.viewW', 'MinimalPathTracer.viewW')
    g.add_edge('MinimalPathTracer.color', 'AccumulatePass.input')
    g.add_edge('MinimalPathTracer.color', 'DLSSPass.color')
    g.add_edge('VBufferRT.depth', 'DLSSPass.depth')
    g.add_edge('VBufferRT.mvec', 'DLSSPass.mvec')
    g.mark_output('ToneMapper.dst')
    g.mark_output('DLSSPass.output')
    return g

MinimalPathTracer = render_graph_MinimalPathTracer()
try: m.addGraph(MinimalPathTracer)
except NameError: None
