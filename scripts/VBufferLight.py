from pathlib import WindowsPath, PosixPath
from falcor import *

def render_graph_Forward():
    g = RenderGraph('Forward')
    g.create_pass('VBufferLighting', 'VBufferLighting', {'envMapIntensity': 0.4000000059604645, 'ambientIntensity': 0.30000001192092896})
    g.create_pass('GBufferRaster', 'GBufferRaster', {'outputSize': 'Default', 'samplePattern': 'Center', 'sampleCount': 8, 'useAlphaTest': True, 'adjustShadingNormals': True, 'forceCullMode': False, 'cull': 'Back'})
    g.create_pass('RayShadow', 'RayShadow', {})
    g.create_pass('ToneMapper', 'ToneMapper', {'outputSize': 'Default', 'useSceneMetadata': True, 'exposureCompensation': 1.0, 'autoExposure': False, 'filmSpeed': 100.0, 'whiteBalance': False, 'whitePoint': 6500.0, 'operator': 'Linear', 'clamp': False, 'whiteMaxLuminance': 1.0, 'whiteScale': 11.199999809265137, 'fNumber': 1.0, 'shutter': 1.0, 'exposureMode': 'AperturePriority'})
    g.create_pass('TAA', 'TAA', {'alpha': 0.10000000149011612, 'colorBoxSigma': 0.5, 'antiFlicker': True})
    g.add_edge('GBufferRaster.posW', 'RayShadow.posW')
    g.add_edge('GBufferRaster.normW', 'RayShadow.normalW')
    g.add_edge('GBufferRaster.mvec', 'TAA.motionVecs')
    g.add_edge('ToneMapper.dst', 'TAA.colorIn')
    g.add_edge('VBufferLighting.color', 'ToneMapper.src')
    g.add_edge('RayShadow.visibility', 'VBufferLighting.visibilityBuffer')
    g.add_edge('GBufferRaster.vbuffer', 'VBufferLighting.vbuffer')
    g.mark_output('TAA.colorOut')
    return g

Forward = render_graph_Forward()
try: m.addGraph(Forward)
except NameError: None
