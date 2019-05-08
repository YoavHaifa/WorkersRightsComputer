#include "stdafx.h"
#include "FilesList.h"
#include "FileName.h"
#include "Utils.h"

CString CFilesList::umsDefaultSaveDir(L"");
CString CFilesList::umsValuesFormat(L"%12.3f ");

void CFilesList::SetDefaultSaveDir(const wchar_t *zDir)
{
	umsDefaultSaveDir = zDir;
}
void CFilesList::SetValuesPrintFormat(const wchar_t *zFormat)
{
	umsValuesFormat = zFormat;
}

CFilesList::CFilesList(const wchar_t *zfName)
	: msListName(zfName ? zfName : L"FilesList")
{
	if (zfName)
		InitOnConstruct();
}
CFilesList::~CFilesList(void)
{
	ClearAll();
}
void CFilesList::InitOnConstruct(void)
{
	CFileName fName(msListName);
	if (!fName.Exist())
	{
		if (umsDefaultSaveDir.IsEmpty())
			return;
		fName.ChangePath(umsDefaultSaveDir);
		if (!fName.Exist())
			return;
	}
	AddNamesFromFile(fName);
}
bool CFilesList::Init(const wchar_t *zListName, int nVerify)
{
	if (zListName)
		msListName = zListName;
	CFileName fName(msListName);
	if (!fName.Exist())
	{
		if (umsDefaultSaveDir.IsEmpty())
			return false;
		fName.ChangePath(umsDefaultSaveDir);
		if (!fName.Exist())
			return false;
	}
	AddNamesFromFile(fName);

	if (nVerify)
	{
		if (N() != nVerify)
		{
			wchar_t zBuf[256];
			swprintf_s(zBuf,sizeof(zBuf)/sizeof(zBuf[0]),L"ERROR: %d files in list != %d expected", N(), nVerify);
			return false;
		}
	}
	return true;
}
void CFilesList::Add(const wchar_t *zfName)
{
	mList.AddTail(new CString(zfName));
}
CString CFilesList::GetHead(void)const
{
	if (mList.IsEmpty())
		return CString(_T(""));
	return CString(*mList.GetHead());
}
CString CFilesList::GetAt(int i)const
{
	POSITION pos = mList.GetHeadPosition();
	int count = 0;
	while (pos)
	{
		CString *ps = mList.GetNext(pos);
		if (count == i)
			return CString(*ps);
		count++;
	}
			
	return CString(_T(""));
}
int CFilesList::AddNamesFromFile(const wchar_t *zfName)
{
	FILE *pf = MyFOpenWithErrorBox(zfName,L"r", L"Files List");
	if (!pf)
		return 0;

	int nAdded = 0;
	wchar_t zBuf[256];
	while (1)
	{
		size_t nRead = fwscanf_s(pf,L"%s",zBuf,sizeof(zBuf));
		if (nRead == 1)
		{
			Add(zBuf);
			nAdded++;
		}
		else
			break;
	}

	return nAdded;
}
void CFilesList::AddWithFactor(const wchar_t *zfName, double factor)
{
	mList.AddTail(new CString(zfName));
	mFactorsList.AddTail(factor);
}
int CFilesList::N(void) const
{
	if (mList.IsEmpty())
		return 0;
	return (int)mList.GetSize();
}
int CFilesList::Invert(const CFilesList &other)
{
	if (other.IsEmpty())
		return 0;

	POSITION pos = other.mList.GetTailPosition();
	while (pos)
	{
		CString *ps = other.mList.GetPrev(pos);
		Add(*ps);
	}
	return N();
}
bool CFilesList::DeleteFileName(POSITION posToRemove)
{
	POSITION pos = mList.GetHeadPosition();
	POSITION posFact = NULL;
	if (!mFactorsList.IsEmpty())
		posFact = mFactorsList.GetHeadPosition();
	while (pos)
	{
		if (pos == posToRemove)
		{
			delete mList.GetAt(pos);
			mList.RemoveAt(pos);
			if (!mFactorsList.IsEmpty())
				mFactorsList.RemoveAt(posFact);
			return true;
		}
		mList.GetNext(pos);
		if (!mFactorsList.IsEmpty())
			mFactorsList.GetNext(posFact);
	}
	return false;
}
void CFilesList::ConvertToPrivateWithoutExtension()
{
	POSITION pos = mList.GetHeadPosition();
	while (pos)
	{
		CString *ps = mList.GetNext(pos);
		*ps = CFileName::GetPrivateWithoutType(*ps);
	}
}
void CFilesList::ComputePrivatePrefixNumber(void)
{
	POSITION pos = mList.GetHeadPosition();
	POSITION posFact = mFactorsList.GetHeadPosition();
	while (pos)
	{
		CString *ps = mList.GetNext(pos);
		CString s = CFileName::GetPrivate(*ps);
		double dn = (double)_wtoi(s);
		if (posFact)
		{
			mFactorsList.SetAt(posFact, dn);
			mFactorsList.GetNext(posFact);
		}
		else
			mFactorsList.AddTail(dn);
	}
}
bool CFilesList::Includes(const wchar_t *zfName)
{
	CString sCheck(zfName);
	POSITION pos = mList.GetHeadPosition();
	while (pos)
	{
		CString *ps = mList.GetNext(pos);
		if (*ps == sCheck)
			return true;
	}
	return false;
}
void CFilesList::ClearAll(void)
{
	while(!mList.IsEmpty())
	{
		delete mList.GetTail();
		mList.RemoveTail();
	}
	while(!mFactorsList.IsEmpty())
	{
		mFactorsList.RemoveTail();
	}
}
/*
void CFilesList::DeleteAllFilesAndClearList(FILE *pfLog)
{
	if (pfLog)
		fprintf(pfLog,"<CFilesList::DeleteAllFilesAndClearList> %d files\n", N());
	int nDeleted = 0;
	POSITION pos = mList.GetHeadPosition();
	while (pos)
	{
		CString *ps = mList.GetNext(pos);
		bool bOK = CMyWindows::Delete1File(*ps);
		if (bOK)
			nDeleted++;

		if (pfLog)
		{
			if (bOK)
				fprintf(pfLog,"%s - Deleted\n", *ps);
			else
				fprintf(pfLog,"%s - Delete Failed!\n", *ps);

		}
	}
	if (pfLog)
		fprintf(pfLog,"<CFilesList::DeleteAllFilesAndClearList> %d/%d files deleted\n", nDeleted, N());
	ClearAll();
}*/
void CFilesList::PrintWithValues(FILE *pf, const wchar_t *zTitle, bool bIndex)
{
	Print(pf, zTitle, bIndex, true);
}
void CFilesList::Print(FILE *pf, const wchar_t *zTitle, bool bIndex, bool bValues)
{
	if (!pf)
		return;
	if (zTitle)
		fwprintf(pf,L"<CFilesList::Print> %s\n", zTitle);

	POSITION pos = mList.GetHeadPosition();
	POSITION posFact = NULL;
	if (bValues)
		posFact = mFactorsList.GetHeadPosition();

	int i = 1;
	while (pos)
	{
		CString *ps = mList.GetNext(pos);
		if (bIndex)
			fprintf(pf, "%3d: ", i++);
		if (bValues)
		{
			double value = mFactorsList.GetNext(posFact);
			fwprintf(pf, umsValuesFormat, value);
		}
		fwprintf(pf, L"%s", (const wchar_t *)*ps);
		fwprintf(pf, L"\n");
	}
	fprintf(pf,"\n");
}
bool CFilesList::Print(const wchar_t *zfName, const wchar_t *zTitle, bool bIndex)
{
	FILE *pf = MyFOpenWithErrorBox(zfName, L"w", L"Files List");
	if (!pf)
		return false;

	Print(pf, zTitle, bIndex);
	fclose(pf);
	return true;
}
bool CFilesList::Save(const wchar_t *zListName)
{
	if (zListName)
		msListName = zListName;

	CFileName fName(msListName);
	if (!umsDefaultSaveDir.IsEmpty())
		fName.ChangePath(umsDefaultSaveDir);
	return Print(fName, NULL, false);
}
int CFilesList::PurgeNonDir(void)
{
	bool bAllDir = false;
	while (!bAllDir && !mList.IsEmpty())
	{
		bAllDir = true;
		POSITION pos = mList.GetHeadPosition();
		while (pos)
		{
			POSITION prevPos = pos;
			CString *psfName = mList.GetNext(pos);
			if (!CUtils::IsDirectory(*psfName))
			{
				DeleteFileName(prevPos);
				bAllDir = false;
				break;
			}
		}
	}
	return (int)mList.GetSize();
}
CString CFilesList::GetBiggest(void)
{
	if (mList.GetSize() != mFactorsList.GetSize())
	{
		CUtils::MessBox(L"<CFilesList::GetBiggest> values not sync with names",L"SW Error");
		return CString(L"");
	}

	if (mList.IsEmpty())
		return CString(L"");

	CString sBiggest = *(mList.GetHead());
	double biggestValue = mFactorsList.GetHead();

	POSITION pos = mList.GetHeadPosition();
	POSITION posFact = mFactorsList.GetHeadPosition();
	while (pos)
	{
		CString *psfName = mList.GetNext(pos);
		double value = mFactorsList.GetNext(posFact);
		if (value > biggestValue)
		{
			biggestValue = value;
			sBiggest = *psfName;
		}
	}
	return sBiggest;
}
