#include "stdafx.h"
#include "UsedVacations.h"
#include "Right.h"
#include "Utils.h"
#include "VacationsDlg.h"
#include "VacationUsed.h"
#include "VacationTable.h"

CUsedVacations gUsedVacations;

CUsedVacations::CUsedVacations()
	: mbAdd14DaysUnpaidVacation4Severance(false)
{
}
CUsedVacations::~CUsedVacations()
{
	ClearAllVacations();
}
void CUsedVacations::AddVacation(class CVacationUsed *pNewVacation)
{
	bool bCopyRest = false;
	CVacationUsed *pPrevVac = NULL;
	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		POSITION posPrev = pos;
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);

		if (bCopyRest)
		{
			mVacationsUsed.SetAt(posPrev, pPrevVac);
		}
		else
		{
			if (pNewVacation->mFirstDay < pVac->mFirstDay)
			{
				if (pNewVacation->mLastDay >= pVac->mFirstDay)
				{
					CUtils::MessBox(L"Vacation overlaps with previously defined vacation", L"Input Error");
					delete pNewVacation;
					return;
				}
				if (pPrevVac && pNewVacation->mFirstDay <= pPrevVac->mLastDay)
				{
					CUtils::MessBox(L"Vacation overlaps with previously defined vacation", L"Input Error");
					delete pNewVacation;
					return;
				}
				mVacationsUsed.SetAt(posPrev, pNewVacation);
				bCopyRest = true;
			}
		}

		pPrevVac = pVac;
	}
	if (bCopyRest)
		mVacationsUsed.AddTail(pPrevVac);
	else
	{
		if (pPrevVac && pNewVacation->mFirstDay <= pPrevVac->mLastDay)
		{
			CUtils::MessBox(L"Vacation overlaps with previously defined vacation", L"Input Error");
			delete pNewVacation;
			return;
		}

		mVacationsUsed.AddTail(pNewVacation);
	}
}
void CUsedVacations::ClearLastVacation(void)
{
	if (mVacationsUsed.GetSize() > 0)
	{
		delete mVacationsUsed.GetTail();
		mVacationsUsed.RemoveTail();
	}
}
void CUsedVacations::ClearAllVacations(void)
{
	while (mVacationsUsed.GetSize() > 0)
		ClearLastVacation();

	mbAdd14DaysUnpaidVacation4Severance = false;
}
CString CUsedVacations::GetVacationsShortText(void)
{
	int n = (int)mVacationsUsed.GetSize();
	if (n > 0)
	{
		Compute();
		CString s(CRight::ToString(n));
		s += L" vacations reported";
		return s;
	}
	return CString(L"No Vacations Reported");
}
CString CUsedVacations::GetVacationsFullText(void)
{
	if (mVacationsUsed.GetSize() < 1)
		return CString(L"No Vacations Reported");

	Compute();
	CString s(gWorkPeriod.GetShortSummary());
	s += "\r\n";
	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		s += pVac->GetText();
		s += L"\r\n";
	}
	return s;
}
void CUsedVacations::Save(FILE *pfSave)
{
	if (mVacationsUsed.GetSize() < 1)
		return;

	fwprintf(pfSave, L"Vacations\n");

	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		fwprintf(pfSave, L"Vacation\n");
		pVac->mFirstDay.Write(pfSave);
		pVac->mLastDay.Write(pfSave);
	}
	if (mbAdd14DaysUnpaidVacation4Severance)
		fprintf(pfSave, "mbAdd14DaysUnpaidVacation4Severance\n");

	fwprintf(pfSave, L"EndVacations\n");

}
void CUsedVacations::Restore(FILE *pfRead)
{
	CString s = CUtils::ReadLine(pfRead);
	while (s == "Vacation")
	{
		CMyTime mFirst;
		CMyTime mLast;
		mFirst.Read(pfRead);
		mLast.Read(pfRead);
		CVacationUsed *pVac = new CVacationUsed(mFirst, mLast);
		AddVacation(pVac);
		s = CUtils::ReadLine(pfRead);
	}
	if (s == "mbAdd14DaysUnpaidVacation4Severance")
	{
		mbAdd14DaysUnpaidVacation4Severance = true;
		s = CUtils::ReadLine(pfRead);
	}
	Compute();
}
void CUsedVacations::Compute()
{
	if (!gWorkPeriod.IsValid())
		return;

	gWorkPeriod.InitDetailsForEachMonth();

	gVacationTable.StartComputingForUsedVacations();

	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		pVac->Compute();
	}
}
void CUsedVacations::UpdateNextYearStart(CMyTime &yearStart, CMyTime &nextYearStart)
{
	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		if (pVac->mnUnPaid > 0)
		{
			if (pVac->mFirstDayUnpaid >= yearStart && pVac->mFirstDayUnpaid < nextYearStart)
				nextYearStart.AddDays(pVac->mnUnpaidCalendarDays);
		}
	}
}
void CUsedVacations::Log()
{
	FILE *pfLog = CUtils::OpenLogFile(L"UsedVacations");
	if (!pfLog)
		return;
	if (mVacationsUsed.GetSize() < 1)
	{
		fprintf(pfLog, "No vacations defined\n");
	}
	else
	{
		int count = 1;
		POSITION pos = mVacationsUsed.GetHeadPosition();
		while (pos)
		{
			CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
			fwprintf (pfLog, L"%d: ", count++);
			pVac->LongLog(pfLog);
		}
		if (mbAdd14DaysUnpaidVacation4Severance)
			fprintf(pfLog, "mbAdd14DaysUnpaidVacation4Severance\n");
	}
	fclose(pfLog);
}
