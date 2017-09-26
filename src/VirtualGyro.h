///
/// @file		VirtualGyro.h
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Sep. 2, 2016
/// @version	1.0
///
/// @brief		Virtual gyro class for simulation
///
/// @remark		INACCURATE because of calculating with 'steering_angle' value.
///

#ifndef _VIRTUAL_GYRO_H_
#define _VIRTUAL_GYRO_H_

#include "Singleton.h"		// TSingleton
#include "math2.h"			// DEG2RAD

/// whether to apply gaussian noise (CHAGEABLE!)
/// 0: do not apply gaussian noise (update with steering angle difference only)
/// 1: apply gaussian noise (update with steering angle difference and gaussian noise)
#define APPLY_NOISE		(0)

/// standard deviation of the gaussian noise
#define NOISE_STDEV		(DEG2RAD(0.1f))

/// whether to apply angle drift (CHANGEABLE!)
/// 0: do not apply drift
/// 1: apply unidirectional drift
#define APPLY_DRIFT		(0)

/// angle drift direction (CHAGEABLE!)
/// 0: drift direction = CCW
/// 1: drift direction = CW
#define DRIFT_DIR		(0)

/// angle error per minute (CHAGEABLE!)
#define DRIFT_RAD_PER_MINUTE	(DEG2RAD(0.3f))

/// angle error per second (DO NOT CHANGE!)
#define DRIFT_RAD_PER_SECOND	(GYRO_ERR_PER_MINUTE / 60.f)

/// @brief		Virtual gyro class for simulation
class CVirtualGyro : public TSingleton<CVirtualGyro>
{
public:
	explicit CVirtualGyro() : m_fAngVel(0.f), m_fAngleRad(0.f) {}
	virtual ~CVirtualGyro() {}

	/// update angle and angular velocity of the gyro
	void Update(const float fTime, const float fSteerRad, const int nEncoderTicks);

	/// get the angular velocity (rad/s)
	float GetAngVel() { return m_fAngVel; }

	/// get the gyro angle (rad)
	//float GetAngleRad() { return m_fAngleRad; }

private:
	/// non construction-copyable
	CVirtualGyro(const CVirtualGyro&);

	/// non copyable
	const CVirtualGyro& operator=(const CVirtualGyro&);

private:
	/// angular velocity of the gyro (rad/s)
	float m_fAngVel;

	/// gyro angle (rad)
	float m_fAngleRad;
};

#endif // _VIRTUAL_GYRO_H_
