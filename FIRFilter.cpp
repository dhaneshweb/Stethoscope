#include "FIRFilter.h"

const int FIRFilter::parameters[64] =
{0 , 0 , 0 , 0 , 0 , -1 , -1 , -1 , 0 , 0 , 0 , 0 , 0 , 0 , -1 , -2 , 
-3 , -4 , -5 , -6 , -7 , -7 , -7 , -5 , -3 , 1 , 5 , 9 , 13 , 16 , 19 , 20 , 
20 , 19 , 16 , 13 , 9 , 5 , 1 , -3 , -5 , -7 , -7 , -7 , -6 , -5 , -4 , -3 , 
-2 , -1 , 0 , 0 , 0 , 0 , 0 , 0 , -1 , -1 , -1 , 0 , 0 , 0 , 0 , 0 };

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

