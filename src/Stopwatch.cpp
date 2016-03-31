#include "../include/linde/Stopwatch.h"

/**
* @author Thomas Lindemeier
*
* University of Konstanz-
* Department for Computergraphics
*/

namespace linde
{
	Stopwatch::Stopwatch() : start(now())
	{

	}

	Stopwatch::~Stopwatch()
	{
	}

	std::chrono::high_resolution_clock::time_point Stopwatch::restart() 
	{
		start = now();
		return start;
	}

	std::chrono::microseconds Stopwatch::elapsedMicroseconds() const
	{
		return intervalMicroseconds(now(), start);
	}
	std::chrono::milliseconds Stopwatch::elapsedMilliseconds() const
	{ 
		return intervalMilliseconds(now(), start);
	}
	std::chrono::seconds Stopwatch::elapsedSeconds() const
	{ 
		return intervalSeconds(now(), start);
	}
	std::chrono::minutes Stopwatch::elapsedMinutes() const
	{ 
		return intervalMinutes(now(), start);
	}

	std::chrono::high_resolution_clock::time_point Stopwatch::now()
	{
		return std::chrono::high_resolution_clock::now();
	}

	long long Stopwatch::currentMilliSeconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(now().time_since_epoch()).count();
	}

	long long Stopwatch::currentSeconds()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(now().time_since_epoch()).count();
	}

	long long Stopwatch::currentMicroSeconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(now().time_since_epoch()).count();
	}

	long long Stopwatch::currentMinutes()
	{
		return std::chrono::duration_cast<std::chrono::minutes>(now().time_since_epoch()).count();
	}

	std::ostream & operator<< (std::ostream & output, const Stopwatch & sw)
	{
		output << "computed in:\t" << sw.elapsedMilliseconds().count() << "msecs" << std::endl; 
		return output;
	}

	std::chrono::microseconds Stopwatch::intervalMicroseconds(
		const std::chrono::high_resolution_clock::time_point & t1, 
		const std::chrono::high_resolution_clock::time_point & t0) const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
	}

	std::chrono::milliseconds Stopwatch::intervalMilliseconds(
		const std::chrono::high_resolution_clock::time_point & t1,
		const std::chrono::high_resolution_clock::time_point & t0) const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
	}

	std::chrono::seconds Stopwatch::intervalSeconds(
		const std::chrono::high_resolution_clock::time_point & t1,
		const std::chrono::high_resolution_clock::time_point & t0) const
	{
		return std::chrono::duration_cast<std::chrono::seconds>(t1 - t0);
	}

	std::chrono::minutes Stopwatch::intervalMinutes(
		const std::chrono::high_resolution_clock::time_point & t1,
		const std::chrono::high_resolution_clock::time_point & t0) const
	{
		return std::chrono::duration_cast<std::chrono::minutes>(t1 - t0);
	}


} // namespace my
