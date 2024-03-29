#include "StdAfx.h"
#include "Utils.h"
#include "Right.h"
#include "FilesList.h"

bool CUtils::umbCreateConsoleWindow = false;
bool CUtils::umbInstallationError = false;
CString CUtils::msApplicationPath;

CUtils::CUtils(void)
{
}
void CUtils::ReportInstallationError(void)
{
	wchar_t zBuf[256];
	GetCurrentDirectory(255, zBuf);
	CUtils::MessBox(zBuf, L"Installation Error");
	umbInstallationError = true;
	exit(1);
}
CString CUtils::GetBaseDir(void)
{
	static CString usBaseDir(L"");
	if (usBaseDir.IsEmpty())
	{
		wchar_t zBuf[256];
		GetCurrentDirectory(255, zBuf);
		usBaseDir = zBuf;
		usBaseDir.Replace(L"WorkersRightsComputer\\WorkersRightsComputer", L"WorkersRightsComputer\\WorkersRights_v\\");
		usBaseDir.Replace(L"WorkersRightsComputer\\x64\\Debug", L"WorkersRightsComputer\\WorkersRights_v\\");
		usBaseDir.Replace(L"WorkersRightsComputer\\x64\\Release", L"WorkersRightsComputer\\WorkersRights_v\\");
		if (usBaseDir.Right(5).MakeLower() == L"debug")
			usBaseDir = usBaseDir.Left(usBaseDir.GetLength() - 5);
		if (usBaseDir.Right(7).MakeLower() == L"release")
			usBaseDir = usBaseDir.Left(usBaseDir.GetLength() - 7);
	}
	return usBaseDir;
}
const CString& CUtils::GetInputPath()
{
	static CString usInputPath;

	if (usInputPath.IsEmpty())
	{
		usInputPath = GetBaseDir();
		usInputPath += "input\\";
	}
	return usInputPath;
}
FILE* CUtils::OpenInputFile(const wchar_t* zName)
{
	if (umbInstallationError)
		return NULL;

	CString sfName(GetInputPath());
	sfName += zName;
	sfName += ".txt";
	FILE* pf = MyFOpenWithErrorBox(sfName, L"r", L"Input");
	if (!pf)
		ReportInstallationError();
	return pf;
}

FILE * CUtils::TryOpenStreamReader(const wchar_t *zfName, const wchar_t *zDesc, bool bReportError)
{
	FILE *pf;
	_wfopen_s(&pf, zfName, L"r");
	if (!pf && bReportError)
		ReportFileOpenError(zfName, zDesc, true);
	return pf;
}
FILE * CUtils::TryOpenStreamWriter(const wchar_t *zfName, const wchar_t *zDesc, bool bAppend, bool bReportError)
{
	FILE *pf;
	if (bAppend)
		_wfopen_s(&pf, zfName, L"a");
	else
		_wfopen_s(&pf, zfName, L"w");

	if (!pf && bReportError)
		ReportFileOpenError(zfName, zDesc, false);
	return pf;
}
FILE * CUtils::OpenOutputFile(const wchar_t *zName, const wchar_t *zExtension)
{
	CString sfName(CRight::GetSaveDir());
	sfName += zName;
	sfName += L".";
	sfName += zExtension;
	return TryOpenStreamWriter(sfName,L"Save");
}
FILE* CUtils::OpenLogFile(const wchar_t* zName, bool bAppend)
{
	CString sfName = GetBaseDir();
	sfName += "Log";
	if (!VerifyDirectory(sfName))
		return NULL;
	sfName += "\\";
	sfName += zName;
	sfName += L".log";
	return TryOpenStreamWriter(sfName, L"log", bAppend);
}
FILE* CUtils::TryOpenLogFile(const wchar_t* zName, const wchar_t* zAt, bool bAppend)
{
	CString sfName = GetBaseDir();
	sfName += "Log";
	if (!VerifyDirectory(sfName))
		return NULL;
	sfName += "\\";
	sfName += zName;
	if (zAt)
	{
		sfName += "_";
		sfName += zAt;
	}
	sfName += L".log";
	return TryOpenStreamWriter(sfName, L"log", bAppend, false /*bReportError*/);
}
FILE * CUtils::OpenSpecialLogFile(const wchar_t *zName)
{
	CString sfName = CRight::GetSaveDir();
	sfName += CRight::umsName;
	sfName += zName;
	sfName += L".log";
	return TryOpenStreamWriter(sfName, L"log", false, false);
}
void CUtils::ReportFileOpenError(const wchar_t *zfName, const wchar_t *zDesc, bool bRead)
{
	static int nErr = 0;
	nErr++;
	CString s(L"Failed to open ");
	if (bRead)
		s += L"input";
	else
		s += L"output";

	s += L" file ";
	s += zDesc;
	s += L": ";
	s += zfName;
	MessageBox(NULL, s, L"Installation Error", MB_OK);

	wchar_t zBuf[256];
	GetCurrentDirectory(sizeof(zBuf), zBuf);
	MessageBox(NULL, zBuf, L"Current Directory", MB_OK);
}
bool CUtils::FileExists(const wchar_t *zfName)
{
	DWORD rc = GetFileAttributes(zfName);
	return rc != INVALID_FILE_ATTRIBUTES;
}
CString CUtils::ReadLine(FILE* pfRead)
{
	wchar_t zBuf[2048];
	if (fgetws(zBuf, sizeof(zBuf), pfRead) == NULL)
	{
		MessageBox(NULL, L"CUtils::ReadLine failed - file end", L"Input Error", MB_OK);
		return CString(L"");
	}
	CString s(zBuf);
	s.Remove('\n');
	if (s.IsEmpty())
		return ReadLine(pfRead);
	return s;
}
CString CUtils::ReadNextLine(FILE* pfRead)
{
	wchar_t zBuf[2048];
	if (fgetws(zBuf, sizeof(zBuf), pfRead) == NULL)
	{
		MessageBox(NULL, L"CUtils::ReadLine failed - file end", L"Input Error", MB_OK);
		return CString(L"");
	}
	CString s(zBuf);
	s.Remove('\n');
	if (s.IsEmpty())
		return CString("0");
	return s;
}
CString CUtils::TryReadLine(FILE *pfRead)
{
	wchar_t zBuf[256];
	if (fgetws(zBuf, sizeof(zBuf), pfRead) == NULL)
		return CString("");
	CString s(zBuf);
	s.Remove('\n');
	if (s.IsEmpty())
		return TryReadLine(pfRead);
	return s;
}
int CUtils::ReadInt(FILE *pfRead)
{
	wchar_t zBuf[256];
	if (fgetws(zBuf, sizeof(zBuf), pfRead) == NULL)
	{
		MessageBox(NULL, L"CUtils::ReadInt failed - file end", L"Input Error", MB_OK);
		return 0;
	}
	if (!isdigit(zBuf[0]))
	{
		MessageBox(NULL, L"CUtils::ReadInt failed - wrong input", L"Input Error", MB_OK);
		return 0;
	}
	return _wtoi(zBuf);
}
double CUtils::ReadFloat(FILE *pfRead)
{
	wchar_t zBuf[256];
	if (fgetws(zBuf, sizeof(zBuf), pfRead) == NULL)
	{
		MessageBox(NULL, L"CUtils::ReadFloat failed - file end", L"Input Error", MB_OK);
		return 0;
	}
	if (zBuf[0] == '\n')
		if (fgetws(zBuf, sizeof(zBuf), pfRead) == NULL)
		{
			MessageBox(NULL, L"CUtils::ReadFloat failed - file end", L"Input Error", MB_OK);
			return 0;
		}

	if (!isdigit(zBuf[0]))
	{
		MessageBox(NULL, L"CUtils::ReadFloat failed - wrong input", L"Input Error", MB_OK);
		return 0;
	}
	return _wtof(zBuf);
}
bool CUtils::VerifyDirectory(const wchar_t * zDirName)
{
	WIN32_FILE_ATTRIBUTE_DATA fileAttrib;
	BOOL bOK = GetFileAttributesEx(zDirName, GetFileExInfoStandard, &fileAttrib);
	if (bOK)
	{
		wchar_t zBuf[1024];
		if (fileAttrib.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;

		CString s(L"Failed to create directory:\n");
		s += zDirName;
		s += L"\nThere's file with this name!\n";
		MessageBox(NULL, zBuf, L"Error", MB_OK);
		return false;
	}
	int rc = _wmkdir(zDirName);
	if (rc == 0)
		return true;

	/*
	char zDirNameA[128];
	size_t size = 0;
	errno_t err = wcstombs_s(
		&size,
		zDirNameA,
		sizeof(zDirNameA),
		(const wchar_t *)zDirName,
		sizeof(zDirNameA)-1
	);
	if (CreateDirectoryA(zDirNameA, NULL))
		return true;*/

	//DisplayLastError();
	CString s(L"Failed to create directory:\n");
	s += zDirName;
	MessageBox(NULL, s, L"Error", MB_OK);
	return false;
}
bool CUtils::IsDirectory(const wchar_t * zDirName)
{
	WIN32_FILE_ATTRIBUTE_DATA fileAttrib;
	BOOL bOK = GetFileAttributesEx(zDirName, GetFileExInfoStandard, &fileAttrib);
	if (bOK)
	{
		if (fileAttrib.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}
	return false;
}

void CUtils::MessBox(const wchar_t *zText, const wchar_t *zTitle, FILE *pfLog)
{
	//MessBox2File(zText, zTitle);

	if (pfLog)
	{
		fwprintf(pfLog, L"\n");
		//PrintLocalTime(pfLog);
		fwprintf(pfLog, L"<CMyWindows::MessBox> %s: %s\n\n", zTitle, zText);
		fflush(pfLog);
	}

	MessageBox(NULL, zText, zTitle, MB_OK);
	/*
	if (umbAllowMessBox)
	{
		if (umpMainWindow)
			umpMainWindow->MessageBox(zText, zTitle, MB_OK);
		else
			MessageBox(NULL, zText, zTitle, MB_OK);
	}*/
	// Save Error Text
	/*
	umsMyLastError = zTitle;
	umsMyLastError += ": ";
	umsMyLastError += zText;
	if (!umnMessBox)
		umsMyFirstError = umsMyLastError;
	umnMessBox++;

	if (umbWriteToPipe)
	{
		CString s(_T("Box: "));
		s += umsMyLastError;
		WriteToPipe(s);
	} */
}
int CUtils::ListFilesInDir(const wchar_t *zDir, const wchar_t *zExt, class CFilesList &list)
{
	if (!CUtils::IsDirectory(zDir))
		return 0;
	CString sPattern(zDir);
	sPattern += L"\\*";
	if (zExt)
	{
		if (zExt[0] != '.')
			sPattern += L".";
		sPattern += zExt;
	}
	return ListFiles(sPattern, list);
}
int CUtils::ListFilesInDirRecursive(const wchar_t *zDir, const wchar_t *zPattern, class CFilesList &list)
{
	CString sDir(zDir);
	int nListed = ListFiles(sDir + zPattern, list);

	CFileFind finder;
	BOOL bFilesFound = finder.FindFile(sDir + L"*");

	while (bFilesFound)
	{
		bFilesFound = finder.FindNextFile();
		CString *psName = new CString(finder.GetFilePath());
		if (IsDirectory(*psName))
		{
			if (IsRealDirectory(*psName))
			{
				*psName += "\\";
				nListed += ListFilesInDirRecursive(*psName, zPattern, list);
			}
		}
		delete psName;
	}
	return nListed;
}
int CUtils::ListFiles(const wchar_t *zPattern, CFilesList &list, bool bRecursive)
{
	int nListed = 0;
	CFileFind finder;
	BOOL bFilesFound = finder.FindFile(zPattern);

	while (bFilesFound)
	{
		bFilesFound = finder.FindNextFile();
		CString *psName = new CString(finder.GetFilePath());
		if (IsDirectory(*psName))
		{
			if (bRecursive)
			{
				if (IsRealDirectory(*psName))
				{
					*psName += "\\*.*";
					nListed += ListFiles(*psName, list, bRecursive);
				}
			}
		}
		else
		{
			list.Add(*psName);
			nListed++;
		}
		delete psName;
	}
	return nListed;
}
bool CUtils::IsRealDirectory(const wchar_t * zDirName)
{
	if (!IsDirectory(zDirName))
		return false;

	size_t len = wcslen(zDirName);
	if (len > 1 && zDirName[len - 1] == '.')
	{
		if (zDirName[len - 2] == '\\' || zDirName[len - 2] == '/')
			return false;

		if (len > 2 && zDirName[len - 2] == '.' && (zDirName[len - 3] == '\\' || zDirName[len - 3] == '/'))
			return false;
	}
	return true;
}
CString CUtils::GetApplicationRegistryName(void)
{
	return CString(L"WorkerRights");
}
CString CUtils::GetApplicationName(void)
{
	return CString(L"WorkerRights");
}
void CUtils::DisplayLastError(const wchar_t *zErrorDesc)
{
	DWORD lastError = GetLastError();
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		lastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	// Display the string
	CString sError((LPCTSTR)lpMsgBuf);
	if (zErrorDesc)
	{
		sError += L"\n";
		sError += zErrorDesc;
	}

	MessBox(sError, L"Last Error");

	// Free the buffer.
	LocalFree(lpMsgBuf);
}
CString CUtils::GetApplicationPath(void)
{
	return msApplicationPath;
}
bool CUtils::CreateThread(DWORD(WINAPI startFunc)(LPVOID), LPVOID lpParameter, HANDLE *pohThread)
{
	static int count = 0;
	//CMyTracer myTracer("CMyWindows::CreateThread", ++count);
	HANDLE hThread = ::CreateThread(NULL, 0, startFunc, lpParameter, 0, NULL);
	if (hThread == NULL)
	{
		DisplayLastError(L"Create Thread");
		return false;
	}
	if (pohThread)
		*pohThread = hThread;
	return true;
}
bool CUtils::CreateProcess(const wchar_t * zProgram, const wchar_t * zParameters, PROCESS_INFORMATION * /*opProcInfo*/)
{
	if (!zProgram || (zProgram[0] == 0))
		return false;

	//STARTUPINFO startInfo;
	//GetStartupInfo(&startInfo);
	STARTUPINFOA info;
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&info, sizeof(STARTUPINFO));
	ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));

	CString sCommandLine;

	if (zProgram[0] == '\"')
		sCommandLine = zProgram;
	else
	{
		sCommandLine = L"\"";
		sCommandLine += zProgram;
		sCommandLine += L"\"";
	}
	                 
	if (zParameters != NULL)
	{
		sCommandLine += L" ";
		sCommandLine += zParameters;
	}

	char zCommandA[128];
	size_t size = 0;
	/*errno_t err = */ wcstombs_s(
		&size,
		zCommandA,
		sizeof(zCommandA),
		(const wchar_t *)sCommandLine,
		sizeof(zCommandA) - 1
	);

	BOOL bOK = ::CreateProcessA(NULL,
		zCommandA, // sCommandLine.GetBuffer(),
		NULL, NULL, false,
		0, // umbCreateConsoleWindow ? 0 : CREATE_NO_WINDOW,  // DWORD dwCreationFlags
		NULL,
		NULL,
		&info,
		&processInfo

	);
	umbCreateConsoleWindow = false;
	if (!bOK)
	{
		DisplayLastError(L"Craete Process");
		wchar_t zBuf[1024];
		swprintf_s(zBuf, 1024, L"Failed to create process:\n%s\n", (const wchar_t *)sCommandLine);
		MessBox(zBuf, L"Error");
	}

	/*
	static int count = 0;
	CString sLog(L"CreateProcess");
	sLog += umsApplicationName;
	FILE *pf = FOpenLogFileAccum(sLog, count++ ? true : false);
	if (pf)
	{
		fprintf(pf, "RUN:    %s\n", zProgram);
		fprintf(pf, "PARAMS: %s\n", zParameters);
		if (!bOK)
			fprintf(pf, "FAILED\n");
		fprintf(pf, "\n");
		fclose(pf);
	}*/
	return bOK != 0;
}
void CUtils::OpenTextFile(const wchar_t * zfName)
{
	CUtils::CreateProcess(L"notepad", zfName);
}
bool CUtils::ReadCommandLine(const CString& sCommand, CString& osfName)
{
	CString msAppFile;
	CString sRest = sCommand;
	if (!ReadFileNameFromCommandLine(sRest, msAppFile, sRest))
		return false;

	msApplicationPath = CFileName::GetPath(msAppFile);
	SetCurrentDirectory(msApplicationPath);
	CString sBaseDir = GetBaseDir();

	if (!ReadFileNameFromCommandLine(sRest, osfName, sRest))
		return false;
	if (!CFileName::StaticIsOfType(osfName, L"xml"))
	{
		MessBox(osfName, L"Workers Rights Computer can open only ""xml"" files");
		exit(1);
	}
	return (!osfName.IsEmpty());
}
static int GetMinFind(int i1, int i2)
{
	if (i1 < 0)
		return i2;
	if (i2 < 0)
		return i1;
	return min(i1, i2);
}
bool CUtils::ReadFileNameFromCommandLine(const CString& sCommand, CString& osfName, CString& osRest)
{
	unsigned char quot2 = (unsigned char)0x93;
	unsigned char quot3 = (unsigned char)0x94;
	CString sRest(sCommand);

	while (!sRest.IsEmpty())
	{
		unsigned char first = (unsigned char)sRest[0];
		if (first == ' ' || first == '\t' || first == '\n')
			sRest = sRest.Right(sRest.GetLength() - 1);
		else if (first == '"' || first == quot2)
		{
			sRest = sRest.Right(sRest.GetLength() - 1);
			int iAfter = sRest.Find('"');
			iAfter = GetMinFind(iAfter, sRest.Find(quot2));
			iAfter = GetMinFind(iAfter, sRest.Find(quot3));

			CString sTry = sRest.Left(iAfter);
			sRest = sRest.Right(sRest.GetLength() - iAfter - 1);
			if (CFileName::Exist(sTry))
			{
				osfName = sTry;
				osRest = sRest;
				return true;
			}
		}
		else // Any other first character
		{
			int iAfter = sRest.Find(' ');
			if (iAfter < 1)
				iAfter = sRest.GetLength();
			CString sTry = sRest.Left(iAfter);
			sRest = sRest.Right(sRest.GetLength() - iAfter);
			if (CFileName::Exist(sTry))
			{
				osfName = sTry;
				osRest = sRest;
				return true;
			}
		}
	}
	return false;
}
