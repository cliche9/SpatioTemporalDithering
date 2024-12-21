Texture2D<float4> gColorIn;
RWTexture2D<float4> gColorOut;

Texture2D<float> gDepthTex;
Texture2D<uint> gEdgeTex;
Texture2DArray<float> gDepthSamples;

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
    //float depth = gDepthTex.SampleLevel(gSampler, uv, 0).r;
    float4 color = gColorIn.SampleLevel(gSampler, uv, 0);

    gColorOut[id.xy] = color;
}
