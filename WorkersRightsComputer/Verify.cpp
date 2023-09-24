#include "stdafx.h"
#include "Verify.h"
#include "Utils.h"
#include "Right.h"
#include "WorkersRightsComputerDlg.h"
#include "WorkPeriod.h"
#include "AllRights.h"
#include "FilesList.h"
#include "Pension.h"
#include "Saver.h"
#include "XmlParse.h"

CString CVerify::umsfName;
bool CVerify::umbBreakonDiff = false;
FILE* CVerify::umpfReport = NULL;
int CVerify::umiBatch = 0;
bool CVerify::umbDisplayDiff = false;

void CVerify::StartVerifyBatch(const wchar_t *zfName)
{
	umsfName = zfName;
	CUtils::CreateThread(StaticVerifyBatch, NULL);
}
DWORD WINAPI CVerify::StaticVerifyBatch(LPVOID)
{
	CString sPath = CFileName::GetPath(umsfName);
	sPath = CFileName::GetPath(sPath);

	CString sLogFileName(sPath + L"VerifyBatch.log");
	FILE *pfLog = MyFOpenWithErrorBox(sLogFileName, L"w", L"Log");
	if (!pfLog)
		return 0;

	CString sReportFileName(sPath + L"VerifyBatchReport.csv");
	umpfReport = MyFOpenWithErrorBox(sReportFileName, L"w", L"Log");

	CFilesList list;
	CUtils::ListFilesInDirRecursive(sPath, L"*.xml", list);

	umiBatch = 0;
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CString *psfName = list.GetNext(pos);
		CString sPrivate = CFileName::GetPrivate((const wchar_t *)*psfName);
		if (sPrivate == L"Save.txt" || sPrivate.Right(8) == "save.xml")
		{
			umiBatch++;
			if (umpfReport)
				fprintf(umpfReport, "%d, ", umiBatch);
			CVerify* pVerify = NULL;
			pVerify = new CVerifyNew((const wchar_t *)*psfName, true);
			bool bSame = pVerify->Verify();
			pVerify->WriteSummary(pfLog);

			if (umpfReport)
			{
				CString sCurPath = CFileName::GetPath((const wchar_t *)*psfName);
				CString sLast = CFileName::GetPrivate(sCurPath);
				fwprintf(umpfReport, L"%s\n", (const wchar_t *)sLast);
				fflush(umpfReport);
			}
			delete pVerify;
			if (!bSame && umbBreakonDiff)
				break;
		}
	}
	fclose(pfLog);
	if (umpfReport)
	{
		fclose(umpfReport);
		umpfReport = NULL;
	}
	CUtils::OpenTextFile(sLogFileName);
	return 0;
}

CVerify::CVerify(const wchar_t *zfName, bool bSilent)
	: msfName(zfName)
	, mbSilentMode(bSilent)
	, mbAllSame(false)
	, mnCompared(0)
	, mnSame(0)
	, mnDiff(0)
	, mnMissing(0)
	, mDebug(3)
	, mpfLog(NULL)
{
	mpfLog = CUtils::OpenLogFile(L"Verify");
	if (mpfLog)
		fwprintf(mpfLog, L"Input file: %s\n\n", zfName);
}
CVerify::~CVerify()
{
	Clear();
	if (mpfLog)
		fclose(mpfLog);
}
bool CVerify::Verify()
{
	if (!ReadSavedFile())
		return false;

	gpDlg->OnInputChange();
	mbAllSame = VerifyResults();
	return mbAllSame;
}
void CVerify::ReadTime(FILE *pfRead, int i)
{
	int year = CUtils::ReadInt(pfRead);
	int month = CUtils::ReadInt(pfRead);
	int day = CUtils::ReadInt(pfRead);
	CTime time(year, month, day, 0, 0, 0, 0);
	switch (i)
	{
	case 1:
		gWorkPeriod.mFirst.Set(time);
		break;
	case 2:
		gWorkPeriod.mLast.Set(time);
		break;
	case 3:
		gWorkPeriod.mNotice.Set(time);
		if (mpfLog)
		{
			CTimeSpan span = gWorkPeriod.mLast.mTime - gWorkPeriod.mNotice.mTime;
			CString s(L"Last - Notice ");
			s += L" TIME SPAN days ";
			s += CRight::ToString(span.GetDays());
			s += L" houres ";
			s += CRight::ToString(span.GetHours());
			s += L" minutes ";
			s += CRight::ToString(span.GetMinutes());
			fwprintf(mpfLog, L"%s\n\n", (const wchar_t *)s);
		}
		break;
	default:
		CUtils::MessBox(L"<CVerify::ReadTime> bad time index", L"SW Error");
	}
	mnTimesDefined++;
}
int CVerify::ReadResults(FILE *pfRead)
{
	Clear();
	CString sLine = CUtils::ReadLine(pfRead);
	while (sLine != L"<CRights::Save>")
	{
		if (sLine.IsEmpty())
			return 0;
		sLine = CUtils::ReadLine(pfRead);
	}

	sLine = CUtils::ReadLine(pfRead);
	while (!sLine.IsEmpty() && sLine != L"end")
	{
		CRightResult *pResult = new CRightResult(pfRead, sLine);
		if (pResult->msName.IsEmpty())
		{
			delete pResult;
			break;
		}
		else
			mResults.AddTail(pResult);
		sLine = CUtils::ReadLine(pfRead);
	}
	return (int)mResults.GetSize();
}
CRightResult::CRightResult(FILE *pfRead, const wchar_t *zName)
	: msName(zName)
	, mDue(0)
{
	CString sLine = CUtils::ReadLine(pfRead);
	if (msName == "Additional")
	{
		int nNumeric = 0;
		int iNumeric = sLine.GetLength();
		while (iNumeric > 0)
		{
			wchar_t sCheck = sLine[iNumeric - 1];
			if (isdigit(sCheck) || sCheck == '.')
			{
				iNumeric--;
				nNumeric++;
			}
			else
				break;
		}
		if (nNumeric > 0)
		{
			CString sSum = sLine.Right(nNumeric);
			mDue = _wtof(sSum);
		}
	}
	else
	{
		int iArrow = 0;
		if (msName == "Pension")
			iArrow = sLine.Find(L"Due");
		else
			iArrow = sLine.Find(L"==>");

		if (iArrow > 0)
		{
			CString sRest = sLine.Right(sLine.GetLength() - (iArrow + 4));
			mDue = _wtof(sRest);
		}
	}
	while (sLine != L"*")
	{
		sLine = CUtils::ReadLine(pfRead);
	}
}
CRightResult::CRightResult(CXMLParseNode* pNode)
	: msName(pNode->msName)
	, mDue(0)
{
	pNode->GetValue(L"Due", mDue);
}
void CVerify::Clear()
{
	while (!mResults.IsEmpty())
	{
		delete mResults.GetTail();
		mResults.RemoveTail();
	}
}
bool CVerify::VerifyResults()
{
	CString s(L"<VerifyResults>");
	mnCompared = 0;
	mnSame = 0;
	mnDiff = 0;
	mnMissing = 0;

	double oldSum = 0;
	double newSum = 0;

	POSITION pos = gAllRights.mRights.GetHeadPosition();
	while (pos)
	{
		CRight *pRight = gAllRights.mRights.GetNext(pos);
		s += L"\r\n";
		s += pRight->msName;
		s += L" ";
		s += CRight::ToString(pRight->mDuePay);
		newSum += pRight->mDuePay;
		mnCompared++;

		bool bFound = false;
		POSITION posRes = mResults.GetHeadPosition();
		while (posRes)
		{
			CRightResult *pResult = mResults.GetNext(posRes);
			if (pResult->msName == pRight->msName)
			{
				oldSum += pResult->mDue;
				bFound = true;
				if (umpfReport)
					fwprintf(umpfReport, L"%s, %.2f, ", (const wchar_t *)pResult->msName, pRight->mDuePay);
				if ((pRight->mDuePay >= (pResult->mDue - 0.2)) && (pRight->mDuePay <= (pResult->mDue + 0.2)))
				{
					s += L" Same";
					mnSame++;
					if (umpfReport)
						fprintf(umpfReport, "0, ");
				}
				else
				{
					s += L" Diff, old =  ";
					s += CRight::ToString(pResult->mDue);
					mnDiff++;
					if (umpfReport)
					{
						double diff = pRight->mDuePay - pResult->mDue;
						fprintf(umpfReport, "%d, ", (int)diff);
					}
				}
				break;
			}
		}
		if (!bFound)
		{
			s += L" No saved result found";
			mnMissing++;
			if (umpfReport)
				fprintf(umpfReport, "miss, ");
		}
	}
	if (!umpfReport || umbDisplayDiff)
	{
		if (!mbSilentMode || mnSame != mnCompared )
			CUtils::MessBox(s, L"Verify Results");
	}
	if (umpfReport)
	{
		double diff = newSum - oldSum;
		fprintf(umpfReport, "%.2f, ", diff);
		if (mnSame == mnCompared)
			fprintf(umpfReport, "same, ");
		else
			fprintf(umpfReport, "diff, ");
	}
	return mnSame == mnCompared;
}
void CVerify::CheckIfNoticeSet()
{
	CString sfName = GetLogFileNameFor(L"Notice");

	FILE *pfRead = MyFOpenWithErrorBox(sfName, L"r", L"Old Holidays Log");
	if (!pfRead)
		return;

	CString sLine = CUtils::ReadLine(pfRead);
	while (!sLine.IsEmpty())
	{
		if (sLine == L"Notice Date not set!")
		{
			gWorkPeriod.mNotice.mbInitialized = false;
			break;
		}
		sLine = CUtils::TryReadLine(pfRead);
	}
	fclose(pfRead);
}
void CVerify::ReadHolidaysDefinitions()
{
	CFilesList list;
	CUtils::ListFilesInDir(CUtils::GetBaseDir() + L"input\\holidays", L"txt", list);
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CString *psName = list.GetNext(pos);
		if (CUtils::FileExists(*psName))
		{
			CHolidaysDef *def = new CHolidaysDef((const wchar_t *)*psName);
			mHolidaysDefs.AddTail(def);
		}
	}
}
CHolidaysDef::CHolidaysDef(const wchar_t *zfName)
	: msfName(zfName)
{
	FILE *pf = MyFOpenWithErrorBox(zfName, L"r", L"Holidays Def");
	if (!pf)
		return;

	FILE *pfLog = CUtils::TryOpenLogFile(L"HolidaysDef", L"read");
	if (pfLog)
		fwprintf(pfLog, L"%s\n", zfName);

	CString sLine = CUtils::ReadLine(pf);
	if (pfLog)
		fwprintf(pfLog, L"%s\n", (const wchar_t *)sLine);
	while (!sLine.IsEmpty() && sLine.Left(3) != L"end")
	{
		if (!isdigit(sLine[0]))
			mNames.AddTail(new CString(sLine));
		else if (sLine.Left(3) == L"2nd")
			mNames.AddTail(new CString(sLine));

		sLine = CUtils::ReadLine(pf);
		if (pfLog)
		{
			fwprintf(pfLog, L"%s\n", (const wchar_t *)sLine);
			fflush(pfLog);
		}
	}
	if (pfLog)
		fclose(pfLog);
	fclose(pf);
}
bool CHolidaysDef::Includes(CList<CString *, CString *> &usedHolidays, FILE *pfLog)
{
	if (pfLog)
		fwprintf(pfLog, L"<CHolidaysDef::Includes> check: %s\n", (const wchar_t *)msfName);

	POSITION pos = usedHolidays.GetHeadPosition();
	while (pos)
	{
		CString *psUsedName = usedHolidays.GetNext(pos);
		int lenUsed = psUsedName->GetLength();

		bool bFound = false;
		POSITION myPos = mNames.GetHeadPosition();
		while (myPos)
		{
			CString *psName = mNames.GetNext(myPos);
			if (psName->GetLength() > lenUsed)
			{
				if (psName->Left(lenUsed) == *psUsedName)
				{
					bFound = true;
					if (pfLog)
						fwprintf(pfLog, L"<CHolidaysDef::Includes> found: %s\n", (const wchar_t *)*psUsedName);
					break;
				}
			}
			if (*psName == *psUsedName)
			{
				bFound = true;
				if (pfLog)
					fwprintf(pfLog, L"<CHolidaysDef::Includes> found: %s\n", (const wchar_t *)*psUsedName);
				break;
			}
		}
		if (!bFound)
		{
			if (pfLog)
				fwprintf(pfLog, L"<CHolidaysDef::Includes> missing: %s\n", (const wchar_t *)*psUsedName);

			return false;
		}
	}
	return true;
}
void CHolidaysDef::Log(FILE *pfLog)
{
	fwprintf(pfLog, L"FILE: %s\n", (const wchar_t *)msfName);
	POSITION pos = mNames.GetHeadPosition();
	while (pos)
	{
		CString *psName = mNames.GetNext(pos);
		fwprintf(pfLog, L"%s\n", (const wchar_t *)*psName);
	}
	fwprintf(pfLog, L"\n");
}
CString CVerify::SelectHolidays()
{
	ReadHolidaysDefinitions();

	if (mDebug)
	{
		POSITION pos = mHolidaysDefs.GetHeadPosition();
		while (pos)
		{
			CHolidaysDef *pDef = mHolidaysDefs.GetNext(pos);
			pDef->Log(mpfLog);
		}
	}

	if (!ReadCurrentHolidaysNames())
		return CString(L"None Yet");

	POSITION pos = mHolidaysDefs.GetHeadPosition();
	while (pos)
	{
		CHolidaysDef *pDef = mHolidaysDefs.GetNext(pos);
		if (pDef->Includes(mUsedHolidays, mpfLog))
		{
			CFileName fName(pDef->msfName);
			CString sPrivate(fName.PrivateWithoutType());
			if (sPrivate.Right(8) == L"Holidays")
				sPrivate = sPrivate.Left(sPrivate.GetLength() - 8);
			if (mDebug & 0x20)
				fwprintf(mpfLog, L"<SelectHolidays> FOUND %s\n\n", (const wchar_t *)sPrivate);
			return sPrivate;
		}
	}
	if (mDebug & 0x20)
		fprintf(mpfLog, "<SelectHolidays> No proper definition\n\n");
	return CString(L"No proper definition");
}

CString CVerify::GetLogFileNameFor(const wchar_t *zRight)
{
	CFileName fName(msfName);
	CString sfName = fName.Path();
	sfName += msFirstName;
	sfName += L"_";
	sfName += msFamilyName;
	sfName += L"_";
	sfName += zRight;
	sfName += L".log";
	return sfName;
}
bool CVerify::ReadCurrentHolidaysNames(void)
{
	CString sfName = GetLogFileNameFor(L"Holidays");

	FILE *pfRead = MyFOpenWithErrorBox(sfName, L"r", L"Old Holidays Log");
	if (!pfRead)
		return false;

	int n = 0;
	CString sLine = CUtils::ReadLine(pfRead);
	while (!sLine.IsEmpty())
	{
		if (sLine == L"Computing Holidays")
			;
		else if (sLine.Left(2) == L"n ")
			;
		else if (sLine.Left(13) == L"Pay Last Year")
			break;
		else if (sLine.Left(19) == L"ComputePayPrevYears")
			break;
		else
		{
			CString sLast = sLine.Right(1);
			while (isdigit(sLast[0]) || sLast == L" " || sLast == L".")
			{
				sLine = sLine.Left(sLine.GetLength() - 1);
				sLast = sLine.Right(1);
			}
			mUsedHolidays.AddTail(new CString(sLine));
			if (mpfLog)
				fwprintf(mpfLog, L"Used Holidays %2d: %s\n", ++n, (const wchar_t *)sLine);
		}
		sLine = CUtils::ReadLine(pfRead);
	}
	fclose(pfRead);
	if (mpfLog)
		fwprintf(mpfLog, L"\n");
	return true;
}
void CVerify::WriteSummary(FILE *pf)
{
	fwprintf(pf, L"%80s", (const wchar_t *)msfName);
	if (mnCompared < 1)
		fwprintf(pf, L" - No rights computed");
	else if (mnCompared == mnSame)
		fwprintf(pf, L" - All %d same", mnCompared);
	else if (mnDiff > 0)
		fwprintf(pf, L" - %d / %d differ", mnDiff, mnCompared);

	if (mnMissing > 0)
		fwprintf(pf, L" - %d / %d missing", mnMissing, mnCompared);

	fwprintf(pf, L"\n");
	fflush(pf);

	CString s(msfName + L" ");
	if (mnCompared == mnSame)
		s += "All Same";
	else if (mnDiff > 0)
		s += "Differ";
	else if (mnMissing > 0)
		s += " Missing";

	gpDlg->DisplaySummary(s);
}

//===================================================================================

CVerifyOld::CVerifyOld(const wchar_t* zfName, bool bSilent)
	: CVerify(zfName, bSilent)
{
}
CVerifyOld::~CVerifyOld()
{
}
bool CVerifyOld::ReadSavedFile()
{
	FILE* pfRead = MyFOpenWithErrorBox(msfName, L"r", L"Old Saved File");
	if (!pfRead)
		return false;

	mnTimesDefined = 0;
	gpDlg->OnBnClickedButtonReset();
	gpDlg->mbDisableComputations = true;
	int nEditsRead = 0;
	int nCheckRead = 0;
	int nChecked = 0;

	// Read Text Boxes
	bool bEndFound = false;
	CString sKey = CUtils::ReadLine(pfRead);
	while (!sKey.IsEmpty() && sKey != L"end" && !bEndFound)
	{
		// Look for known boxes old names
		POSITION pos = gpDlg->mEditBoxes.GetHeadPosition();
		while (pos)
		{
			CEditRef* pRef = gpDlg->mEditBoxes.GetNext(pos);
			if (pRef->msOldName == sKey)
			{
				nEditsRead++;
				CString sValue = CUtils::ReadNextLine(pfRead);
				pRef->mEdit.SetWindowTextW(sValue);
				if (pRef->msOldName == L"textBox1")
					msFirstName = sValue;
				else if (pRef->msOldName == L"textBox2")
					msFamilyName = sValue;
				if (mpfLog)
					fwprintf(mpfLog, L"<Read> Set %s %s to %s\n", (const wchar_t*)pRef->msName,
					(const wchar_t*)pRef->msOldName, (const wchar_t*)sValue);
				if (sValue.Left(3) == L"end")
					bEndFound = true;
			}
		}
		if (!bEndFound)
			sKey = CUtils::ReadLine(pfRead);
	}

	// Read Work Period
	sKey = CUtils::ReadLine(pfRead);
	while (!sKey.IsEmpty() && sKey != L"end")
	{
		if (sKey == L"dateTimePicker1")
			ReadTime(pfRead, 1);
		else if (sKey == L"dateTimePicker2")
			ReadTime(pfRead, 2);
		else if (sKey == L"dateTimePicker3")
			ReadTime(pfRead, 3);
		sKey = CUtils::ReadLine(pfRead);
	}

	// Read Checkboxes
	sKey = CUtils::ReadLine(pfRead);
	while (!sKey.IsEmpty() && sKey != L"end")
	{
		// Look for known boxes old names
		bool bFound = false;
		POSITION pos = gpDlg->mButtons.GetHeadPosition();
		while (pos)
		{
			CButtonRef* pRef = gpDlg->mButtons.GetNext(pos);
			if (pRef->msOldName == sKey)
			{
				nCheckRead++;
				int value = CUtils::ReadInt(pfRead);
				if (value)
				{
					pRef->mButton.SetCheck(BST_CHECKED);
					nChecked++;
				}
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			if (sKey.Left(8) == L"checkBox")
			{
				CString sRest = sKey.Right(sKey.GetLength() - 8);
				int iDay = _wtoi(sRest);
				if (iDay >= 4 && iDay <= 10)
				{
					nCheckRead++;
					int value = CUtils::ReadInt(pfRead);
					if (value)
					{
						nChecked++;
						gWorkPeriod.SetWorkingDay(iDay - 4, 1);
					}
				}
			}
		}
		sKey = CUtils::ReadLine(pfRead);
	}
	int nResults = ReadResults(pfRead);

	CString s(CRight::ToString(nEditsRead));
	s += L" edit boxes,  ";
	s += CRight::ToString(mnTimesDefined);
	s += L" times,  ";
	s += CRight::ToString(nChecked);
	s += L" / ";
	s += CRight::ToString(nCheckRead);
	s += L" checked, nResults ";
	s += CRight::ToString(nResults);

	CString sHolidays = SelectHolidays();
	gpDlg->mComboHolidays.SetWindowTextW(sHolidays);
	s += L"\r\nHolidays: ";
	s += sHolidays;

	if (!mbSilentMode)
		CUtils::MessBox(s, L"Old Save Read");

	CheckIfNoticeSet();
	gpDlg->mbDisableComputations = false;
	if (pfRead)
		fclose(pfRead);
	return true;
}

//===================================================================================

CVerifyNew::CVerifyNew(const wchar_t* zfName, bool bSilent)
	: CVerify(zfName, bSilent)
{
}

CVerifyNew::~CVerifyNew()
{
}
bool CVerifyNew::ReadSavedFile()
{
	CSaver saver;
	if (!saver.Restore(msfName))
		return false;

	CXMLParse XMLParse(msfName, true);
	CXMLParseNode* pRoot = XMLParse.GetRoot();
	if (!pRoot)
		return false;

	CXMLParseNode* pRights = pRoot->GetFirst(L"AllRightsAsComputed");
	if (!pRights)
		return false;

	POSITION pos = pRights->GetHeadPosition();
	while (pos)
	{
		CXMLParseNode* pRight = pRights->GetNext(pos);
		CRightResult* pResult = new CRightResult(pRight);
		mResults.AddTail(pResult);
	}

	return true;
}