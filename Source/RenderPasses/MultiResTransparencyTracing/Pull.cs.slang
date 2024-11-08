RWTexture2D<float4> gColor;
RWTexture2D<float> gRayT;
Texture2D<float4> gColorPrev;
Texture2D<float> gRayTPrev;

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID, uint3 group : SV_GroupID, uint3 localId : SV_GroupThreadID)
{
    float4 color = gColor[id.xy];
    //if(color.a < 0.01) return;
    //float rayT = gRayT[id.xy];

    float4 colorPrev = gColorPrev[id.xy / 2];
    
    color.rgb = color.rgb + color.a * colorPrev.rgb;
    color.a = 1.0;
    gColor[id.xy] = color;
}
