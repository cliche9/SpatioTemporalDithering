import Scene.Raster;

Texture2D<PackedHitInfo> gVBuffer;
RWTexture2D<float4> gPosW;
RWTexture2D<float4> gNormalW;
RWTexture2D<float4> gFaceNormalW;
RWTexture2D<float> gRasterZ;
RWTexture2D<float> gLinearZ;


[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID, uint3 group : SV_GroupID, uint3 localId : SV_GroupThreadID)
{
    const uint2 pixel = id.xy;
    const HitInfo hit = HitInfo(gVBuffer[pixel]);
    if (hit.isValid() && hit.getType() == HitType::Triangle)
    {
        const TriangleHit triangleHit = hit.getTriangleHit();
        let v = gScene.getVertexData(triangleHit);
        //materialID = gScene.getMaterialID(triangleHit.instanceID);

        gPosW[pixel] = float4(v.posW, 1.0);
        gNormalW[pixel] = float4(v.normalW, 0.0);
        gFaceNormalW[pixel] = float4(v.faceNormalW, 0.0);
        //float4 posW = mul(gScene.camera.data.viewProjMat, float4(v.posW, 1.0));
        float4 posW = mul(gScene.camera.data.viewProjMatNoJitter, float4(v.posW, 1.0));
        gRasterZ[pixel] = posW.z / posW.w;
        gLinearZ[pixel] = posW.w;
    }
    else // invalid hit => default values
    {
        gPosW[pixel] = 0.0;
        gNormalW[pixel] = 0.0;
        gFaceNormalW[pixel] = 0.0;
        gRasterZ[pixel] = 1.0;
        gLinearZ[pixel] = gScene.camera.data.farZ;
    }
}
