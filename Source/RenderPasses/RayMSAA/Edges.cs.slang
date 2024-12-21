
Texture2D<float> gDepthTex;
RWTexture2D<uint> gEdgeTex;

SamplerState gSampler;

cbuffer CB
{
    float2 gStep; // = 1 / resolution
    float2 gResolution;
}

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID, uint3 group : SV_GroupID, uint3 localId : SV_GroupThreadID)
{
    float2 uv = (id.xy + 0.5) / gResolution;
    float depth = gDepthTex.SampleLevel(gSampler, uv, 0).r;

    float dLeft = depth - gDepthTex.SampleLevel(gSampler, uv - float2(gStep.x, 0), 0).r;
    float dRight = gDepthTex.SampleLevel(gSampler, uv + float2(gStep.x, 0), 0).r - depth;

    float dUp = gDepthTex.SampleLevel(gSampler, uv + float2(0, gStep.y), 0).r - depth;
    float dDown = depth - gDepthTex.SampleLevel(gSampler, uv - float2(0, gStep.y), 0).r;

    // compute difference in gradients and normalize by depth
    float diffX = abs(dRight - dLeft) / depth;
    float diffY = abs(dUp - dDown) / depth;

    float threshold = 0.001;
    uint edge = (diffX > threshold || diffY > threshold) ? 255 : 0;
    gEdgeTex[id.xy] = edge;
}
