#include "StdAfx.h"
#include "MyRegistry.h"
#include "Utils.h"
#include "FileName.h"
#include "XMLParse.h"

bool CMyRegistry::umbLookedForXml = false;
CString CMyRegistry::umsXmlFileName = false;
FILE *CMyRegistry::umpfLog = NULL;

void CMyRegistry::SetLogFile(FILE *pfLog)
{
	umpfLog = pfLog;
}

CMyRegistry::CMyRegistry(REGSAM samDesired)
: msPath(CUtils::GetApplicationRegistryName())
, mhKey(NULL)
, mbOpen(false)
, mSamDesired (samDesired)
, mpXMLParseFile(NULL)
, mpXMLParseRoot(NULL)
{
	DoConsturct(false);
}
CMyRegistry::CMyRegistry(const wchar_t *zPath, REGSAM samDesired, bool bMustRead)
: msPath(zPath)
, mhKey(NULL)
, mbOpen(false)
, mSamDesired (samDesired)
, mpXMLParseFile(NULL)
, mpXMLParseRoot(NULL)
{
	DoConsturct(bMustRead);
}
CMyRegistry::~CMyRegistry(void)
{
    if (mbOpen && mhKey)
        RegCloseKey(mhKey);

	if (mpXMLParseFile)
		delete mpXMLParseFile;
}
void CMyRegistry::DoConsturct(bool bMustRead)
{
	if (umpfLog)
	{
		fwprintf(umpfLog, L"<CMyRegistry> %s %s  ", (const wchar_t *)msPath, bMustRead ? L"MUST" : L"OPTIONAL");
		if (mSamDesired == KEY_READ)
			fwprintf(umpfLog, L"KEY_READ");
		else if (mSamDesired == KEY_WRITE)
			fwprintf(umpfLog, L"KEY_WRITE");
		fwprintf(umpfLog, L"\n");
		fflush(umpfLog);
	}
	if (Open() && (mhKey != NULL))
		return;
	if (umpfLog)
	{
		fwprintf(umpfLog, L"<CMyRegistry> Failed to open registry\n");
		if (mbOpen)
			fwprintf(umpfLog, L"<CMyRegistry> Opened XML definitions file\n");
		fflush(umpfLog);
	}
	if (mSamDesired == KEY_READ)
	{
		if (bMustRead)
		{
	        CString sError (L"RegOpenKeyEx KEY_READ failed: ");
		    sError += msPath;
			if (umpfLog)
			{
				fwprintf(umpfLog, L"<CMyRegistry> Report must read failed\n%s\n", (const wchar_t *)sError);
				fflush(umpfLog);
			}
			CUtils::DisplayLastError(sError);
		}
		return;
	}

	if (umpfLog)
	{
		fwprintf(umpfLog, L"<CMyRegistry> try to create %s\n", (const wchar_t *)msPath);
		fflush(umpfLog);
	}
    CFileName name(msPath);
    CString sRoot = name.Path();
    if (sRoot.Right(1) == L"\\")
        sRoot = sRoot.Left(sRoot.GetLength() - 1);
    CString sPrivate = name.Private();
    if (!sRoot.IsEmpty())
    {
	    HKEY hRootKey;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sRoot, 0, KEY_CREATE_SUB_KEY , &hRootKey) == ERROR_SUCCESS)
        {
            LONG rc = RegCreateKeyEx(hRootKey, sPrivate, 0, NULL, 0, 
                mSamDesired, NULL, &mhKey, NULL);
            if (rc == ERROR_SUCCESS)
                mbOpen = true;
            RegCloseKey(hRootKey);
       }
    }
	if (!mbOpen && !mhKey && bMustRead)
    {
        CString sError (L"RegOpenKeyEx failed: ");
        sError += msPath;
        CUtils::DisplayLastError(sError);
    }
}
bool CMyRegistry::VerifyXml(void)
{
	umsXmlFileName += L".xml";
	if (!CFileName::Exist(umsXmlFileName))
	{
		if (umpfLog)
		{
			fwprintf(umpfLog, L"<CMyRegistry::VerifyXml> umsXmlFileName <%s> Missing\n", (const wchar_t *)umsXmlFileName);
			fflush(umpfLog);
		}
		return false;
	}

	/*
	CString sfLog ("d:\\MyLog\\RegistryXmlName_");
	sfLog += CMyWindows::GetApplicationName();
	sfLog += ".Log";
	FILE *pf = MyFOpen(sfLog, "w");
	if (pf)
	{
		fprintf (pf, "App: %s\n", CMyWindows::GetApplicationPath());
		fprintf (pf, "XML: %s\n", umsXmlFileName);
		fclose(pf);
	} */
	if (umpfLog)
	{
		fwprintf(umpfLog, L"<CMyRegistry::VerifyXml> umsXmlFileName <%s> Exist\n", (const wchar_t *)umsXmlFileName);
		fflush(umpfLog);
	}
	return true;
}
bool CMyRegistry::LookForXml(void)
{
	static int count = 0;
	count++;
	if (umbLookedForXml && count > 5)
	{
		if (umpfLog)
		{
			fwprintf(umpfLog, L"<CMyRegistry::LookForXml> umbLookedForXml %d\n", count);
			if (umsXmlFileName.IsEmpty())
				fwprintf(umpfLog, L"<CMyRegistry::LookForXml> Empty\n");
			else
				fwprintf(umpfLog, L"<CMyRegistry::LookForXml> found <%s>\n", (const wchar_t *)umsXmlFileName);

			fflush(umpfLog);
		}
		return !umsXmlFileName.IsEmpty();
	}

	CString sPath = CUtils::GetApplicationPath();
	if (umpfLog)
	{
		fwprintf(umpfLog, L"<CMyRegistry::LookForXml> GetApplicationPath <%s>\n", (const wchar_t *)sPath);
		fflush(umpfLog);
	}
	if (sPath.IsEmpty())
		return false;

	umbLookedForXml = true;

	// Check for server - use client application's XML
	/*
	CString sClientName;
	CString sClientFullName;
	if (CMyWindows::GetClientName(sClientName, &sClientFullName))
	{
		if (umpfLog)
		{
			fprintf(umpfLog, "<CMyRegistry::LookForXml> sClientFullName <%s>\n", sClientFullName);
			fflush(umpfLog);
		}
		CFileName fName(sClientFullName);
		umsXmlFileName = fName.Path();
		umsXmlFileName += sClientName;
		if (VerifyXml())
			return true;

		if (mbOpen && Get("Client",sClientFullName))
		{
			umsXmlFileName = sClientFullName + sClientName;
			if (VerifyXml())
				return true;
		}
	} */

	umsXmlFileName = sPath + CUtils::GetApplicationName();
	if (umpfLog)
	{
		fwprintf(umpfLog, L"<CMyRegistry::LookForXml> umsXmlFileName <%s>\n", (const wchar_t *)umsXmlFileName);
		fflush(umpfLog);
	}
	if (VerifyXml())
		return true;

	umsXmlFileName = sPath + L"MyViewer\\MyViewer";
	if (VerifyXml())
		return true;

	umsXmlFileName = L"";
	return false;
}
bool CMyRegistry::Open()
{
	// Open Registry
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, msPath, 0, mSamDesired, &mhKey) == ERROR_SUCCESS) 
	{
        mbOpen = true;
		if (umpfLog)
		{
			fwprintf(umpfLog, L"<CMyRegistry::Open> RegOpenKeyEx OK\n");
			fflush(umpfLog);
		}
	}
	else
	{
		mhKey = NULL;
		if (umpfLog)
		{
			fwprintf(umpfLog, L"<CMyRegistry::Open> RegOpenKeyEx Failed\n");
			fflush(umpfLog);
		}
	}

	// Open XML configuration file
	if (LookForXml())
	{
		mpXMLParseFile = new CXMLParse(umsXmlFileName);
		mpXMLParseRoot = mpXMLParseFile->GetRoot();
		if (mpXMLParseRoot)
		{
			mbOpen = true;
			if (umpfLog)
			{
				fwprintf(umpfLog, L"<CMyRegistry::Open> XML <%s> Opened\n", (const wchar_t *)mpXMLParseFile->GetName());
				fflush(umpfLog);
			}
		}
	}
	return mbOpen;
}
bool CMyRegistry::Set(const wchar_t * zName, DWORD value)
{
    if (!mbOpen || !mhKey)
        return false;

    long rc = RegSetValueEx(mhKey, zName, 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));
	if (rc == ERROR_SUCCESS)
        return true;

    CString sError (L"RegSetValueEx failed: ");
    sError += msPath;
    sError += L"\nFailed to set ";
    sError += zName;
    CUtils::DisplayLastError (sError);
    return false;
}
bool CMyRegistry::Set(const wchar_t * zName, const wchar_t *zValue)
{
    if (!mbOpen || !mhKey)
        return false;

    long rc = RegSetValueEx(mhKey, zName, 0, REG_SZ, (LPBYTE)zValue,
		(DWORD)(wcslen(zValue) + 1));
	if (rc == ERROR_SUCCESS)
        return true;

    CString sError (L"RegSetValueEx failed: ");
    sError += msPath;
    sError += L"\nFailed to set ";
    sError += zName;
    CUtils::DisplayLastError (sError);
    return false;
}
bool CMyRegistry::Get(const wchar_t * zName, int & value)
{
	bool bFound = false;
	if (mhKey)
	{
		DWORD tmp = 0;
		DWORD dwBufLen = sizeof(tmp);
		if (RegQueryValueEx(mhKey, zName, 0, NULL, (LPBYTE)&tmp, &dwBufLen) == ERROR_SUCCESS)
		{
			value = (int)tmp;
			bFound = true;
		}
	}
	if (mpXMLParseRoot)
	{
		if (mpXMLParseRoot->GetValue(zName, value))
			bFound = true;
	}

    return bFound;
}
bool CMyRegistry::Get(const wchar_t * zName, DWORD & value)
{
	bool bFound = false;
	if (mhKey)
	{
		DWORD tmp = 0;
		DWORD dwBufLen = sizeof(tmp);
		if (RegQueryValueEx(mhKey, zName, 0, NULL, (LPBYTE)&tmp, &dwBufLen) == ERROR_SUCCESS)
		{
			value = tmp;
			bFound = true;
		}
	}
	if (mpXMLParseRoot)
	{
		if (mpXMLParseRoot->GetValue(zName, value))
			bFound = true;
	}

    return bFound;
}
bool CMyRegistry::Get(const wchar_t * zName, bool & value)
{
	bool bFound = false;
	if (mhKey)
	{
		DWORD tmp = 0;
		DWORD dwBufLen = sizeof(tmp);
		if (RegQueryValueEx(mhKey, zName, 0, NULL, (LPBYTE)&tmp, &dwBufLen) == ERROR_SUCCESS)
		{
			value = (tmp != 0);
			bFound = true;
		}
	}
	if (mpXMLParseRoot)
	{
		if (mpXMLParseRoot->GetValue(zName, value))
			bFound = true;
	}

    return bFound;
}
bool CMyRegistry::Get(const wchar_t * zName, CString &sValue)
{
	if (umpfLog)
		fwprintf(umpfLog,L"<CMyRegistry::Get> string ""%s"" ? (mhKey %p) (pXml %p)\n", 
			zName, (void *)mhKey, mpXMLParseRoot);
	bool bFound = false;
	if (mhKey)
	{
		wchar_t zBuf[256] = L"";
		DWORD dwBufLen = sizeof(zBuf);
		if (RegQueryValueEx(mhKey, zName, 0, NULL, (LPBYTE)zBuf, &dwBufLen) == ERROR_SUCCESS)
		{
			sValue = zBuf;
			bFound = true;
			if (umpfLog)
				fwprintf(umpfLog,L"<CMyRegistry::Get> Registry %s\n", zBuf);
		}
	}
	if (mpXMLParseRoot)
	{
		if (umpfLog)
			mpXMLParseRoot->Print(umpfLog);


		if (mpXMLParseRoot->GetValue(zName, sValue))
		{
			bFound = true;
			if (umpfLog)
				fwprintf(umpfLog,L"<CMyRegistry::Get> XML %s\n", (const wchar_t *)sValue);
		}
	}

    return bFound;
}
CXMLParseNode *CMyRegistry::GetXmlNode(const wchar_t *zName)
{
	if (!mpXMLParseRoot)
		return false;

	return mpXMLParseRoot->GetFirst(zName);
}
