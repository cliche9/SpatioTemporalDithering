from pathlib import WindowsPath, PosixPath
from falcor import *

def render_graph_Dither():
    g = RenderGraph('Dither')
    g.create_pass('DitherVBuffer', 'DitherVBuffer', {'useWhitelist': True, 'whitelist': '/root/_materials/Burn,/root/_materials/Fire_Magic,/root/_materials/Healing,/root/_materials/Hit1,/root/_materials/Hit1_001,/root/_materials/Light,/root/_materials/Sadness_water,/root/_materials/Water_drip,/root/_materials/Wirble,/root/_materials/boss_healthbar,/root/_materials/eff_clouds,/root/_materials/effect_Fire,/root/_materials/effect_barrier,/root/_materials/effect_light,/root/_materials/effect_shield,/root/_materials/effect_thunder,Board,CollectInner,Collectible,Smoke,TransparentPlane1,'})
    g.create_pass('ToneMapper', 'ToneMapper', {'outputSize': 'Default', 'useSceneMetadata': True, 'exposureCompensation': 0.0, 'autoExposure': False, 'filmSpeed': 100.0, 'whiteBalance': False, 'whitePoint': 6500.0, 'operator': 'Linear', 'clamp': False, 'whiteMaxLuminance': 1.0, 'whiteScale': 11.199999809265137, 'fNumber': 1.0, 'shutter': 1.0, 'exposureMode': 'AperturePriority'})
    g.create_pass('DLSSPass', 'DLSSPass', {'enabled': True, 'outputSize': 'Default', 'profile': 'DLAA', 'preset': 'Default(CNN)', 'motionVectorScale': 'Relative', 'isHDR': True, 'useJitteredMV': False, 'sharpness': 0.3499999940395355, 'exposure': 0.0})
    g.create_pass('UnpackVBuffer', 'UnpackVBuffer', {})
    g.create_pass('VideoRecorder', 'VideoRecorder', {})
    g.create_pass('PathBenchmark', 'PathBenchmark', {})
    g.add_edge('DitherVBuffer.mvec', 'DLSSPass.mvec')
    g.add_edge('DitherVBuffer.vbuffer', 'UnpackVBuffer.vbuffer')
    g.add_edge('UnpackVBuffer.rasterZ', 'DLSSPass.depth')
    g.add_edge('VideoRecorder', 'DitherVBuffer')
    g.add_edge('ToneMapper', 'PathBenchmark')
    g.add_edge('DitherVBuffer.color', 'DLSSPass.color')
    g.add_edge('DLSSPass.output', 'ToneMapper.src')
    g.mark_output('ToneMapper.dst')
    return g

Dither = render_graph_Dither()
try: m.addGraph(Dither)
except NameError: None
