#ifndef __AVERAGEFILTER_H__	
#define __AVERAGEFILTER_H__

class AverageFilter
{
public:
	AverageFilter();
	
	int push_filter_buffer(int val);
private:
	int filter_buffer_average();

	static const int filterLength = 16;
	static const int filterMask = filterLength - 1;
	int filterPointer;
	short filterBuffer[filterLength];
};

#endif
