///
/// @file		TestTricycle.h
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Sep. 2, 2016
/// @version	1.0
///
/// @brief		Test class for Tricycle class
///

#include <iostream>			// std::cout
#include <sstream>			// std::stringstream, std::istringstream
#include <fstream>			// std::fstream
#include <iomanip>			// std::setw, std::fill
#include <cstdlib>			// atof, atoi
#include <cstdio>			// popen, fprintf

#if defined(WIN32)
#	include <conio.h>		// getch
#else
#	include <termios.h>		// termios, tcgetattr
#	include <unistd.h>		// readlink
#endif

#include "TestTricycle.h"
#include "Tricycle.h"		// CTricycle
#include "VirtualGyro.h"	// CVirtualGyro

#if defined(__linux__)
///
/// @brief		getch() for Linux
/// @param		N/A
/// @return		N/A
///
int getch()
{
	int ch = 0;
	struct termios old, _new;

	tcgetattr(0, &old);

	_new = old;
	_new.c_lflag &= ~(ICANON|ECHO);
	_new.c_cc[VMIN] = 1;
	_new.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &_new);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &old);

	return ch;
}
#endif // defined(__linux__)

///
/// @brief		constructor
/// @param		N/A
/// @return		N/A
///
CTestTricycle::CTestTricycle()
: m_nTestCase(0)
#if defined(WIN32)
, m_pGnuPlot(0)
#else
, m_fpPipe(0)
#endif
{
}

///
/// @brief		constructor
/// @param		N/A
/// @return		N/A
///
CTestTricycle::~CTestTricycle()
{
}

///
/// @brief		run a test case
/// @param		nTestCase [in] test case number
/// @return		0 on success, < 0 if occurred error
///
int CTestTricycle::Run(const int nTestCase)
{
	/// robot pose (x, y, heading)
	SPose pose;

	/// set filenames for input, pose, and contour
	SetFilename(nTestCase);

	/// read the input file
	if (ReadInputFile() != 0)
	{
		std::cout << "Error occurred in ReadInputFile()." << std::endl;
		return -1;
	}

	/// create result files (pose, contour)
	CreateResultFiles();

	/// write initial pose to output files
	//@{
	CTricycle::GetInstance()->GetRobotPose(pose);
	Write(0.f, pose);
	//@}

	/// calculate odometry for each record
	//@{
	for (std::vector<SRecord>::iterator it = m_vRecord.begin(); \
		it != m_vRecord.end(); ++it)
	{
		/*
		std::cout << "time: ";
		std::cout.setf(std::ios::fixed);
		std::cout.precision(3);
		std::cout << it->time << ", ";
		std::cout.unsetf(std::ios::fixed);

		std::cout << "steering_angle: ";
		std::cout.setf(std::ios::fixed);
		std::cout.precision(3);
		std::cout << it->steering_angle << ", ";
		std::cout.unsetf(std::ios::fixed);

		std::cout << "encoder_ticks: ";
		std::cout << std::setfill('0') << std::setw(3);
		std::cout << it->encoder_ticks << ", ";

		std::cout << "angular_velocity: ";
		std::cout.setf(std::ios::fixed);
		std::cout.precision(3);
		std::cout << it->angular_velocity << std::endl;
		std::cout.unsetf(std::ios::fixed);
		*/

		/// update virtual gyro
		CVirtualGyro::GetInstance()->Update(it->time, it->steering_angle, it->encoder_ticks);

		/// calculate robot pose
		pose = estimate( \
			it->time, \
			it->steering_angle, \
			it->encoder_ticks, \
			CVirtualGyro::GetInstance()->GetAngVel());

		/// write a robot pose to the output files (pose, contour)
		Write(it->time, pose);
	}
	//@}

	/// close result files (pose, contour)
	CloseResultFiles();

	/// draw a result plot
	DrawGnuplot();

	/// wait for user's key press
	//@{
	std::cout << "Press any key in this window to quit." << std::endl;
	getch();
	//@}

#if defined(WIN32)
	if (m_pGnuPlot)
		delete m_pGnuPlot;	///< delete CpGnuplot instance
#else // defined(WIN32)
#endif // defined(WIN32)

	return 0;
}

///
/// @brief		set input, pose, contour filename
/// @param		N/A
/// @return		0 on success, -1 if occurred error
///
int CTestTricycle::SetFilename(const int nTestCase)
{
	/// temporary string
	std::string str;

	/// stringstream for the formatted string
	std::stringstream ss;

	/// get the executable file path and filename
	//@{
	const int nPathBufSize = 1024;
	char exePath[nPathBufSize] = { 0, };
#if defined(WIN32)
	//::GetCurrentDirectory(nMaxPath, exePath, nPathBufSize);
	int rc = ::GetModuleFileName(NULL, exePath, nPathBufSize - 1);
	if (!rc) return -1;			///< return if failure
#else
	int rc = readlink("/proc/self/exe", exePath, nPathBufSize - 1);
	if (rc == -1) return -1;	///< return if failure
#endif // defined(WIN32)
	//@}

	/// get the path only. E.g., "D:\\home" on Windows, "/home" on Linux
	/// E.g., "D:\\home\\Tricycle.exe" on Windows, "/home/Tricycle" on Linux
	str = exePath;
	/// get path only. E.g., "D:\\home" on Windows, "/home" on Linux
	str = str.substr(0, str.find_last_of("\\/"));
#if defined(WIN32)
	str += "\\";	///< E.g., "D:\\home\\" on Windows
#else
	str += "/";	///< E.g., "/home/" on Linux
#endif // defined(WIN32)
	//@}

	/// copy path to filename strings
	/// E.g., "D:\\home\\" on Windows, "/home/" on Linux
	//@{
	m_sFilenameInput   = str;
	m_sFilenamePose    = str;
	m_sFilenameContour = str;
	//@}

	/// set the filename for reading input data
	//@{
	ss << std::setfill('0') << std::setw(2) << nTestCase;
	ss << "_input.csv";				///< E.g., '01_input.csv'
	m_sFilenameInput += ss.str();	///< append the filename
	//std::cout << m_sFilenameInput << std::endl;
	//@}

	/// set the filename to writing pose data
	//@{
	ss.str(std::string());			///< clear
	ss << std::setfill('0') << std::setw(2) << nTestCase;
	ss << "_pose.txt";				///< E.g., '01_pose.csv'
	m_sFilenamePose += ss.str();	///< append the filename
	//std::cout << m_sFilenamePose << std::endl;
	//@}

	/// set the filename for writing contour data
	//@{
	ss.str(std::string());			///< clear
	ss << std::setfill('0') << std::setw(2) << nTestCase;
	ss << "_contour.txt";			///< E.g., '01_contour.csv'
	m_sFilenameContour += ss.str();	///< append the filename
	//std::cout << m_sFilenameContour << std::endl;
	//@}

	return 0;
}

///
/// @brief		read the input file
/// @param		N/A
/// @return		0 on success, < 0 if occurred error
///
int CTestTricycle::ReadInputFile()
{
	///< file stream for input
	std::fstream fsFileInput;

	///< temporary struct to read a record from input file
	SRecord sRecord;

	///< string for getline
	std::string str;

	/// open input file
	fsFileInput.open(m_sFilenameInput, std::fstream::in);

	/// if file open is failed
	if (!fsFileInput.is_open())
		return -1;

	/// iterate each record of the input file
	while (std::getline(fsFileInput, str))
	{
		/// if the line is start with '#' (comment line), skip parsing
		if (str.at(0) == '#')
			continue;

		//std::cout << str << std::endl;

		/// save to istringstream to use getline()
		std::istringstream iss(str);

		/// get 'time' field
		//@{
		std::getline(iss, str, ',');
		sRecord.time = float(atof(str.c_str()));
		/*float time = atof(str.c_str());
		std::cout.setf(std::ios::fixed);
		std::cout.precision(3);
		std::cout << "time: " << time << ", ";
		std::cout.unsetf(std::ios::fixed);*/
		//@}

		/// get 'steering_angle' field
		//@{
		std::getline(iss, str, ',');
		sRecord.steering_angle = float(atof(str.c_str()));
		/*float steering_angle = atof(str.c_str());
		std::cout.setf(std::ios::fixed);
		std::cout.precision(3);
		std::cout << "steering_angle: " << steering_angle << ", ";
		std::cout.unsetf(std::ios::fixed);*/
		//@}

		/// get 'encoder_ticks' field
		//@{
		std::getline(iss, str, ',');
		sRecord.encoder_ticks = atoi(str.c_str());
		/* float encoder_ticks = atoi(str.c_str());
		std::cout << std::setfill('0') << std::setw(3);
		std::cout << "encoder_ticks: " << encoder_ticks << ", ";*/
		//@}

		/// add a record to the vector
		m_vRecord.push_back(sRecord);
	}

	return 0;
}

///
/// @brief		create result files
/// @param		N/A
/// @return		0 if no errors
///
int CTestTricycle::CreateResultFiles()
{
	/// create a file to save poses of robot center (trajectory)
	m_fsFilePose.open(m_sFilenamePose);

	/// write comment (attribute of each field)
	m_fsFilePose << "#time\t" \
		<< "robot_x\t"  << "robot_y\t"  << "robot_q" << std::endl;

	/// create a file to save polygon shapes of the robot
	m_fsFileContour.open(m_sFilenameContour);

	/// write 1st line comment
	m_fsFileContour << "#robot_x\t" << "robot_y\t" << std::endl \
		<< "#LWheel_x\t" << "LWheel_y\t" << std::endl \
		<< "#FWheel_x\t" << "FWheel_y\t" << std::endl \
		<< "#RWheel_x\t" << "RWheel_y\t" << std::endl \
		<< "#robot_x\t" << "robot_y" << std::endl << std::endl;

	// no errors
	return 0;
}

///
/// @brief		close result files
/// @param		N/A
/// @return		0 if no errors
///
int CTestTricycle::CloseResultFiles()
{
	/// close files
	m_fsFilePose.close();
	m_fsFileContour.close();

	/// no errors
	return 0;
}

///
/// @brief		write robot pose and shape information to files
///
/// @param		time [in] timestamp
/// @param		pose [in] robot pose (x, y, heading)
/// @param		y [in] y position of the robot
/// @param		q [in] heading angle of the robot
///
/// @return		0 if no errors, -1 if failed
///
int CTestTricycle::Write(const float time, const SPose pose)
{
	/// positions of front and left/right wheel
	SPos posFW, posLW, posRW;

	/// check logical errors of file stream
	if (m_fsFilePose.fail() || m_fsFileContour.fail())
		return -1;

	/// save a robot pose of robot center to 'pose.txt' file
	m_fsFilePose << std::fixed;	/// set fixed format
	m_fsFilePose << time << "\t";
	m_fsFilePose << pose.x << "\t" << pose.y << "\t" << pose.q << std::endl;

	/// get the robot contour (positions of front/left/right wheel)
	CTricycle::GetInstance()->GetRobotContour(posFW, posLW, posRW);

	/// save a robot polygon shape to 'contour.txt' file
	m_fsFileContour << std::fixed;	/// set fixed format
	m_fsFileContour << pose.x  << "\t" << pose.y  << std::endl;
	m_fsFileContour << posLW.x << "\t" << posLW.y << std::endl;
	m_fsFileContour << posFW.x << "\t" << posFW.y << std::endl;
	m_fsFileContour << posRW.x << "\t" << posRW.y << std::endl;
	m_fsFileContour << pose.x  << "\t" << pose.y  << std::endl;
	m_fsFileContour << std::endl;		/// need a blank line to seperate polygons

	// no errors
	return 0;
}

///
/// @brief		draw a plot to see the result
/// @param		N/A
/// @return		void
///
void CTestTricycle::DrawGnuplot(const bool bSetRange, \
	const float x_min, const float x_max, \
	const float y_min, const float y_max)
{
#if defined(WIN32)

	/// create CpGnuplot instance
	m_pGnuPlot = new CpGnuplot("..\\gnuplot\\wgnuplot.exe");

	m_pGnuPlot->cmd("set size ratio -1");	///< set same ratio in all axes
	m_pGnuPlot->cmd("set grid");			///< show grid
	m_pGnuPlot->cmd("set title \'Trajectory of the " \
		"Tricycle-Drive\'");				///< set title

	/// if the range is given
	if (bSetRange)
	{
		char str[100] = { 0, };
		sprintf(str, "set xrange [%f:%f]", x_min, x_max);
		m_pGnuPlot->cmd(str);
		sprintf(str, "set yrange [%f:%f]", y_min, y_max);
		m_pGnuPlot->cmd(str);
	}

	/// set labels of x and y axis
	m_pGnuPlot->cmd("set xlabel \'X (m)\'");
	m_pGnuPlot->cmd("set ylabel \'Y (m)\'");

	/// string for gnuplot command
	std::string cmd = "plot \'";
	cmd += m_sFilenamePose;
	cmd += "\' using 2:3 with linespoints pt 7 title \'pose\', \'";
	cmd += m_sFilenameContour;
	cmd += "\' with lines notitle";
	//std::cout << cmd;

	/// display the result plot
	m_pGnuPlot->cmd(cmd.c_str());

#else // defined(WIN32)

	m_fpPipe = popen("gnuplot -persistent", "w");
	if (!m_fpPipe)
	{
		std::cout << "popen() error." << std::endl;
		return;
	}

	fputs("set size ratio -1\n", m_fpPipe);	///< set same ratio in all axes
	fputs("set grid\n", m_fpPipe);			///< show grid
	fputs("set title \'Trajectory of the Tricycle-Drive\'\n", \
		m_fpPipe);							///< set title

	/// if the range is given
	if (bSetRange)
	{
		char str[100] = { 0, };
		sprintf(str, "set xrange [%f:%f]\n", x_min, x_max);
		fputs(str, m_fpPipe);
		sprintf(str, "set yrange [%f:%f]\n", y_min, y_max);
		fputs(str, m_fpPipe);
	}

	/// set labels of x and y axis
	fputs("set xlabel \'X (m)\'\n", m_fpPipe);
	fputs("set ylabel \'Y (m)\'\n", m_fpPipe);

	/// string for gnuplot command
	std::string cmd = "plot \'";
	cmd += m_sFilenamePose;
	cmd += "\' using 2:3 with linespoints pt 7 title \'pose\', \'";
	cmd += m_sFilenameContour;
	cmd += "\' with lines notitle\n";
	//std::cout << cmd;

	/// display the result plot
	fputs(cmd.c_str(), m_fpPipe);

	if (m_fpPipe)
		pclose(m_fpPipe);	///< close pipe

#endif // defined(WIN32)
}
