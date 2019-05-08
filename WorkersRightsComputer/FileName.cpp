#include "StdAfx.h"
#include "FileName.h"
#include "Utils.h"


CFileName::CFileName(const wchar_t *zName)
: msName(_T(""))
, miExtension(0)
, miPrivate(0)
{
    Set (zName);
}
CFileName::CFileName(const wchar_t *zPath, const wchar_t *zName)
: msName(_T(""))
, miExtension(0)
, miPrivate(0)
{
    Set (zName);
    ChangePath(zPath);
}
CFileName::CFileName(const wchar_t *zPath, const wchar_t *zName, const wchar_t *zExt)
: msName(_T(""))
, miExtension(0)
, miPrivate(0)
{
    Set (zName);
    ChangePath(zPath);
	ChangeType(zExt);
}
CFileName::CFileName(void)
: msName(_T(""))
, miExtension(0)
, miPrivate(0)
{
}
CFileName::~CFileName(void)
{
}
const wchar_t * CFileName::Name(void) const
{
    return (const wchar_t *)msName;
}
const wchar_t * CFileName::Type(void) const
{
    return (const wchar_t *)msName + miExtension;
}
const wchar_t * CFileName::Private(void) const
{
    return (const wchar_t *)msName + miPrivate;
}
CString CFileName::PrivateWithoutType(void) const
{
	CString sPrivate(Private());
	CString sType(Type());
	if (!sType.IsEmpty())
	{
		int baseLength = sPrivate.GetLength() - sType.GetLength() - 1;
		if (baseLength > 0)
			return sPrivate.Left(baseLength);
	}
	return sPrivate;
}
CString CFileName::PrivateBase(void)
{
	CString s(Private());
	int iFind = s.FindOneOf(L"._");
	if (iFind > 0)
		return s.Left(iFind);

	return s;
}
CString CFileName::NameWithoutType(void)
{
	CString sType(Type());
	if (!sType.IsEmpty())
	{
		int baseLength = msName.GetLength() - sType.GetLength() - 1;
		if (baseLength > 0)
			return msName.Left(baseLength);
	}
	return msName;
}
CString CFileName::NameWithoutTypeAndMetaType(void)
{
	CString sName(NameWithoutType());
	CFileName fName(sName);
	return fName.NameWithoutType();
}
void CFileName::AddMetaTypeAndType(const wchar_t *zMetaType, const wchar_t *zType)
{
	CString s(msName);
	s += L".";
	s += zMetaType;
	s += L".";
	s += zType;
	Set(s);
}
CString CFileName::Path(bool bCurrentDirIfNone)
{
    if (miPrivate == 0)
	{
		if (bCurrentDirIfNone)
		{
			wchar_t zBuf[1024];
			GetCurrentDirectory(sizeof(zBuf),zBuf);
			wcscat_s(zBuf,sizeof(zBuf),L"\\");
			return CString(zBuf);
		}
		return CString(L"");
	}

	int len = msName.GetLength();
	if (miPrivate < len)
	    return msName.Left (miPrivate);

	CString sTemp(msName);
	CString sLast = sTemp.Right(1);
	while (len > 0 && (sLast == '\\' || sLast == '/'))
	{
		sTemp = sTemp.Left(--len);
		sLast = sTemp.Right(1);
	}

	int iFind = sTemp.ReverseFind('\\');
	int iFind2 = sTemp.ReverseFind('/');
	int iLast = max(iFind, iFind2);
	if (iLast > 0)
		return sTemp.Left(iLast + 1);
	return CString(L"");
}
CString CFileName::PathDir(void)
{
    if (miPrivate == 0)
        return CString(L".\\");

    return msName.Left (miPrivate - 1);
}
bool CFileName::HasFullPath (void)
{
    if (miPrivate == 0)
        return false;
    wchar_t firstChar = msName[0];
    if (firstChar == '.' || firstChar == '/' || firstChar == '\\')
        return true;
    if (msName[1] == ':')
        return true;
    return false;
}
bool CFileName::IsOfType(const wchar_t *zType)
{
	if (!miExtension)
        return false;

    return (_wcsicmp (zType, Type()) == 0);
}
bool CFileName::StaticIsOfType(const wchar_t *zName, const wchar_t *zType)
{
	CFileName fName(zName);
	return fName.IsOfType(zType);
}
bool CFileName::IsOfMetaType(const wchar_t *z)
{
	if (!miExtension)
        return false;

    int len = (int)wcslen (z);
    int start = (int)miExtension - 1 - len;
    if (start < 0)
        return false;

    return (_wcsnicmp (z, (const wchar_t *)msName + start, len) == 0);
}
CString CFileName::GetMetaType(void)
{
	CString sMetaType(L"");
	if (miExtension)
	{
		int start = (int)miExtension - 2;
		while (start >= 0 && msName[start] != '.')
			start--;
		if (start >= 0)
		{
			start++;
			sMetaType = msName.Mid(start,miExtension-start-1);
		}
	}
    return sMetaType;
}
bool CFileName::IsImageFromHostArchives(void)
{
	if (HasExtension())
        return false;

    const wchar_t *z = Private();

    return (z[0] == 'I'); // Multi-file from nominal archives
}

void CFileName::Set(const wchar_t *zName)
{
    msName = zName;
    miExtension = msName.ReverseFind ('.');
    miExtension++;  // Also in case of -1 for 'not found'

    miPrivate = msName.ReverseFind ('\\');
    
    int iSlash = msName.ReverseFind ('/');
    if (iSlash > miPrivate)
        miPrivate = iSlash;
    miPrivate++;

    if (miExtension < miPrivate)
        miExtension = 0;
}
void CFileName::ChangeType(const wchar_t *zNewType)
{
    if (miExtension > 0)
        Set (msName.Left(miExtension) + zNewType);
    else
		AddType(zNewType);
}
void CFileName::AddType(const wchar_t *zNewType)
{
    msName += ".";
    msName += zNewType;
	miExtension = msName.ReverseFind ('.');
	miExtension++;  // Also in case of -1 for 'not found'
}
void CFileName::ChangePath(const wchar_t *zNewPath)
{
    if (!zNewPath)
        return;

    size_t len = wcslen (zNewPath);
    if (len < 1)
        return;

    wchar_t last = zNewPath [len - 1];
    if (last == '\\' || last == '/')
        Set (CString(zNewPath) + Private());
    else
        Set (CString(zNewPath) + "\\" + Private());
}
void CFileName::PrefixPath(const wchar_t * zPrefix)
{
    if (!zPrefix)
        return;

    size_t len = wcslen (zPrefix);
    if (len < 1)
        return;

    wchar_t last = zPrefix [len - 1];
    if (last == '\\' || last == '/')
        Set (CString(zPrefix) + Name());
    else
        Set (CString(zPrefix) + "\\" + Name());
}

bool CFileName::ChangeDrive(wchar_t drive)
{
	if (msName.IsEmpty())
		return false;
    msName.SetAt (0, drive);
	return true;
}
bool CFileName::ChangeToVerifiedSubDir(const wchar_t *zSubDirName)
{
	CString sPath = Path();
	sPath += "\\";
	sPath += zSubDirName;
	if (!CUtils::VerifyDirectory(sPath))
		return false;

	ChangePath(sPath);
	return true;
}
bool CFileName::ChangeToSubDirIfExists(const wchar_t *zSubDirName)
{
	CString sPath = Path();
	sPath += "\\";
	sPath += zSubDirName;
	if (!CUtils::IsDirectory(sPath))
		return false;

	ChangePath(sPath);
	return true;
}
FILE *CFileName::Open (const wchar_t *zMode)
{
    return MyFOpen ((const wchar_t *)msName, zMode);
}
FILE *CFileName::OpenWithErrorBox (const wchar_t *zMode, const wchar_t *zDesc)
{
    return MyFOpenWithErrorBox ((const wchar_t *)msName, zMode, zDesc);
}
bool CFileName::Exist (const wchar_t *zfName)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttrib;
    BOOL bOK = GetFileAttributesEx (zfName, GetFileExInfoStandard, &fileAttrib);
    if (bOK)
        return true;
    return false;
}
bool CFileName::ExistNonEmpty (const wchar_t *zfName)
{
	return GetFileLength(zfName) > 0;
}
FILETIME CFileName::GetModificationTime (const wchar_t *zfName)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttrib;
    BOOL bOK = GetFileAttributesEx (zfName, GetFileExInfoStandard, &fileAttrib);
    if (bOK)
        return fileAttrib.ftLastWriteTime;

    FILETIME fileTime;
    fileTime.dwHighDateTime = 0;
    fileTime.dwLowDateTime = 0;
    return fileTime;
}
void CFileName::MakeLegalName(wchar_t * zName)
{
    while (*zName)
    {
        wchar_t ch = *zName;
        if (ch == '"' || ch == '%' || ch == ' ')
            *zName = '_';
        zName++;
    }
}
void CFileName::MakeLegalName(CString &sName)
{
	if (sName.IsEmpty())
		return;

    sName.Replace (' ', '_');
    sName.Replace ('"', '_');
    sName.Replace ('%', '_');
	bool bDrive = sName[1] == ':';
    sName.Replace (':', '_');
    sName.Replace ('*', '_');
	if (bDrive)
		sName.SetAt(1, ':');
}
void CFileName::MakeLegalShortName(CString &sName)
{
	if (sName.IsEmpty())
		return;
    MakeLegalName(sName);
    sName.Replace ('/', '_');
    sName.Replace ('\\', '_');
}


CString CFileName::FirstName(void)
{
    CString s(Private());
    int iDot = s.Find('.');
    if (iDot > 0)
        return s.Left(iDot);
    return s;
}
CString CFileName::GetPrivate(const wchar_t * zName)
{
	int len = (int)wcslen(zName);
	if (zName[len-1] == '\\' || zName[len-1] == '/')
	{
		CString sName(zName);
		sName = sName.Left(len-1);
		return GetPrivate(sName);
	}
    CFileName fName(zName);
    return CString(fName.Private());
}
CString CFileName::GetPrivateWithoutType(const wchar_t * zName)
{
	CFileName fName(zName);
	return fName.PrivateWithoutType();
}
CString CFileName::GetPath(const wchar_t * zName)
{
    CFileName fName(zName);
    return fName.Path();
}
CString CFileName::GetDirOrPath(const wchar_t * zName)
{
	if (CUtils::IsDirectory(zName))
		return CString(zName);
	return GetPath(zName);
}
CString CFileName::GetLastDirName(void)
{
    CString sPath = Path();
    if (sPath.IsEmpty())
        return sPath;
    CString sLast = sPath.Right(1);
    if (sLast == '\\' || sLast == '/')
        sPath = sPath.Left(sPath.GetLength() - 1);
    int i = sPath.ReverseFind('\\');
    int i2 = sPath.ReverseFind('/');
    if (i2 > i)
        i = i2;
    if (i < 0)
        return sPath;
    return sPath.Right(sPath.GetLength() - i - 1);
}
CString CFileName::GetLastDirName(const wchar_t * zName)
{
    CFileName fName(zName);
	return fName.GetLastDirName();
}

bool CFileName::Exist(void)
{
	if (msName.IsEmpty())
		return false;
    return Exist(msName);
}
CString CFileName::GetIndexedName(int index, bool bKeepSecondExtension)
{
	wchar_t zAdd[128];
	swprintf_s(zAdd, 128, L"_%03d", index);

	if (!miExtension)
		return msName + zAdd;
	
	int iAddAt = miExtension-1;
	CString s (msName.Left(miExtension-1));

	if (bKeepSecondExtension)
	{
		int iSecondExtensionPoint = s.ReverseFind ('.');
		if (iSecondExtensionPoint > miPrivate)
		{
			iAddAt = iSecondExtensionPoint;
			s = msName.Left(iSecondExtensionPoint);
		}
	}
 
	s += zAdd;
	s += (const wchar_t *)msName + iAddAt;
	return s;
}
CString CFileName::GetBaseOfPattern(const wchar_t * zPattern)
{
	CString sPattern(zPattern);
	int iStar = sPattern.Find('*');
	if (iStar == 0)
		return CString("");
	if (iStar > 0)
		sPattern = sPattern.Left(iStar);

	if (CUtils::IsDirectory(sPattern))
		return sPattern;

	CFileName fName(sPattern);
	return fName.Path();
}
bool CFileName::CreateRelativePath(const wchar_t * zBase, const wchar_t * zSrcPath, CString &osPath)
{
	CString sPath(zBase);
	CString sSrcPath(zSrcPath);

	while (!sSrcPath.IsEmpty())
	{
		wchar_t ch0 = sSrcPath[0];
		if (ch0 == '\\' || ch0 == '/')
			sSrcPath = sSrcPath.Right(sSrcPath.GetLength() - 1);
		else
		{
			const wchar_t *zSrc = (const wchar_t *)sSrcPath;
			int iEnd = -1;
			int iLook = 0;
			while (zSrc[iLook])
			{
				wchar_t chCheck = zSrc[iLook];
				if (chCheck == '\\' || chCheck == '/')
				{
					iEnd = iLook;
					break;
				}
				iLook++;
			}
			if (iEnd > 0) // Direcotry
			{
				CString sDirName(sSrcPath.Left(iEnd));
				sPath += "\\";
				sPath += sDirName;
				if (!CUtils::VerifyDirectory(sPath))
					return false;
				sSrcPath = sSrcPath.Right(sSrcPath.GetLength() - iEnd - 1);
			}
			else
				break;
		}
	}

	osPath = sPath;
	return true;
}
CString CFileName::GetLastInPath(const wchar_t *zPath)
{
	CString sPath(zPath);
	if (sPath.IsEmpty())
		return sPath;

	CString sLastChar(sPath.Right(1));
	if (sLastChar == "\\" || sLastChar == "/")
		sPath = sPath.Left(sPath.GetLength() - 1);

	CFileName fName(sPath);
	return fName.Private();
}
CString CFileName::BuildFileName(const wchar_t *zPath, const wchar_t *zName, const wchar_t *zExtension)
{
	CString sPath(zPath);
	if (sPath.IsEmpty())
		return CString(zName);

	CString sLastChar(sPath.Right(1));
	if (sLastChar != "\\" && sLastChar != "/")
		sPath += "\\";

	sPath += zName;
	if (zExtension)
	{
		sPath += ".";
		sPath += zExtension;
	}
	return sPath;
}
CString CFileName::BuildDirNearFile(const wchar_t *zFileName, const wchar_t *zNewDirName)
{
	CFileName fName(zFileName);
	CString sPath(fName.Path());
	CString sNewFullName(BuildFileName(sPath,zNewDirName));
	if (CUtils::VerifyDirectory(sNewFullName))
		return sNewFullName;
	return CString(_T(""));
}
CString CFileName::ReplaceTopDirectoryName(const wchar_t * zFileName, const wchar_t * zNewDir)
{
	CFileName fName(zFileName);
	CString sPath = fName.Path();
	CFileName fPath(sPath);
	CString sPrePath = fPath.Path();
	sPath = sPrePath + zNewDir;
	sPath += "\\";
	CString sNewName = sPath + fName.Private();
	return sNewName;
}

bool CFileName::IsSubDir(const wchar_t * zPath, const wchar_t * zRoot)
{
	size_t rootLen = wcslen(zRoot);
	if (rootLen < 1)
		return true;

	if (wcslen(zPath) < rootLen)
		return false;

	return _wcsnicmp(zRoot,zPath, rootLen) == 0;
}
bool CFileName::GetNewFileName(const wchar_t *zName, int nTrials, CString &sNewName)
{
	if (!Exist(zName))
	{
		sNewName = zName;
		return true;
	}
	CFileName fName(zName);
	for (int i = 1; i <= nTrials; i++)
	{
		CString sTry(fName.GetIndexedName(i));
		if (!Exist(sTry))
		{
			sNewName = sTry;
			return true;
		}
	}
	return false;
}
/*
bool CFileName::LookForVariantOf(CString &sioName)
{
	CFileName fName(sioName);
	if (fName.Exist())
		return true;

	CString sPattern(GetVariantPattern(sioName));
	CString sName = CUtils::GetFirstFileName(sPattern);
	if (sName.IsEmpty())
		return false;

	sioName = sName;
	return true;
}*/
CString CFileName::GetVariantPattern(const wchar_t *zName)
{
	CString sName(zName);
	int iLast1 = sName.ReverseFind('/');
	int iLast2 = sName.ReverseFind('\\');
	int iStartPrivate = max(0, max(iLast1, iLast2) + 1);
	int iFirstPoint = sName.Find('.', iStartPrivate);
	if (iFirstPoint < 1)
	{
		return sName += "*";
	}
	CString sPattern = sName.Left(iFirstPoint);
	sPattern += "*";
	sPattern += sName.Right(sName.GetLength() - iFirstPoint);
	return sPattern;
}
/*
void CFileName::TestGetVariantPattern1(const wchar_t *zfName, CLogFile &log)
{
	CString sPattern(GetVariantPattern(zfName));
	if (log)
		fprintf(log.mpf,"%s --> %s\n", zfName, sPattern);
} */
/*
void CFileName::TestGetVariantPattern(void)
{
	CLogFile log("CFileName__TestGetVariantPattern");
	TestGetVariantPattern1("SimpleFileName",log);
	TestGetVariantPattern1("Name\\With\\Path",log);
	TestGetVariantPattern1("Name\\With\\PathAnd.Type",log);
	TestGetVariantPattern1("q:Name\\With\\PathAnd.Meta.Type",log);
	TestGetVariantPattern1("q:Name\\With\\PathAnd.Multi.Meta.Type",log);
	TestGetVariantPattern1("q:Name\\With\\S.hort.Multi.Meta.Type",log);
	TestGetVariantPattern1("s",log);
	TestGetVariantPattern1("s.t",log);
	TestGetVariantPattern1("s.p/s.t",log);
} */
bool CFileName::HasExtension (const wchar_t *zfName)
{
	CFileName fName(zfName);
	return fName.HasExtension();
}
bool CFileName::FindValidPath(CString &sioPath)
{
	if (CUtils::IsDirectory(sioPath))
		return true;

	CString s(sioPath);
	while(s.GetLength() > 2)
	{
		s = GetPath(s);
		if (CUtils::IsDirectory(s))
		{
			sioPath = s;
			return true;
		}
	}
	return false;
}
CString CFileName::CreateVariantOfPrivateName(const wchar_t *zfName, const wchar_t *zVar)
{
	CFileName fName(zfName);
	CString s(fName.PrivateWithoutType());
	s += "_";
	s += zVar;
	s += ".";
	s += fName.Type();
	return s;
}
CString CFileName::CreateVariantOfFullName(const wchar_t *zfName, const wchar_t *zVar)
{
	CFileName fName(zfName);
	CString s(fName.NameWithoutType());
	s += "_";
	s += zVar;
	s += ".";
	s += fName.Type();
	return s;
}
CString CFileName::GetNameValue(const wchar_t *zProperty, int value)
{
	CString s(_T("_"));
	s += zProperty;
	wchar_t zBuf[128];
	swprintf_s(zBuf,sizeof(zBuf),L"%d",value);
	s += zBuf;
	return s;
}
ULONG64 CFileName::FileLength(void)
{
	return GetFileLength(msName);
}
ULONG64 CFileName::GetFileLength(const wchar_t * zfName)
{
    ULONG64 length = 0;
    ULONG64 longUnit = (ULONG64)0x10000 * (ULONG64)0x10000;
    WIN32_FILE_ATTRIBUTE_DATA fileAttrib;

    BOOL bOK = GetFileAttributesEx (zfName, GetFileExInfoStandard, &fileAttrib);
    if (bOK)
    {
        length = fileAttrib.nFileSizeHigh * longUnit + fileAttrib.nFileSizeLow;
    }

    return length;
}
 CString CFileName::AddIndexToPivateName(const wchar_t * zfName, int i)
 {
	 if (i < 0)
		 return CString(zfName);

	 CFileName name(zfName);
	 return name.GetIndexedName(i);
 }
size_t CFileName::TypeLength (void) const
{
	return wcslen (Type());
}
size_t CFileName::Length (void) const
{
	return msName.GetLength();
}
