///
/// @file		main.cpp
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Aug 29, 2016
/// @version	1.0
///
/// @brief		Calculates odometry for the tricycle drive
///

#include <iostream>			// std::cout
#include <cstdlib>			// atoi

#include "TestTricycle.h"	// CTestTricycle

#define TEST_CASE_NUM	(4)

///
/// @brief		show usage of this program
/// @param		exeFilename [in] executed filename
/// @return		void
///
void ShowUsage(char* exeFilename)
{
	std::cout << "Usage: " << exeFilename << " <test_case_num>" << std::endl;
	std::cout << "Range of <test_case_num>: 1.." << TEST_CASE_NUM << std::endl;
}

///
/// @brief		entry point of this program
/// @param		argc [in] the number of arguments
/// @param		argv [in] string point array of arguments
/// @return		0 on success
///
int main(int argc, char* argv[])
{
	/// test case number
	int test_case = -1;

	/// check arguments
	if (argc != 2)
	{
		ShowUsage(argv[0]);
		return 0;
	}

	/// convert to integer
	test_case = atoi(argv[1]);

	/// if the argument is not within the range
	if (test_case <= 0 || test_case > TEST_CASE_NUM)
	{
		ShowUsage(argv[0]);
		return 0;
	}

	/// run test code
	CTestTricycle::GetInstance()->Run(test_case);

	return 0;
}
