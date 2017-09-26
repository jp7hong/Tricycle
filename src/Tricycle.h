///
/// @file		Tricycle.h
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Sep. 2, 2016
/// @version	1.0
///
/// @brief		Calculates odometry for the Tricycle-drive
///

#ifndef _TRICYCLE_H_
#define _TRICYCLE_H_

#include <iostream>		// std::cout
#include <fstream>		// std::ofstream
#include <cmath>		// sinf, cosf
#include <cstdio>		// _popen, _pclose, fprintf

#if defined(WIN32)
#	include <conio.h>
#	include "pGNUPlot.h"
#endif

#include "Singleton.h"	// TSingleton
#include "Pose.h"		// SPos, SPose
#include "math2.h"		// M_PI

/// PLATFORM DEPENDENT VARIABLES
//@{
/// front wheel radius (unit: m)
#define FRONT_WHEEL_RADIUS		(0.2f)

/// rear wheel radius (unit: m) - not used
#define REAR_WHEEL_RADIUS		(0.2f)

/// distance from front wheel to back axis (r) (unit: m)
#define DIST_BTW_FRONT_REAR		(1.f)

/// distance between rear wheel (d) (unit: meter) - used for drawing
#define DIST_BTW_REAR_WHEELS	(0.75f)

/// number of ticks per revolution of the front wheel
#define TICKS_PER_REVOLUTION	(512)
//@}

/// @brief		Pose estimator class for the Tricycle mobile robot
class CTricycle : public TSingleton<CTricycle>
{
public:
	/// default constructor
	explicit CTricycle()
	: m_fFrontWheelRadius(FRONT_WHEEL_RADIUS)
	, m_fDistBtwFrontRear(DIST_BTW_FRONT_REAR)
	, m_fDistBtwRearWheels(DIST_BTW_REAR_WHEELS)
	, m_nTicksPerRevolution(TICKS_PER_REVOLUTION)
	, m_fFrontWheelCircum(2.f * M_PI * m_fFrontWheelRadius)
	, m_fFrontDistPerTick(m_fFrontWheelCircum / m_nTicksPerRevolution) {}

	/// default destructor
	virtual ~CTricycle() {}

	/// convert front wheel distance to the number of encoder ticks
	static int Dist2Ticks(const float fDist, const float fTimeGap = 1.f)
	{
		return int(fDist * TICKS_PER_REVOLUTION);
	}

	/// get the distance from front wheel to back axis (m)
	float GetDistBtwFrontRear() { return m_fDistBtwFrontRear; }

	/// get the distance per a tick of the front wheel (m/tick)
	float GetFrontDistPerTick() { return m_fFrontDistPerTick; }

	/// get the robot pose
	void GetRobotPose(SPose& pose) { pose = m_pose; }

	/// get the contour of the front wheel and rear wheels
	void GetRobotContour(SPos& posFW, SPos& posLW, SPos& posRW);

	/// pose estimator
	SPose Estimate(const float time, const float steering_angle, \
		const int encoder_ticks, float angular_velocity);

private:
	/// non construction-copyable
	CTricycle(const CTricycle&);

	/// non copyable
	const CTricycle& operator=(const CTricycle&);

private:
	/// current robot pose
	SPose m_pose;

	/// front wheel radius (m)
	const float m_fFrontWheelRadius;

	/// distance from front wheel to back axis (m)
	const float m_fDistBtwFrontRear;

	/// distance between rear wheel (d) (unit: meter)
	const float m_fDistBtwRearWheels;

	/// number of ticks per revolution of the front wheel
	const int   m_nTicksPerRevolution;

	/// circumference of the front steering wheel (m)
	const float m_fFrontWheelCircum;

	/// distance per a tick of the front wheel
	const float m_fFrontDistPerTick;
};

/// Pose estimator interface function for the Tricycle mobile robot
/// (extern function to satisfy requirements of the question - NOT USED)
SPose estimate(float time, float steering_angle, int encoder_ticks, \
	float angular_velocity);

#endif // _TRICYCLE_H_
