#define TYPE uint

RWStructuredBuffer<TYPE> aux_data;
RWStructuredBuffer<TYPE> data;

cbuffer BufferData
{
	uint u_stride;
};

[numthreads(64, 1, 1)]
void main(uint3 globalInvocationID : SV_DispatchThreadID)
{
	data[globalInvocationID.x + u_stride] += aux_data[int(globalInvocationID.x / u_stride)];
}
