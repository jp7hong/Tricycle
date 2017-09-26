///
/// @file		math2.h
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Sep 1, 2016
/// @version	0.1
///
/// @brief		Miscellaneous mathematical definitions and functions
///

#ifndef _MATH2_H_
#define _MATH2_H_

#include <cassert>		// assert
#include <typeinfo>		// typeid
#include <cmath>		// fabs, pow, log sqrt
#include <limits>		// numeric_limits
#include <cstring>		// memcpy
#include <cstdlib>		// srand
#include <ctime>		// time

/// PI
#ifndef M_PI
#	define M_PI	(3.1415926535897932384626433832795f)
#endif

/// convert radians into degrees
#ifndef RAD2DEG
#	define RAD2DEG(r)	(((r) * 180.0f) / M_PI)
//#	define RAD2DEG(r)	((r) * 57.295779513082320876798154814105f)
#endif

/// convert degrees into radians
#ifndef DEG2RAD
#	define DEG2RAD(d)	(((d) * M_PI) / 180.0f)
//#	define DEG2RAD(d)	((d) * 0.01745329251994329576923690768489f)
#endif

///
/// @brief		clamp the angle (rad) between [-M_PI..+M_PI) range
/// @param		rad [in] angle (rad) to clamp
/// @return		clamped angle (rad)
///
template<typename T> inline
T AngleClamp(const T angleRad)
{
	T clapmedRad = angleRad;

	while (clapmedRad >= +M_PI)
		clapmedRad -= (M_PI + M_PI);
	while (clapmedRad < -M_PI)
		clapmedRad += (M_PI + M_PI);

	return clapmedRad;
}

///
/// @brief		get the angle difference (rad) [-M_PI..+M_PI)
///				(shortest way, keep sign)
/// @param		prevRad [in] previous angle
/// @param		currRad [in] current angle
/// @return		angle difference (rad)
///
template<typename T> inline
T AngleDiff(const T startRad, const T endRad)
{
	T diffRad = endRad - startRad;

	while (diffRad > +M_PI)
		diffRad -= (M_PI + M_PI);
	while (diffRad < -M_PI)
		diffRad += (M_PI + M_PI);

	return diffRad;
}

///
/// @brief		compare two float/double
/// @param		v1 [in] one of two variables to compare
/// @param		v2 [in] the other of two variables to compare
/// @return		true: same, false: different
///
template<typename T> inline
bool almostEqual(const T v1, const T v2)
{
	assert((typeid(v1) == typeid(float)) || (typeid(v1) == typeid(double)));

	return (fabs(v1 - v2) < std::numeric_limits<T>::epsilon());
}

///
/// @brief		check whether a given variable is almost zero
/// @param		v [in] variable to check
/// @return		true: almost zero, false: not almost zero
///
template<typename T> inline
bool almostZero(const T v)
{
	assert((typeid(v) == typeid(float)) || (typeid(v) == typeid(double)));

	return (v < std::numeric_limits<T>::epsilon());
}

///
/// @brief		check whether a given value is exact zero
/// @param		v [in] variable to check
/// @return		true: zero, false: not zero
///
template<typename T>
inline
bool isRealZero(const T v)
{
	assert((typeid(v) == typeid(float)) || (typeid(v) == typeid(double)));

	T i = 0.f;

	return (!memcmp(&i, &v, sizeof(v)));
}

///
/// @brief		produce a uniform random float between 0..1
/// @param		N/A
/// @return		uniform random float between 0..1
/// @remark		https://stackoverflow.com/questions/686353/c-random-float-number-generation
///
template<typename T> inline
T rand_uniform_between_0_and_1()
{
	static bool bFirst = true;

	/// prevent not to produce same sequenced values
	if (bFirst)
	{
		bFirst = false;
		srand(static_cast<unsigned>(time(0)));
	}

	return static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
}

///
/// @brief		produce a uniform random float between 0..v
/// @param		f [in] upper bound
/// @return		uniform random float between 0..v
/// @remark		https://stackoverflow.com/questions/686353/c-random-float-number-generation
///
template<typename T> inline
T rand_uniform_between_0_and_f(const T v)
{
	static bool bFirst = true;

	assert((typeid(v) == typeid(float)) || (typeid(v) == typeid(double)));

	/// prevent not to produce same sequenced values
	if (bFirst)
	{
		bFirst = false;
		srand(static_cast<unsigned>(time(0)));
	}

	return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / v));
}

///
/// @brief		produce a uniform random float between lo..hi
/// @param		lo [in] lower bound
/// @param		hi [in] upper bound
/// @return		uniform random float between lo..hi
/// @remark		https://stackoverflow.com/questions/686353/c-random-float-number-generation
///
template<typename T> inline
T rand_uniform_between_lo_and_hi(const T lo, const T hi)
{
	static bool bFirst = true;

	assert((typeid(lo) == typeid(float)) || (typeid(lo) == typeid(double)));
	assert((typeid(hi) == typeid(float)) || (typeid(hi) == typeid(double)));

	/// prevent not to produce same sequenced values
	if (bFirst)
	{
		srand(static_cast<unsigned>(time(0)));
		bFirst = false;
	}

	return lo + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX / (hi - lo)));
}

///
/// @brief		produce a gaussian random float (Box-Muller method)
/// @param		mean [in] mean value
/// @param		stdev [in] standard deviation value
/// @return		gaussian random float
/// @remark		https://stackoverflow.com/questions/19944111/creating-a-gaussian-random-generator-with-a-mean-and-standard-deviation
///
template<typename T> inline
T rand_gaussian(const T mean, const T stdev)
{
	static T n2 = 0.f;
	static int n2_cached = 0;
	static bool bFirst = true;

	assert((typeid(mean) == typeid(float)) || (typeid(mean) == typeid(double)));
	assert((typeid(stdev) == typeid(float))|| (typeid(mean) == typeid(double)));

	/// prevent not to produce same sequenced values
	if (bFirst)
	{
		bFirst = false;
		srand(static_cast<unsigned>(time(0)));
	}

	if (!n2_cached)
	{
		T x = 0.f, y = 0.f, r = 0.f;
		do
		{
			x = 2.f * rand() / RAND_MAX - 1;
			y = 2.f * rand() / RAND_MAX - 1;
			r = x * x + y * y;
		}

		while (r == 0.f || r > 1.f);
		{
			T d = 0.f;

			if (typeid(mean) == typeid(float))
				d = sqrtf(-2.f * logf(r) / r);
			else // double
				d = sqrt(-2.f * log(r) / r);

			T n1 = x * d;
			n2 = y * d;
			T result = n1 * stdev + mean;
			n2_cached = 1;

			return result;
		}
	}
	else
	{
		n2_cached = 0;
		return n2 * stdev + mean;
	}
}

#endif // _MATH2_H_
