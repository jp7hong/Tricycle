///
/// @file		TestTricycle.h
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Sep. 2, 2016
/// @version	1.0
///
/// @brief		Test class to test Tricycle class
///

#ifndef _TEST_TRICYCLE_H_
#define _TEST_TRICYCLE_H_

#include <fstream>			// std::ofstream
#include <string>			// std::string
#include <vector>			// std::vector
#include <cstdio>			// FILE

#include "Singleton.h"		// TSingleton
#include "Pose.h"			// SPos, SPose

#if defined(WIN32)
#	include "pGNUPlot.h"	// CpGnuplot
#endif

/// @brief		Test class to test Tricycle class
class CTestTricycle : public TSingleton<CTestTricycle>
{
public:
	typedef struct _tagSRecord
	{
		float time;
		float steering_angle;
		int   encoder_ticks;
		float angular_velocity;

		/// default constructor
		explicit _tagSRecord()
		: time(0.f)
		, steering_angle(0.f)
		, encoder_ticks(0) {}
	} SRecord;

public:
	/// constructor
	explicit CTestTricycle();

	/// destructor
	virtual ~CTestTricycle();

	/// perform test case
	int Run(const int nTestCase);

private:
	/// set input, pose, contour filename
	int SetFilename(const int nTestCase);

	/// read test case file
	int ReadInputFile();

	/// create result files
	int CreateResultFiles();

	/// close result files
	int CloseResultFiles();

	/// write pose information to the files (pose, contour)
	int Write(const float time, const SPose pose);

	/// draw a plot to see the result
	void DrawGnuplot(const bool bSetRange = false, \
		const float x_min = 0.f, const float x_max = 0.f, \
		const float y_min = 0.f, const float y_max = 0.f);

private:
	/// non construction-copyable
	CTestTricycle(const CTestTricycle&);

	/// non copyable
	const CTestTricycle& operator=(const CTestTricycle&);

private:
	/// test case number
	int m_nTestCase;

	/// filename for reading input data
	std::string m_sFilenameInput;

	/// filename for writing pose data
	std::string m_sFilenamePose;

	/// filename for writing contour data
	std::string m_sFilenameContour;

	/// file stream to save poses of robot center (trajectory)
	std::ofstream m_fsFilePose;

	/// file stream to save robot polygon shapes of the robot
	std::ofstream m_fsFileContour;

	/// vector for records of input file
	std::vector<SRecord> m_vRecord;

#if defined(WIN32)
	/// CpGnuplot instance pointer
	CpGnuplot* m_pGnuPlot;
#else // defined(WIN32)
	FILE *m_fpPipe;
#endif
};

#endif // _TEST_TRICYCLE_H_
