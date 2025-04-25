#define WK_SIZE 1024

#define TYPEV4 uint4
#define TYPE uint

RWStructuredBuffer<TYPEV4> in_buffer;
RWStructuredBuffer<TYPEV4> data;

RWStructuredBuffer<TYPE> aux;

cbuffer BufferData
{
	uint u_bufferSize;
	uint u_writeAux;
};

groupshared TYPE s_temp[WK_SIZE * 2];
groupshared TYPE s_blockSums[WK_SIZE / 32];

// Inclusive scan which produces the prefix sums for 64 elements in parallel
void intraWarpScan(uint threadID)
{
	int id = threadID * 2;
	s_temp[id + 1] += s_temp[id];
	
	id = (threadID | 1) << 1;
	s_temp[id + (threadID & 1)] += s_temp[id - 1];

	id = ((threadID >> 1) | 1) << 2;
	s_temp[id + (threadID & 3)] += s_temp[id - 1];

	id = ((threadID >> 2) | 1) << 3;
	s_temp[id + (threadID & 7)] += s_temp[id - 1];

	id = ((threadID >> 3) | 1) << 4;
	s_temp[id + (threadID & 15)] += s_temp[id - 1];

	id = ((threadID >> 4) | 1) << 5;
	s_temp[id + (threadID & 31)] += s_temp[id - 1];
}

void intraBlockScan(uint threadID)
{
	int id = threadID * 2;
	s_blockSums[id + 1] += s_blockSums[id];
	
	id = (threadID | 1) << 1;
	s_blockSums[id + (threadID & 1)] += s_blockSums[id - 1];

	id = ((threadID >> 1) | 1) << 2;
	s_blockSums[id + (threadID & 3)] += s_blockSums[id - 1];

	id = ((threadID >> 2) | 1) << 3;
	s_blockSums[id + (threadID & 7)] += s_blockSums[id - 1];

	id = ((threadID >> 3) | 1) << 4;
	s_blockSums[id + (threadID & 15)] += s_blockSums[id - 1];
}

[numthreads(WK_SIZE, 1, 1)]
void main(uint3 localInvocationID : SV_GroupThreadID, uint3 workGroupID : SV_GroupID)
{
	uint threadID = localInvocationID.x;
	uint idx = workGroupID.x * WK_SIZE * 2 + threadID;
	TYPEV4 inputValuesA = in_buffer[idx];
	TYPEV4 inputValuesB = in_buffer[idx + WK_SIZE];
	inputValuesA.y += inputValuesA.x;
	inputValuesA.z += inputValuesA.y;
	inputValuesA.w += inputValuesA.z;
	s_temp[threadID] = inputValuesA.w;
	inputValuesB.y += inputValuesB.x;
	inputValuesB.z += inputValuesB.y;
	inputValuesB.w += inputValuesB.z;
	s_temp[threadID + WK_SIZE] = inputValuesB.w;
	GroupMemoryBarrierWithGroupSync();
	//barrier();
	//memoryBarrierShared();
	
	// 1. Intra-warp scan in each warp
	intraWarpScan(threadID);
	//intraWarpScan(threadID + WK_SIZE);
	GroupMemoryBarrierWithGroupSync();
	
	// 2. Collect per-warp sums
	if(threadID < (WK_SIZE/32))
		s_blockSums[threadID] = s_temp[threadID * 64 + 63];
		
	// 3. Use 1st warp to scan per-warp results
	if(threadID < (WK_SIZE/64))
		intraBlockScan(threadID);
	GroupMemoryBarrierWithGroupSync();
	
	// 4. Add new warp offsets from step 3 to the results
	//idx = int(gl_WorkGroupID.x * WK_SIZE * 2);
	TYPE blockOffset = threadID < 64 ? 0 : s_blockSums[threadID / 64 - 1];
	TYPE val = s_temp[threadID] + blockOffset;
	if(idx < u_bufferSize / 4)
		data[idx] = TYPEV4(val - inputValuesA.w + inputValuesA.xyz, val);
	
	
	blockOffset = s_blockSums[(threadID + WK_SIZE) / 64 - 1];
	val = s_temp[threadID + WK_SIZE] + blockOffset;

	if(idx + WK_SIZE < u_bufferSize / 4)
		data[idx + WK_SIZE] = TYPEV4(val - inputValuesB.w + inputValuesB.xyz, val);
	
	// 5. The last thread in each block must return into the (thickly packed) auxiliary array
	if(threadID == WK_SIZE-1 && u_writeAux)
		aux[workGroupID.x] = val;
}
