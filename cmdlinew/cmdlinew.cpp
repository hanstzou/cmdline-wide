// cmdlinew.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CLI11.hpp"
#include "windows.h"
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <wchar.h>

int s2ws(std::string &str, std::wstring &wstr)
{
	int ret = 0;
	const int MAX_ARG_LEN = 32767;
	static wchar_t wcBuf[MAX_ARG_LEN];

	ret = MultiByteToWideChar(
		CP_UTF8,
		0, // dwFlags: MBCS flags 0 for UTF8
		str.c_str(),
		-1, // cbMultiByte: null-terminated
		wcBuf,
		MAX_ARG_LEN
	);
	wstr = std::wstring(wcBuf);

	return ret;
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	LPCWSTR wincmdline = GetCommandLine();
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);

	wprintf(L"# LPCWSTR\n");
	size_t len = wcslen(wincmdline) * sizeof(wchar_t);
	BYTE * ptr = (BYTE*)(wincmdline);
	for (size_t i = 0; i < len - 1; ++i)
		wprintf(L"%x,", ptr[i]);
	wprintf(L"%x\n", ptr[len - 1]);
	wprintf(L"%ls\n", wincmdline);



	wprintf(L"\n");
	wprintf(L"# std::wstring\n");
	std::wstring cmdline(wincmdline);
	len = cmdline.length() * sizeof(wchar_t);
	ptr = (BYTE*)(cmdline.c_str());
	for (size_t i = 0; i < len - 1; ++i)
		wprintf(L"%x,", ptr[i]);
	wprintf(L"%x\n", ptr[len - 1]);
	std::wcout << cmdline << std::endl;



	wprintf(L"\n");
	wprintf(L"# wmain argv\n");
	for (int i = 0; i < argc; ++i) {
		wprintf(L"  ## arg %d: ", i);
		len = wcslen(argv[i]) * sizeof(wchar_t);
		ptr = (BYTE*)argv[i];
		for (size_t i = 0; i < len - 1; ++i)
			wprintf(L"%x,", ptr[i]);
		wprintf(L"%x\n", ptr[len - 1]);
	}



	wprintf(L"\n");
	wprintf(L"# wmain argv to UTF-8\n");
	const int MAX_ARG_LEN = 32767;
	char szArgBuffer[MAX_ARG_LEN];
	std::vector<std::string> std_args;
	for (int i = 1; i < argc; ++i) {
		wprintf(L"  ## arg %d: ", i);
		WideCharToMultiByte(
			CP_UTF8,
			0, // dwFlags: MBCS flags 0 for UTF8
			argv[i],
			-1, // cchWideChar: null-terminated
			szArgBuffer,
			MAX_ARG_LEN,
			NULL, // lpDefaultChar: must be NULL for UTF8
			NULL // lpUsedDefaultChar: must be NULL for UTF8
		);
		len = strlen(szArgBuffer);
		ptr = (BYTE*)szArgBuffer;
		std::string tmp_arg(szArgBuffer);
		std_args.push_back(tmp_arg);
		for (size_t i = 0; i < len - 1; ++i)
			wprintf(L"%x,", ptr[i]);
		wprintf(L"%x %hs\n", ptr[len - 1], szArgBuffer);
	}



	wprintf(L"\n");
	wprintf(L"# UTF-8 std::string back to LPCWSTR\n");
	wchar_t wcBuf[MAX_ARG_LEN];
	int arg_i = 0;
	for (std::string tmp_arg : std_args) {
		MultiByteToWideChar(
			CP_UTF8,
			0, // dwFlags: MBCS flags 0 for UTF8
			tmp_arg.c_str(),
			-1, // cbMultiByte: null-terminated
			wcBuf,
			MAX_ARG_LEN
		);
		len = wcslen(wcBuf) * sizeof(wchar_t);
		ptr = (BYTE*)wcBuf;
		wprintf(L"  ## arg %d: ", arg_i);
		for (size_t i = 0; i < len - 1; ++i)
			wprintf(L"%x,", ptr[i]);
		wprintf(L"%x\n", ptr[len - 1]);
		++arg_i;
	}



	wprintf(L"\n");
	wprintf(L"# wmain argv to std::wstring\n");
	std::vector<std::wstring> std_wargs;
	for (int i = 0; i < argc; ++i) {
		std::wstring tmp_warg(argv[i]);
		std_wargs.push_back(tmp_warg);
	}
	arg_i = 0;
	for (std::wstring tmp_warg : std_wargs) {
		len = tmp_warg.length() * sizeof(wchar_t);
		ptr = (BYTE*)(tmp_warg.c_str());
		wprintf(L"  ## arg %d: ", arg_i);
		for (size_t i = 0; i < len - 1; ++i)
			wprintf(L"%x,", ptr[i]);
		wprintf(L"%x\n", ptr[len - 1]);
		++arg_i;
	}


	CLI::App app{ "App description" };

	std::string filename = "default file";
	int debug;
	std::string name = "default name";
	app.add_option("-f,--file", filename, "Input filename");
	app.add_option("-d,--debug", debug, "The debug flag");
	app.add_option("--name", name, "The name field");

	try {
		app.parse(std_args);
	}
	catch (const CLI::ParseError &e) {
		std::string strout;
		std::string strerr;
		std::wstring wstrout;
		std::wstring wstrerr;
		int exit_code = app.exit_info(e, strout, strerr);

		s2ws(strout, wstrout);
		s2ws(strerr, wstrerr);
		//std::wcout << wstrout;
		//std::wcerr << wstrerr << std::flush;
		fwprintf(stdout, L"%ls\n", wstrout.c_str());
		fwprintf(stderr, L"%ls\n", wstrerr.c_str());
		wprintf(L"end except\n");

		return exit_code;
	}

	wprintf(L"debug: %d\n", debug);
	wprintf(L"end\n");

	/*
	int WideCharToMultiByte(
		UINT                               CodePage,
		DWORD                              dwFlags,
		_In_NLS_string_(cchWideChar)LPCWCH lpWideCharStr,
		int                                cchWideChar,
		LPSTR                              lpMultiByteStr,
		int                                cbMultiByte,
		LPCCH                              lpDefaultChar,
		LPBOOL                             lpUsedDefaultChar
	);

	int MultiByteToWideChar(
		UINT                              CodePage,
		DWORD                             dwFlags,
		_In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
		int                               cbMultiByte,
		LPWSTR                            lpWideCharStr,
		int                               cchWideChar
	);
	*/

    return 0;
}
