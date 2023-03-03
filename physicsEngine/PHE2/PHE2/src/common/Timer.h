#pragma once

/// Timer for profiling. This has platform specific code and may
/// not work on every platform.
class Timer
{
public:
	/// Constructor
	Timer();

	/// Reset the timer.
	void Reset();

	/// Get the time since construction or the last reset.
	float GetMilliseconds() const;

private:
	double m_start;
	static double s_invFrequency;

};

