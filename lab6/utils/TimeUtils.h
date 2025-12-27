#pragma once
#include <sys/time.h>

class TimeUtils
{
public:
	static long long GetCurrentTimeMillis()
	{
		struct timeval tv{};
		gettimeofday(&tv, nullptr);
		return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}

	static double GetTimeInSeconds()
	{
		struct timeval tv{};
		gettimeofday(&tv, nullptr);
		return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
	}
};