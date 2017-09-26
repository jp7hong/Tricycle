///
/// @file		VirtualGyro.cpp
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Sep. 2, 2016
/// @version	1.0
///
/// @brief		Virtual gyro class for simulation
///
/// @remark		INACCURATE because of calculating with 'steering_angle' value.
///

#include <cmath>		// sqrtf, logf, sinf
#include <cstdlib>		// RAND_MAX

#include "VirtualGyro.h"
#include "Tricycle.h"	// CTriCycle

//==============================================================================
//
// Modeling concept for VirtualGyro
// --------------------------------
//
// Basically, the VirtualGyro angle is made with 'steering_angle' variable.
//
// Three data (encoder_ticks, angular_velocity, steering_angle) are obtained at
// each sampling time. In the cases of 'encoder_ticks' and 'angular_velocity',
// the values are obtained from sensors just before sampling time.
// But, in case of steering_angle, we don't know exact time when it was changed.
//
// Let's suppose a sampling case such as following diagram.
//
//     When did 'steer_angle' change?
//
//          (1)     (2)     (3)
//           |       |       |
//           V       V       V
// ---------+-----------------+--------->
//        t_s(n)           t_s(n+1)
//
// In the case of (1), it affects the change of the robot pose at sampling time
// t_s(n+1), but in case of (3), it has little effect on the change of the
// robot pose.
//
// Therefore, I've implemented to make VirtualGyro angle value with the average
// value of 'steering_angle' at t_s(n) and t_s(n+1).
//
//==============================================================================

///
/// @brief		update angle and angular velocity of the gyro
/// @param		fTime [in] current time (sec)
/// @param		fSteerRad [in] steering angle of the front wheel
/// @return		void
/// @remark		must be called at the beginning in the estimate()
///
void CVirtualGyro::Update(const float fTime, const float fSteerRad, const int nEncoderTicks)
{
	static float fPrevTime = 0.f;		///< previous timestamp
	static float fPrevSteerRad = 0.f;	///< previous steering angle (rad)
	float  fDiffTime = 0.f;				///< difference since previous time (s)

	/// difference since previous time (s)
	fDiffTime = fTime - fPrevTime;

	/// make the gyro angle (rad)
	float fDiffAngleRad = (nEncoderTicks * CTricycle::GetInstance()->GetFrontDistPerTick()) / 2.f;
	fDiffAngleRad /= CTricycle::GetInstance()->GetDistBtwFrontRear();
	fDiffAngleRad *= sinf((fPrevSteerRad + fSteerRad) / 2.f);

#if (APPLY_NOISE)
#	error Not implemented.
	// gaussian noise...
#endif // (APPLY_NOISE)

#if (APPLY_DRIFT)
#	error Not implemented.
	// unidirectional drift...
#endif // (APPLY_DRIFT)

	/// update the angular velocity of the gyro
	//@{
	m_fAngVel = AngleDiff<float>(m_fAngleRad, m_fAngleRad + fDiffAngleRad);
	m_fAngVel = AngleClamp(m_fAngVel);
	if (!almostZero<float>(fDiffTime))	///< prevent divide by zero
		m_fAngVel /= fDiffTime;			///< angular velocity (rad/s)
	//@}

	/// copy to the member variable 'm_fAngleRad'
	m_fAngleRad = m_fAngleRad + fDiffAngleRad;

	/// clamp gyro angle between [-M_PI..+M_PI)
	m_fAngleRad = AngleClamp(m_fAngleRad);

	/// update fPrevTime for the next time
	fPrevTime = fTime;

	/// update fPrevSteerRad for the next time
	fPrevSteerRad = fSteerRad;
}
