RWTexture2D<float4> gColor;
Texture2D<uint> gHead;

struct BufferData // 4 * 4 byte 
{
    uint next;
    float depth;
    half4 color;
    //uint2 color;
    //float2 color;
};

StructuredBuffer<BufferData> gBuffer;

cbuffer PerFrame
{
    uint2 gFrameDim;
    uint maxElements;
}

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID, uint3 group : SV_GroupID, uint3 localId : SV_GroupThreadID)
{
    uint2 pixel = id.xy;
    if (pixel.x >= gFrameDim.x || pixel.y >= gFrameDim.y)
        return;

    uint head = gHead[pixel];
    if (head >= maxElements)
        return;

    float4 color = float4(0, 0, 0, 1);
    uint count = 0;
    
    // count items
    uint next = head;
    while(next < maxElements)
    {
        BufferData data = gBuffer[next];
        next = data.next;
        // assume sorted
        color.rgb += color.a * data.color.a * data.color.rgb;
        color.a *= (1.0 - data.color.a);
        count++;
    }

    gColor[pixel] = color;
}
