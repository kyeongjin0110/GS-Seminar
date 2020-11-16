#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

class Process
{
public:

	Process(std::string file_name) : _file_name(file_name)
	{
		ZeroMemory(&_si, sizeof(_si));
		_si.cb = sizeof(_si);
		ZeroMemory(&_pi, sizeof(_pi));
	}

	~Process()
	{
		// Close process and thread handles. 
		CloseHandle(_pi.hProcess);
		CloseHandle(_pi.hThread);
	}

	bool createProcess()
	{
		char* filename = const_cast<char*>(_file_name.c_str());
		bool success =
			CreateProcess(
				NULL,			// No module name (use command line)
				filename,		// Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				&_si,           // Pointer to STARTUPINFO structure
				&_pi);          // Pointer to PROCESS_INFORMATION structure

		if (success)
		{
			return true;
		}
		else
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return false;
		}
	}

	void wait()
	{
		// Wait until child process exits.
		WaitForSingleObject(_pi.hProcess, INFINITE);
	}

	bool isRunning()
	{
		DWORD exit_code;
		GetExitCodeProcess(_pi.hProcess, &exit_code);
		return 259 == exit_code;
	}

private:

	STARTUPINFO _si;
	PROCESS_INFORMATION _pi;
	std::string _file_name;
};