///
/// @file		Tricycle.cpp
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Sep. 2, 2016
/// @version	1.0
///
/// @brief		Calculates odometry for the Tricycle-drive
///

#include "Tricycle.h"
#include "VirtualGyro.h"	// CVirtualGyro

///
/// @brief		get positions of the front wheel and rear wheels
///
/// @param		posFW [out] position of the front wheel
/// @param		posLW [out] position of the left wheel
/// @param		posRW [out] position of the right wheel
///
/// @return		void
///
void CTricycle::GetRobotContour(SPos& posFW, SPos& posLW, SPos& posRW)
{
	/// distance between a rear wheel and robot center
	float fDistRearWheelFromCenter = m_fDistBtwRearWheels / 2.f;

	/// angle to calculate wheel position
	float fAngle = DEG2RAD(90.f) - m_pose.q;

	posFW.x = m_pose.x + m_fDistBtwFrontRear * cosf(m_pose.q);
	posFW.y = m_pose.y + m_fDistBtwFrontRear * sinf(m_pose.q);
	posLW.x = m_pose.x - fDistRearWheelFromCenter * cosf(fAngle);
	posLW.y = m_pose.y + fDistRearWheelFromCenter * sinf(fAngle);
	posRW.x = m_pose.x + fDistRearWheelFromCenter * cosf(fAngle);
	posRW.y = m_pose.y - fDistRearWheelFromCenter * sinf(fAngle);
}

///
/// @brief		pose estimator interface member function
///
/// @param		time [in] time of reading of the input data (unit: sec)
/// @param		steering_angle [in] steering wheel angle (unit: rad)
/// @param		encoder_ticks [in] number of ticks from the traction motor
///				encoder (unit: ticks (integer))
/// @param		angular_velocity [in] reading from a gyroscope measuring the
///				rotation velocity of the platform around the Z axis
///				(unit: rad/s)
///
/// @return		new estimated pose. Tuple (x, y, heading) representing the
///				estimated pose of the platform (unit: m, m, rad)
///
SPose CTricycle::Estimate(float time, float steering_angle, int encoder_ticks, \
	float angular_velocity)
{
	// Front wheel radius = 0.2 m
	// Back wheels radius = 0.2 m
	// Distance from front wheel to back axis (r) = 1m
	// Distance between rear wheels (d) = 0.75m
	// Front wheel encoder = 512 ticks per revolution

	// circumference of a wheel:
	//     2 * M_PI * 0.2 = 0.4 * M_PI = 1.2566370614359172953850573533118f (m)
	// distance per tick:
	//     (0.4 * M_PI) / 512 = 0.00245436926061702596754894014319 (m/pulse)

	/// previous timestamp (sec)
	static float fPrevTime = 0.f;

	/// time difference since previous time
	float fDiffTime = time - fPrevTime;

	/// distance of the front steering wheel
	float fFrontWheelDist = encoder_ticks * m_fFrontDistPerTick;

	/// front wheel velocity (m/s)
	//@{
	float fFrontWheelVel = 0.f;
	if (!almostZero(fDiffTime))	///< prevent divide by zero
		fFrontWheelVel = fFrontWheelDist / fDiffTime;
	//@}

	/// angle difference (rad)
	/*float fDiffQ = (fFrontWheelVel / m_fDistBtwFrontRear) \
		* sinf(steering_angle);*/

	/// get the angular velocity from gyro (rad/s)
	float fW = CVirtualGyro::GetInstance()->GetAngVel();

	/// consider time difference
	m_pose.q += fW * fDiffTime;

	/// clamp angle
	m_pose.q = AngleClamp(m_pose.q);

	/// differences of robot position (x, y)
	float fDiffX = (fFrontWheelVel * fDiffTime) * cosf(steering_angle) \
		* cosf(m_pose.q);
	float fDiffY = (fFrontWheelVel * fDiffTime) * cosf(steering_angle) \
		* sinf(m_pose.q);

	/// update the robot pose
	m_pose.x += fDiffX;
	m_pose.y += fDiffY;

	/// update timestamp for the next time
	fPrevTime = time;

	/// return robot pose (x, y, heading)
	return m_pose;
}

///
/// @brief		Pose estimator interface function for the Tricycle mobile robot
///
/// @param		time [in] time of reading of the input data (unit: sec)
/// @param		steering_angle [in] steering wheel angle (unit: rad)
/// @param		encoder_ticks [in] number of ticks from the traction motor
///				encoder (unit: ticks (integer))
/// @param		angular_velocity [in] reading from a gyroscope measuring the
///				rotation velocity of the platform around the Z axis
///				(unit: rad/s)
///
/// @return		new estimated pose. Tuple (x, y, heading) representing the
///				estimated pose of the platform (unit: m, m, rad)
///
SPose estimate(float time, float steering_angle, int encoder_ticks, \
	float angular_velocity)
{
	/// return calculated odometry pose (x, y, heading)
	return CTricycle::GetInstance()->Estimate(time, steering_angle, \
		encoder_ticks, angular_velocity);
}
