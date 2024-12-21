#include "Utils/Math/MathConstants.slangh"
import Scene.Raytracing;

Texture2D<float> gDepthTex;
Texture2D<uint> gEdgeTex;
RWTexture2DArray<float> gDepthSamples;

struct RayData
{
    float depth;
};

// sample positions in (-0.5, 0.5)
static const float2 sample_pos[8] = {
    float2( 1.0 / 16.0, -3.0 / 16.0),
    float2(-1.0 / 16.0,  3.0 / 16.0),
    float2( 5.0 / 16.0,  1.0 / 16.0),
    float2(-3.0 / 16.0, -5.0 / 16.0),
    float2(-5.0 / 16.0,  5.0 / 16.0),
    float2(-7.0 / 16.0, -1.0 / 16.0),
    float2( 3.0 / 16.0,  7.0 / 16.0),
    float2( 7.0 / 16.0, -7.0 / 16.0)
};


[shader("anyhit")]
void anyHit(inout RayData rayData, BuiltInTriangleIntersectionAttributes attribs)
{
    GeometryInstanceID instanceID = getGeometryInstanceID();
    VertexData v = getVertexData(instanceID, PrimitiveIndex(), attribs);
    const uint materialID = gScene.getMaterialID(instanceID);
    ExplicitLodTextureSampler lod = { 0.0 };

    if (gScene.materials.alphaTest(v, materialID, 0.f)) IgnoreHit();
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

    float2 pixel = float2(id.xy) + 0.5 + sample_pos[id.z];
    RayDesc ray;
    ray.Origin = gScene.camera.data.posW;
    ray.Direction = normalize(gScene.camera.computeNonNormalizedRayDirPinhole(id.xy, DispatchRaysDimensions().xy, sample_pos[id.z] + 0.5));
    ray.TMin = gScene.camera.data.nearZ;
    ray.TMax = gScene.camera.data.farZ;

    RayData rayData;
    rayData.depth = FLT_MAX;

    TraceRay(
        gScene.rtAccel,
        RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES,
        0xff /* instanceInclusionMask */, 0 /* hitIdx */, 1 /*ray type count*/, 0 /* missIdx */,
        ray, rayData
    );

    gDepthSamples[id] = rayData.depth;
}
