from pathlib import WindowsPath, PosixPath
from falcor import *

def render_graph_Forward():
    g = RenderGraph('Forward')
    g.create_pass('VBufferLighting', 'VBufferLighting', {'envMapIntensity': 0.4000000059604645, 'ambientIntensity': 0.30000001192092896})
    g.create_pass('ToneMapper', 'ToneMapper', {'outputSize': 'Default', 'useSceneMetadata': True, 'exposureCompensation': -2.799999952316284, 'autoExposure': False, 'filmSpeed': 100.0, 'whiteBalance': False, 'whitePoint': 6500.0, 'operator': 'Linear', 'clamp': False, 'whiteMaxLuminance': 1.0, 'whiteScale': 11.199999809265137, 'fNumber': 1.0, 'shutter': 1.0, 'exposureMode': 'AperturePriority'})
    g.create_pass('TAA', 'TAA', {'alpha': 0.10000000149011612, 'colorBoxSigma': 0.5, 'antiFlicker': True})
    g.create_pass('StochasticVBuffer', 'StochasticVBuffer', {})
    g.create_pass('AccumulatePass', 'AccumulatePass', {'enabled': False, 'outputSize': 'Default', 'autoReset': True, 'precisionMode': 'Single', 'maxFrameCount': 0, 'overflowMode': 'Stop'})
    g.add_edge('ToneMapper.dst', 'TAA.colorIn')
    g.add_edge('StochasticVBuffer.rayDir', 'VBufferLighting.rayDir')
    g.add_edge('StochasticVBuffer.vbuffer', 'VBufferLighting.vbuffer')
    g.add_edge('StochasticVBuffer.mvec', 'TAA.motionVecs')
    g.add_edge('VBufferLighting.color', 'AccumulatePass.input')
    g.add_edge('AccumulatePass.output', 'ToneMapper.src')
    g.mark_output('ToneMapper.dst')
    g.mark_output('TAA.colorOut')
    return g

Forward = render_graph_Forward()
try: m.addGraph(Forward)
except NameError: None
