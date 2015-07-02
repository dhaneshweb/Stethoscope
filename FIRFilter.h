#ifndef __FIRFILTER_H__
#define __FIRFILTER_H__

#include "IFilter.h"

class FIRFilter : public IFilter
{
public:
	FIRFilter(int totalDecay);
	
	virtual int push_filter_buffer(int val);
private:
	int filter_buffer_average();
	int filterPointer;
	int decay;

	static const int filterLength = 64;
	static const int filterMask = filterLength - 1;
	static const int parameters[filterLength];
	int filterBuffer[128];	
};

#endif
