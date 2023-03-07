#include "pch.h"
#include "CommandWnd.h"
#include <iostream>

BOOL InitializeCommandWnd(BOOL initMode)
{
	BOOL rc = FALSE;
	TCHAR szOutputPipeName[256];
	TCHAR szInputPipeName[256];
	TCHAR szErrorPipeName[256];
	FILE *stream;

	// construct named pipe names
	//
	_stprintf_s(szOutputPipeName, _T("\\\\.\\pipe\\%dcout"), GetCurrentProcessId());
	_stprintf_s(szInputPipeName, _T("\\\\.\\pipe\\%dcin"), GetCurrentProcessId());
	_stprintf_s(szErrorPipeName, _T("\\\\.\\pipe\\%dcerr"), GetCurrentProcessId());

	// attach named pipes to stdin/stdout/stderr
	//
	rc = _wfreopen_s(&stream, szOutputPipeName, _T("a"), stdout) == 0 &&
		_wfreopen_s(&stream, szInputPipeName, _T("r"), stdin) == 0 &&
		_wfreopen_s(&stream, szErrorPipeName, _T("a"), stderr) == 0;


	// if unsuccessfule, i.e. no console was available
	// and initmode specifiec that we need to create one
	// we do so
	//
	if (!rc && initMode)
	{
		rc = AllocConsole();
		if (rc)
		{
			rc = _wfreopen_s(&stream, _T("CONOUT$"), _T("a"), stdout) == 0 &&
				_wfreopen_s(&stream, _T("CONIN$"), _T("r"), stdin) == 0 &&
				_wfreopen_s(&stream, _T("CONERR$"), _T("a"), stderr) == 0;
		}
	}

	// synchronize iostreams with standard io
	//
	//	if ( rc )
	//		ios::sync_with_stdio();
	return rc;
}