#include "AverageFilter.h"

AverageFilter::AverageFilter()
	:filterPointer(0)
{
	for(int i=0; i<filterLength; i++)
	{
		filterBuffer[i] = 0;
	}	
}

int AverageFilter::push_filter_buffer(int val)
{
	short val1 = (short)(*((short *)&val));
	short val2 = (short)(*(((short *)&val)+1));
	short avg_val1, avg_val2;
	filterBuffer[filterPointer] = val1;
	avg_val1 = filter_buffer_average();
	filterPointer = (++filterPointer) & filterMask;
	filterBuffer[filterPointer] = val2;
	avg_val2 = filter_buffer_average();
	return (avg_val1 << 16) +  avg_val2;	
}

int AverageFilter::filter_buffer_average()
{
	int sum = 0;
	for(int i=0; i< filterLength; i++)
	{
		sum += filterBuffer[i];
	}
	return sum /filterLength;
}
