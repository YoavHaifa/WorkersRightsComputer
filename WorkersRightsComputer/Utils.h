#pragma once
#include "FileName.h"



inline FILE * MyFOpen(const wchar_t *zfName, const wchar_t *zMode)
{
	FILE *pf = NULL;
	_wfopen_s(&pf, zfName, zMode);
	return pf;
}

class CUtils
{
public:
	CUtils(void);

	static bool umbInstallationError; 
	static CString GetBaseDir(void);
	static void ReportInstallationError(void);
	static FILE* OpenInputFile(const wchar_t* zName);
	//static FILE* OpenLogFile(const char* zName);

	static FILE * TryOpenStreamReader(const wchar_t *zfName, const wchar_t *zDesc, bool bReportError = true);
	static FILE * TryOpenStreamWriter(const wchar_t *zfName, const wchar_t *zDesc, bool bAppend = false);

	// static FILE * OpenLetterInput(const wchar_t *zName, const wchar_t *zExtension);
	//static FILE * OpenInputFile(const wchar_t *zName, const wchar_t *zExtension);
	static FILE * OpenOutputFile(const wchar_t *zName, const wchar_t *zExtension);
	static FILE * OpenLogFile(const wchar_t *zName, bool bAppend = false);
	static FILE * OpenSpecialLogFile(const wchar_t *zName);
	// static FILE * OpenSaveFileRead(const wchar_t *zName);

	static void ReportFileOpenError(const wchar_t *zfName, const wchar_t *zDesc, bool bRead);

	static bool FileExists(const wchar_t *zfName);
	static CString ReadLine(FILE *pf);
	static CString TryReadLine(FILE *pf);
	static int ReadInt(FILE *pfRead);
	static double ReadFloat(FILE *pfRead);
	static bool VerifyDirectory(const wchar_t * zDirName);
	static bool IsDirectory(const wchar_t * zDirName);
	static void MessBox(const wchar_t *zText, const wchar_t *zTitle, FILE *pfLog = NULL);

	static int ListFiles(const wchar_t *zPattern, class CFilesList &list, bool bRecursive = false);
	static int ListFilesInDir(const wchar_t *zDir, const wchar_t *zExt, class CFilesList &list);
	static int ListFilesInDirRecursive(const wchar_t *zDir, const wchar_t *zPattern, class CFilesList &list);
	static bool IsRealDirectory(const wchar_t * zDirName); // Call me only if it is a directory...
	static CString GetApplicationRegistryName(void);
	static CString GetApplicationName(void);
	static void DisplayLastError(const wchar_t *zError);
	static CString GetApplicationPath(void);
	static bool CreateThread(DWORD(WINAPI startFunc)(LPVOID), LPVOID lpParameter, HANDLE *pohThread = NULL);
	static bool CreateProcess(const wchar_t * zProgram, const wchar_t * zParameters, PROCESS_INFORMATION *opProcInfo = NULL);
	static bool umbCreateConsoleWindow;
	static void CUtils::OpenTextFile(const wchar_t * zfName);
};

inline FILE * MyFOpenWithErrorBox(const wchar_t *zfName, const wchar_t *zMode, const wchar_t *zDesc, FILE *pfLog = NULL)
{
	FILE *pf = NULL;
	errno_t err = _wfopen_s(&pf, zfName, zMode);
	if (!pf)
	{
		CString s = L"Failed to open ";
		s += zDesc;
		s += L"\r\nName: ";
		s += zfName;
		s += L"\r\nMode: ";
		s += zMode;
		s += "\r\n";
		bool bExist = CFileName::Exist(zfName);
		if (!bExist)
		{
			s += L"File doesn't exists";
		}
		else
		{ 
			//wchar_t zErr[1024] = "Unknown Type";
			//strcpy_s(zErr, sizeof(zErr), _sys_errlist[];

			//wchar_t zErr[128];
			//_wcserror_s(zErr, sizeof(zErr), err);
			wchar_t zMsg[256];
			swprintf_s(zMsg, sizeof(zMsg), L"Error %d\n\r File exist\n\r ", err);
			s += zMsg;
		}

		if (pfLog)
		{
			fwprintf(pfLog, L"\n<MyFOpenWithErrorBox> ERROR:\n");
			fwprintf(pfLog, L" - %s\n\n", (const wchar_t *)s);
			fflush(pfLog);
		}
		CUtils::MessBox(s, L"fopen error");
	}
	return pf;
}
