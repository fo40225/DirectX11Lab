struct BufType
{
	int i;
};

StructuredBuffer<BufType> Buffer0 : register(t0);
StructuredBuffer<BufType> Buffer1 : register(t1);
RWStructuredBuffer<BufType> BufferOut : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
	BufferOut[DTid.x].i = Buffer0[DTid.x].i + Buffer1[DTid.x].i;
}