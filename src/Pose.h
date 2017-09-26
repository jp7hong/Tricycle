///
/// @file		Pose.h
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Aug 29, 2016
/// @version	1.0
///
/// @brief		classes for 2D position and robot pose (x, y, theta)
///

#ifndef _POSE_H_
#define _POSE_H_

/// type definition to represent the 2D position
typedef struct _tagSPos
{
	float x;	///< position x (unit: m)
	float y;	///< position y (unit: m)

	/// default constructor
	_tagSPos() : x(0.f), y(0.f) {}

	/// constructor
	_tagSPos(const float fX, const float fY)
		: x(fX), y(fY) {}
} SPos;

/// type definition to represent the 2D pose of the robot
typedef struct _tagSPose
{
	float x;	///< position x (unit: m)
	float y;	///< position y (unit: m)
	float q;	///< heading angle (unit: rad)

	/// default constructor
	_tagSPose() : x(0.f), y(0.f), q(0.f) {}

	/// constructor
	_tagSPose(const float fX, const float fY, const float fQ)
	: x(fX), y(fY), q(fQ) {}
} SPose;

#endif // _POSE_H_
