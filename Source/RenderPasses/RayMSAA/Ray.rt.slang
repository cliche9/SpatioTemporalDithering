import Scene.Raytracing;

Texture2D<float> gDepthTex;
Texture2D<uint> gEdgeTex;
RWTexture2DArray<float> gDepthSamples;

struct RayData
{
    float depth;
};

[shader("anyhit")]
void anyHit(inout RayData rayData, BuiltInTriangleIntersectionAttributes attribs)
{
    // TODO check if hit
}

[shader("miss")]
void miss(inout RayData rayData)
{
}

[shader("closesthit")]
void closestHit(inout RayData rayData, BuiltInTriangleIntersectionAttributes attribs)
{
    rayData.depth = RayTCurrent();
}

[shader("raygeneration")]
void rayGen()
{
    uint3 id = DispatchRaysIndex();
    uint edge = gEdgeTex[id.xy];
    if(!edge) return;

    // TODO trace rays
}
