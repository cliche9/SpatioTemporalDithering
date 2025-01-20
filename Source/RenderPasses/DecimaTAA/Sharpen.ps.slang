cbuffer PerFrameCB
{
    float2 dir;
};

Texture2D<float4> gInput;
SamplerState gSampler;

float4 main(float2 texC : TEXCOORD) : SV_TARGET0
{
    uint2 texDim;
    uint levels;
    gInput.GetDimensions(0, texDim.x, texDim.y, levels);
    float2 step = 1.0 / float2(texDim.x, texDim.y);

    // TODO catmull-like sharpen

    return gInput.SampleLevel(gSampler, texC, 0);
}
