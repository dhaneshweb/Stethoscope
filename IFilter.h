#ifndef __IFILTER_H__
#define __IFILTER_H__

class IFilter
{
public:
	virtual int push_filter_buffer(int val) = 0;
	virtual ~IFilter() {}
};

#endif
