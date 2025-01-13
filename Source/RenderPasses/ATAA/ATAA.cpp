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
#include "ATAA.h"

namespace
{
    const std::string kColorIn = "colorIn";
    const std::string kMotionVec = "motionVecs";
    const std::string kDepth = "linearDepth";
    const std::string kNormals = "normals";
    const std::string kMeshId = "meshId";
    const std::string kTaaOut = "taaOut"; // output from TAA
    const std::string kColorOut = "colorOut"; // final output (TAA+FXAA+RT mixed)
    const std::string kPrevColor = "prevColor";
    const std::string kSegmentationMask = "segmentationMask";

    const std::string kFxaaQuality = "fxaaQuality";
    const std::string kAlpha = "alpha";
        
    const std::string kTaaShaderFilename = "RenderPasses/ATAA/TAA.ps.slang";
    const std::string kFxaaShaderFilename = "RenderPasses/FXAA/FXAA.ps.slang";
        
}

extern "C" FALCOR_API_EXPORT void registerPlugin(Falcor::PluginRegistry& registry)
{
    registry.registerClass<RenderPass, ATAA>();
}

ATAA::ATAA(ref<Device> pDevice, const Properties& props)
    : RenderPass(pDevice)
{
}

Properties ATAA::getProperties() const
{
    return {};
}

RenderPassReflection ATAA::reflect(const CompileData& compileData)
{
    // Define the required resources here
    RenderPassReflection reflector;
    reflector.addInput(kColorIn, "Colors of current frame");
    reflector.addInput(kDepth, "linear depths");
    reflector.addInput(kMotionVec, "Screen-space motion vectors");
    reflector.addInput(kNormals, "Surface normals");
    reflector.addInput(kMeshId, "Mesh ID");

    reflector.addOutput(kSegmentationMask, "Segmentation mask").format(ResourceFormat::RGBA8Unorm);
    reflector.addOutput(kTaaOut, "Temporal Anti-Aliased color");
    reflector.addOutput(kColorOut, "Anti-Aliased color");
    return reflector;
}

void ATAA::execute(RenderContext* pRenderContext, const RenderData& renderData)
{
    const auto& pColorIn = renderData.getTexture(kColorIn);
    const auto& pDepth = renderData.getTexture(kDepth);
    const auto& pMotionVec = renderData.getTexture(kMotionVec);
    const auto& pNormals = renderData.getTexture(kNormals);
    const auto& pMeshId = renderData.getTexture(kMeshId);

    const auto& pSegmentationMask = renderData.getTexture(kSegmentationMask);
    const auto& pTaaOut = renderData.getTexture(kTaaOut);
    const auto& pColorOut = renderData.getTexture(kColorOut);
    if(!mEnabled)
    {
        pRenderContext->blit(pColorIn->getSRV(), pColorOut->getRTV());
        return;
    }

    allocatePrevColor(pColorIn.get());

    {
        FALCOR_PROFILE(pRenderContext, "TAA");
        mpFbo->attachColorTarget(pTaaOut, 0);
        mpFbo->attachColorTarget(pSegmentationMask, 1);

        auto var = mpTaaPass->getRootVar();
        var["PerFrameCB"]["gAlpha"] = mAlpha;

        var["gTexColor"] = pColorIn;
        var["gTexDepth"] = pDepth;
        var["gTexNormals"] = pNormals;
        var["gMeshId"] = pMeshId;
        var["gTexMotionVec"] = pMotionVec;
        var["gTexPrevColor"] = mpPrevColor;

        var["gSampler"] = mpLinearSampler;

        mpTaaPass->execute(pRenderContext, mpFbo);
    }

    pRenderContext->blit(pTaaOut->getSRV(), pColorOut->getRTV());

    // copy output color to previous color for next frame
    pRenderContext->blit(pColorOut->getSRV(), mpPrevColor->getRTV());
}

void ATAA::renderUI(Gui::Widgets& widget)
{
}

void ATAA::allocatePrevColor(const Texture* pColor)
{
    bool allocate = mpPrevColor == nullptr;
    allocate = allocate || (mpPrevColor->getWidth() != pColor->getWidth());
    allocate = allocate || (mpPrevColor->getHeight() != pColor->getHeight());
    allocate = allocate || (mpPrevColor->getDepth() != pColor->getDepth());
    allocate = allocate || (mpPrevColor->getFormat() != pColor->getFormat());
    FALCOR_ASSERT(pColorOut->getSampleCount() == 1);

    if (!allocate) return;
    mpPrevColor = Texture::create2D(mpDevice, pColor->getWidth(), pColor->getHeight(), pColor->getFormat(), 1, 1, nullptr, Resource::BindFlags::RenderTarget | Resource::BindFlags::ShaderResource);
}
