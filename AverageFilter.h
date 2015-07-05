#ifndef __AVERAGEFILTER_H__	
#define __AVERAGEFILTER_H__

#include "IFilter.h"

class AverageFilter : public IFilter
{
public:
	AverageFilter();
	
	virtual int push_filter_buffer(int val);
private:
	int filter_buffer_average();

	static const int filterLength = 8;
	static const int filterMask = filterLength - 1;
	int filterPointer;
	short filterBuffer[filterLength];
};

#endif
