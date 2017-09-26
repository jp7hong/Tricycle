// pGNUPlot.cpp

#include "StdAfx.h"
#include <stdarg.h>
#include <string.h>
#include "pGNUPlot.h"

#define PARENTCLASS _T("wgnuplot_parent")
#define TEXTCLASS   _T("wgnuplot_text")
#define GRAPHCLASS  _T("wgnuplot_graph")

BOOL CALLBACK cbGetTextWindow(HWND hWnd, LPARAM  lParam)
{
	CpGnuplot *pThis = (CpGnuplot *)lParam;	

	HWND hWndFinded = FindWindowEx(hWnd, NULL, TEXTCLASS, NULL );
	if (hWnd && hWndFinded) {
		pThis->_hWndParent = hWnd;
		pThis->_hWndText   = hWndFinded;
	}

	return TRUE;
}

BOOL CALLBACK cbGetGraphWindow(HWND hWnd, LPARAM  lParam)
{
	CpGnuplot *pThis = (CpGnuplot *)lParam;

	HWND hWndFinded = FindWindowEx(NULL, hWnd, GRAPHCLASS, NULL );
	if (hWndFinded) {
		pThis->_hWndGraph = hWndFinded;
	}

	return TRUE;
}

CpGnuplot::CpGnuplot(const char *gnuplotName)
{
	_hWndParent = NULL;
	_hWndText = NULL;
	_hWndGraph = NULL;

	STARTUPINFO si; 
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi; 
	ZeroMemory( &pi, sizeof(pi) );

	char path[MAX_PATH+1];
	strncpy (path, gnuplotName, MAX_PATH);

	BOOL bSuccess = CreateProcess(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!bSuccess) return;

	for (int i=0; i<100 && !_hWndText; ++i) {
		EnumThreadWindows(pi.dwThreadId, cbGetTextWindow, (LPARAM)this);		
		Sleep (10);
	}
		
	cmd(_T("clear"));

	for (int i=0; i<100 && !_hWndGraph; ++i) {
		EnumThreadWindows(pi.dwThreadId, cbGetGraphWindow, (LPARAM)this);
		Sleep (10);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (_hWndParent){
		::ShowWindow (_hWndParent, SW_HIDE);
	}
}

CpGnuplot::~CpGnuplot()
{
	if(_hWndGraph)	::SendMessage(_hWndGraph, WM_CLOSE, 0, 0);
	if(_hWndText)	::SendMessage(_hWndText, WM_CLOSE, 0, 0);
	if(_hWndParent) ::SendMessage(_hWndParent, WM_CLOSE, 0, 0);
}

void CpGnuplot::cmd(const char *format, ...)
{
    va_list ap;
    char cmd[1024];

    va_start(ap, format);
    vsprintf_s(cmd, sizeof(cmd), format, ap);
    va_end(ap);
 
	if(_hWndText) {
		for (char *p = cmd; *p; ++p){
			PostMessage( _hWndText, WM_CHAR, *p, 1);
		}
		PostMessage( _hWndText, WM_CHAR, '\n', 1);
	}
}
