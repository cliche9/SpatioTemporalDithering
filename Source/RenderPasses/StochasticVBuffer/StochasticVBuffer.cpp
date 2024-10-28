/***************************************************************************
 # Copyright (c) 2015-23, NVIDIA CORPORATION. All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions
 # are met:
 #  * Redistributions of source code must retain the above copyright
 #    notice, this list of conditions and the following disclaimer.
 #  * Redistributions in binary form must reproduce the above copyright
 #    notice, this list of conditions and the following disclaimer in the
 #    documentation and/or other materials provided with the distribution.
 #  * Neither the name of NVIDIA CORPORATION nor the names of its
 #    contributors may be used to endorse or promote products derived
 #    from this software without specific prior written permission.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY
 # EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 # IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 # PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 # CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 # EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 # PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 # PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 # OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 # OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/
#include "StochasticVBuffer.h"

namespace
{
    const std::string kVbuffer = "vbuffer";
    const std::string kRayDiffs = "duv"; // uv differentials
    const std::string kMotion = "mvec";
    const std::string kRayDir = "rayDir";

    const uint32_t kMaxPayloadSizeBytes = 4;
    const std::string kProgramRaytraceFile = "RenderPasses/StochasticVBuffer/StochasticVBuffer.rt.hlsl";
}

extern "C" FALCOR_API_EXPORT void registerPlugin(Falcor::PluginRegistry& registry)
{
    registry.registerClass<RenderPass, StochasticVBuffer>();
}

StochasticVBuffer::StochasticVBuffer(ref<Device> pDevice, const Properties& props)
    : RenderPass(pDevice)
{
}

Properties StochasticVBuffer::getProperties() const
{
    return {};
}

RenderPassReflection StochasticVBuffer::reflect(const CompileData& compileData)
{
    // Define the required resources here
    RenderPassReflection reflector;
    reflector.addOutput(kVbuffer, "V-buffer").format(HitInfo::kDefaultFormat);
    reflector.addOutput(kMotion, "Motion vector").format(ResourceFormat::RG32Float);
    reflector.addOutput(kRayDiffs, "Ray differentials (dUVdx, dUVdy)").format(ResourceFormat::RGBA32Float);
    reflector.addOutput(kRayDir, "incomming camera ray direction").format(ResourceFormat::RG32Float);
    return reflector;
}

void StochasticVBuffer::execute(RenderContext* pRenderContext, const RenderData& renderData)
{
    if(!mpScene) return;
    assert(mpProgram);
    assert(mpVars);

    auto pVbuffer = renderData[kVbuffer]->asTexture();
    auto pMotion = renderData[kMotion]->asTexture();
    auto pRayDiffs = renderData[kRayDiffs]->asTexture();
    auto pRayDir = renderData[kRayDir]->asTexture();

    auto var = mpVars->getRootVar();
    var["gVBuffer"] = pVbuffer;
    var["gMotion"] = pMotion;
    var["gRayDiffs"] = pRayDiffs;
    var["gRayDir"] = pRayDir;

    uint3 dispatch = uint3(1);
    dispatch.x = pVbuffer->getWidth();
    dispatch.y = pVbuffer->getHeight();
    mpScene->raytrace(pRenderContext, mpProgram.get(), mpVars, dispatch);
}

void StochasticVBuffer::renderUI(Gui::Widgets& widget)
{

}

void StochasticVBuffer::setScene(RenderContext* pRenderContext, const ref<Scene>& pScene)
{
    mpScene = pScene;
    setupProgram();
}

void StochasticVBuffer::setupProgram()
{
    if (!mpScene) return;

    DefineList defines;
    defines.add(mpScene->getSceneDefines());
    defines.add(mpSampleGenerator->getDefines());

    RtProgram::Desc desc;
    desc.addShaderModules(mpScene->getShaderModules());
    desc.addShaderLibrary(kProgramRaytraceFile);
    desc.addTypeConformances(mpScene->getTypeConformances());
    desc.setMaxPayloadSize(kMaxPayloadSizeBytes);
    desc.setMaxAttributeSize(mpScene->getRaytracingMaxAttributeSize());
    desc.setMaxTraceRecursionDepth(1);

    ref<RtBindingTable> sbt = RtBindingTable::create(1, 1, mpScene->getGeometryCount());
    sbt->setRayGen(desc.addRayGen("rayGen"));
    sbt->setMiss(0, desc.addMiss("miss"));
    sbt->setHitGroup(0, mpScene->getGeometryIDs(Scene::GeometryType::TriangleMesh), desc.addHitGroup("closestHit", "anyHit"));

    // Add hit group with intersection shader for triangle meshes with displacement maps.
    /*if (mpScene->hasGeometryType(Scene::GeometryType::DisplacedTriangleMesh))
    {
        sbt->setHitGroup(0, mpScene->getGeometryIDs(Scene::GeometryType::DisplacedTriangleMesh), desc.addHitGroup("displacedTriangleMeshClosestHit", "", "displacedTriangleMeshIntersection"));
    }

    // Add hit group with intersection shader for curves (represented as linear swept spheres).
    if (mpScene->hasGeometryType(Scene::GeometryType::Curve))
    {
        sbt->setHitGroup(0, mpScene->getGeometryIDs(Scene::GeometryType::Curve), desc.addHitGroup("curveClosestHit", "", "curveIntersection"));
    }

    // Add hit group with intersection shader for SDF grids.
    if (mpScene->hasGeometryType(Scene::GeometryType::SDFGrid))
    {
        sbt->setHitGroup(0, mpScene->getGeometryIDs(Scene::GeometryType::SDFGrid), desc.addHitGroup("sdfGridClosestHit", "", "sdfGridIntersection"));
    }*/

    mpProgram = RtProgram::create(mpDevice, desc, defines);
    mpVars = RtProgramVars::create(mpDevice, mpProgram, sbt);

    // Bind static resources.
    ShaderVar var = mpVars->getRootVar();
    mpSampleGenerator->setShaderData(var);
}
