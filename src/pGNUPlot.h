// pGNUPlot.h

// A C++ interface to gnuplot.

#pragma once

#include <stdio.h>
#include <windows.h>

class CpGnuplot
{
public:
	CpGnuplot(const char *gnuplotName);
	~CpGnuplot();

	// send a command to gnuplot
	void cmd(const char *format, ...);

private:
	HWND _hWndParent;
	HWND _hWndText;
	HWND _hWndGraph;

	friend BOOL CALLBACK cbGetTextWindow(HWND hWnd, LPARAM  lParam);
	friend BOOL CALLBACK cbGetGraphWindow(HWND hWnd, LPARAM  lParam);
};
