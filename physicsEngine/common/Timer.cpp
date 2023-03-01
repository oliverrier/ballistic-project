#include "Timer.h"

double Timer::s_invFrequency = 0.0;

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

Timer::Timer()
{
	LARGE_INTEGER largeInteger;

	if (s_invFrequency == 0.0)
	{
		QueryPerformanceFrequency(&largeInteger);
		s_invFrequency = double(largeInteger.QuadPart);
		if (s_invFrequency > 0.0)
		{
			s_invFrequency = 1000.0 / s_invFrequency;
		}
	}

	QueryPerformanceCounter(&largeInteger);
	m_start = double(largeInteger.QuadPart);
}

void Timer::Reset()
{
	LARGE_INTEGER largeInteger;
	QueryPerformanceCounter(&largeInteger);
	m_start = double(largeInteger.QuadPart);
}

float Timer::GetMilliseconds() const
{
	LARGE_INTEGER largeInteger;
	QueryPerformanceCounter(&largeInteger);
	double count = double(largeInteger.QuadPart);
	float ms = float(s_invFrequency * (count - m_start));
	return ms;
}
