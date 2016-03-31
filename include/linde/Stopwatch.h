#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "linde.h"

#include <chrono>
#include <iostream>
/**
* @author Thomas Lindemeier
*
* University of Konstanz-
* Department for Computergraphics
*/
namespace linde
{
	/*
	#######################################################################
	#######################################################################
	#######################################################################
	#######################################################################
	#################### Stopwatch ########################################
	#######################################################################
	#######################################################################
	#######################################################################
	#######################################################################
	#######################################################################
	*/
	class Stopwatch
	{
		std::chrono::high_resolution_clock::time_point start;

	public:
		Stopwatch();

		~Stopwatch();

		std::chrono::high_resolution_clock::time_point restart();

		std::chrono::microseconds elapsedMicroseconds() const;

		std::chrono::milliseconds elapsedMilliseconds() const;

		std::chrono::seconds elapsedSeconds() const;

		std::chrono::minutes elapsedMinutes() const;

		static 
			std::chrono::high_resolution_clock::time_point now();

		static 
			long long currentMicroSeconds();
		static 
			long long currentMilliSeconds();
		static 
			long long currentSeconds();
		static 
			long long currentMinutes();

		std::ostream & operator<< (std::ostream & output) const;
		

	private:
		std::chrono::microseconds intervalMicroseconds(
			const std::chrono::high_resolution_clock::time_point & t1, 
			const std::chrono::high_resolution_clock::time_point & t0) const;

		std::chrono::milliseconds intervalMilliseconds(
			const std::chrono::high_resolution_clock::time_point & t1,
			const std::chrono::high_resolution_clock::time_point & t0) const;

		std::chrono::seconds intervalSeconds(
			const std::chrono::high_resolution_clock::time_point & t1,
			const std::chrono::high_resolution_clock::time_point & t0) const;

		std::chrono::minutes intervalMinutes(
			const std::chrono::high_resolution_clock::time_point & t1,
			const std::chrono::high_resolution_clock::time_point & t0) const;
	};

	std::ostream & operator<< (std::ostream & output, const Stopwatch & sw);

} // namespace my

#endif // STOPWATCH_H
