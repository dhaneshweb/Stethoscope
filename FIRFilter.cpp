#include "FIRFilter.h"

const int FIRFilter::parameters[64] =
{0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , -1 , -2 , -2 , -3 , -3 , -2 , 
-1 , 0 , 0 , 0 , -1 , -4 , -7 , -10 , -11 , -10 , -6 , 2 , 11 , 20 , 28 , 32 , 
32 , 28 , 20 , 11 , 2 , -6 , -10 , -11 , -10 , -7 , -4 , -1 , 0 , 0 , 0 , -1 , 
-2 , -3 , -3 , -2 , -2 , -1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0};

FIRFilter::FIRFilter(int totalDecay)
	:filterPointer(0), decay(totalDecay)
{
}

int FIRFilter::push_filter_buffer(int val)
{
	short val1 = (short)(*((short *)&val));
	short val2 = (short)(*(((short *)&val)+1));
	short avg_val1, avg_val2;
	filterBuffer[filterPointer] = val1;
	avg_val1 = filter_buffer_average();
	filterPointer = (++filterPointer) & filterMask;
	filterBuffer[filterPointer] = val2;
	avg_val2 = filter_buffer_average();
	filterPointer = (++filterPointer) & filterMask;
	return (avg_val1 << 16) +  avg_val2;	
}

int FIRFilter::filter_buffer_average()
{
	int sum = 0;
	int nowPointer = filterPointer - 1;
	nowPointer = nowPointer < 0 ? nowPointer + filterLength : nowPointer;
	for(int i = 0; i < filterLength; i++, nowPointer++)
	{
		nowPointer &= filterMask;
		sum += filterBuffer[nowPointer] * parameters[i];
	}
	return sum / decay;
}

