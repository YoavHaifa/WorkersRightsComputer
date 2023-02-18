#include "StdAfx.h"
#include "Holidays.h"
#include "WorkPeriod.h"
#include "WageTable.h"
#include "Utils.h"
#include "WorkYears.h"
#include "WorkersRightsComputerDlg.h"
#include "HolidaysDue.h"

void CHoliday::Log(FILE *pf)
{
	if (!pf)
		return;
	fwprintf(pf, L"%s m %d d %d\n", (const wchar_t *)msName, mMonth, mDay);
}

CHolidays::CHolidays(void)
	: CRight(L"Holidays", L"חגים")
	, mpNDaysInLastYearBox(NULL)
	, mpNDaysWorkedLastYearBox(NULL)
	, mpNDaysPaidLastYearBox(NULL)
	, mpNDaysWorkedPrevYearsBox(NULL)
	, mpNDaysPaidPrevYearsBox(NULL)
	, mpPrevYearsFromBox(NULL)
	, mpPrevNYearsBox(NULL)
	//, mpPayRatePerHolidayBox(NULL)
	//, mRateSetByUser(0)
{
	miPrintOrder = 4;
	//mnWorkedLastYear = -1;
	//mnPaidLastYear = -1;

	for (int i = 0; i < MAX_HOLIDAYS_DEFINED; i++)
		map[i] = NULL;
}
bool CHolidays::SetEditRef(class CEditRef *pRef)
{
	if (pRef->msName == "LastYearWork")
	{
		mpNDaysWorkedLastYearBox = &pRef->mEdit;
		return true;
	}
	if (pRef->msName == "LastYearPaid")
	{
		mpNDaysPaidLastYearBox = &pRef->mEdit;
		return true;
	}
	if (pRef->msName == "LastYearFrom")
	{
		mpNDaysInLastYearBox = &pRef->mEdit;
		return true;
	}
	if (pRef->msName == "PrevYearWork")
	{
		mpNDaysWorkedPrevYearsBox = &pRef->mEdit;
		return true;
	}
	if (pRef->msName == "PrevYearPaid")
	{
		mpNDaysPaidPrevYearsBox = &pRef->mEdit;
		return true;
	}
	if (pRef->msName == "PrevYearFrom")
	{
		mpPrevYearsFromBox = &pRef->mEdit;
		return true;
	}
	if (pRef->msName == "PrevNYear")
	{
		mpPrevNYearsBox = &pRef->mEdit;
		return true;
	}
	/*
	if (pRef->msName == "PayRatePerHoliday")
	{
		mpPayRatePerHolidayBox = &pRef->mEdit;
		return true;
	}*/

	return false;
}
bool CHolidays::InitFromFile(const wchar_t *zfName)
{
	CString sSelected(zfName);
	if (sSelected.Left(6) == "Select")
		return false;

	mbValid = false;
	CString sfName = CUtils::GetBaseDir();
	sfName += L"Input\\Holidays\\";
	sfName = sfName + zfName;
	sfName = sfName + L"Holidays.txt";

	FILE *pfRead = NULL;
	_wfopen_s(&pfRead, sfName, L"r");
	if(!pfRead)
	{
		CString s = L"Missing File for Holidays: ";
		s += sfName;
		MessageBox(NULL, s , L"Installation Error", MB_OK);
		static int nErr = 0;
		nErr++;
		return false;
	}
	//mStatusLabel = sfName;
	msfName = sfName;

	FILE *pfLog = CUtils::TryOpenLogFile(L"ReadHolidays");
	if (pfLog)
		fwprintf(pfLog, L"Reading File: %s\n\n", (const wchar_t *)sfName);

	bool bOK = InitFromFileInternals(pfRead, pfLog);

	if (pfLog)
	{
		if (bOK)
			fwprintf(pfLog, L"Succeeded\n");
		else
			fwprintf(pfLog, L"Failed\n");

		fclose(pfLog);
	}
	fclose(pfRead);
	return bOK;
}
bool CHolidays::InitFromFileInternals(FILE *pfRead, FILE *pfLog)
{
	mn = 0;

	for (int i = 0; i < MAX_HOLIDAYS_DEFINED; i++)
	{
		// Read name or end
		CString sLine = CUtils::ReadLine(pfRead);
		if (pfLog)
			fwprintf(pfLog, L"%s\n", (const wchar_t *)sLine);
		if (sLine.Left(3) == L"end")
			break;

		map[i] = new CHoliday(sLine);

		// Read Year

		sLine = CUtils::ReadLine(pfRead);
		if (pfLog)
			fwprintf(pfLog, L"%s\n", (const wchar_t *)sLine);
		if (sLine == L"*")
		{
			map[i]->mbAllYears = true;
		}
		else
		{
			int year = 0;
			if (!TryConvertInt(sLine, L"year", year))
			{
				CUtils::MessBox(L"Failed to read year as number", L"Input Error");
				return false;

			}
			if (year < 1980 || year > 2050)
			{
				wchar_t zBuf[256];
				swprintf_s(zBuf, 256, L"<Reading holidays from file> Bad value for year: %d", year);
				CUtils::MessBox(zBuf, L"Input Error");
				return false;
			}
			map[i]->mYear = year;
		}

		// Read Month
		int month = 0;
		if (!TryReadInt(pfRead, L"month", month))
		{
			CUtils::MessBox(L"Failed to read month as number", L"Input Error");
			return false;
		}
		if (month < 1 || month > 12)
		{
			wchar_t zBuf[256];
			swprintf_s(zBuf, 256, L"<Reading holidays from file> Bad value for month: %d", month);
			CUtils::MessBox(zBuf, L"Input Error");
			return false;
		}
		map[i]->mMonth = month;

		// Read Day
		int day = 0;
		if (!TryReadInt(pfRead, L"day", day))
		{
			CUtils::MessBox(L"Failed to read day as number", L"Input Error");
			return false;
		}
		if (day < 1 || day > 31)
		{
			wchar_t zBuf[256];
			swprintf_s(zBuf, 256, L"<Reading holidays from file> Bad value for day: %d", day);
			CUtils::MessBox(zBuf, L"Input Error");
			return false;
		}
		map[i]->mDay = day;

		mn++;
	}

	mbValid = true;
	PrintLog();
	return true;
}

void CHolidays::PrintLog()
{
	FILE *pfLog = CUtils::TryOpenLogFile(L"HolidaysDefinition");
	if (!pfLog)
		return;
	fwprintf(pfLog, L"Input File: %s\n\n", (const wchar_t *)msfName);
	fwprintf(pfLog, L"%d holidays defined\n", mn);

	for (int i = 0; i < mn; i++)
	{
		fwprintf(pfLog, L"%2d: %40s - ", i, (const wchar_t *)map[i]->msName);
		if (map[i]->mbAllYears)
			fwprintf(pfLog, L"*        ");
		else
			fwprintf(pfLog, L"year %4d", map[i]->mYear);

		fwprintf(pfLog, L" - month %d, day %d\n", map[i]->mMonth, map[i]->mDay);
	}
	fclose(pfLog);
}
void CHolidays::AddToDebug(int i, bool bPrice)
{
	CString s = map[i]->msName; 
	s += L" ";
	s += CRight::ToString(map[i]->mDay); 
	s += L".";
	s += CRight::ToString(map[i]->mMonth);
	if (map[i]->mYear > 0)
	{
		s += L".";
		s += CRight::ToString(map[i]->mYear);
	}

	if (bPrice)
	{
		s += L" ";
		s += CRight::ToString(map[i]->mPrice);
	}
	LogLine(s);

	msDebug += L" ";
	msDebug += s;
	msDebug += "\n";
}
int CHolidays::NInLastYear(void)
{
	msDebug = L"Holidays:";
	FILE *pfWrite = CUtils::TryOpenLogFile(L"HolidaysInLastYear");
	if (pfWrite)
	{
		fwprintf(pfWrite, L"Input File: %s\n\n", (const wchar_t *)msfName);
		fwprintf(pfWrite, L"%d holidays defined\n", mn);
	}

	if (mn < 1)
	{
		if (pfWrite)
			fclose(pfWrite);
		return 0;
	}

	int n = 0;
	for (int i = 0; i < mn; i++)
	{
		if (gWorkPeriod.LastYearContains(*map[i]))
		{
			n++;
			if (pfWrite)
				map[i]->Log(pfWrite);
			AddToDebug(i,false);
		}
	}

	if ( n > MAX_HOLIDAYS_PER_YEAR )
	{
		n = MAX_HOLIDAYS_PER_YEAR;
		if (pfWrite)
			fprintf(pfWrite, "Max Holidays Per Year: %d\n", n);
	}
	if (pfWrite)
	{
		fprintf(pfWrite, "Return %d", n);
		fclose(pfWrite);
	}
	return n;
}

void CHolidays::ComputePayLastYear(void)
{
	//if (mnWorkedLastYear < 0 || mnPaidLastYear < 0)
	//{
	//	return;
	//}

	for (int i = 0; i < mn; i++)
	{
		map[i]->mbInLastYearPaySum = false;
		if (map[i]->mbInLastYear)
		{
			ComputeHolidayPrice(*map[i]);
		}
	}

	//int nToSum = mnWorkedLastYear - mnPaidLastYear;
	int nToSum = gHolidaysDue.GetNDueLastYear();
	if (nToSum > MAX_HOLIDAYS_PER_YEAR)
		nToSum = MAX_HOLIDAYS_PER_YEAR;
	if (nToSum < 1)
	{
		msDebug = "Holiday Pay - None";
		nToSum = 0;
		msDue += msSelection;
		msDue += " - None last year ";
		return;
	}

	msDebug = "Holidays Pay:\n";
	int nSummed = 0;
	while (nSummed < nToSum)
	{
		double bestPay = 0;
		int iBest = -1;
		for (int i = 0; i < mn; i++)
		{
			if (map[i]->mbInLastYear && !map[i]->mbInLastYearPaySum)
			{
				if (map[i]->mPrice > bestPay)
				{
					bestPay = map[i]->mPrice;
					iBest = i;
				}
			}
		}
		if (iBest >= 0)
		{
			mDuePay += bestPay;
			map[iBest]->mbInLastYearPaySum = true;
			nSummed++;
			AddToDebug(iBest,true);
			mnDaysToPay += 1;
			RememberPayParDay(bestPay);
		}
		else
			break;

	}
	LogLine(L"Pay Last Year", mDuePay);

	msDebug += L" SUM(";
	msDebug += ToString(nSummed);
	msDebug += L")=";

	//msDue += L" - ";
	msDue += msSelection;
	msDue += L" - Last Year ";
	msDue += ToString(nSummed);
	msDue += L" days ";
}
int CHolidays::AddPay4PrevYear(int iPrev)
{
	int nDue = gHolidaysDue.GetNDuePrevYear(iPrev);
	LogLine(L"prev year", iPrev);
	LogLine(L"n days due", nDue);
	if (nDue > 0)
	{
		CMyTime payDate = gWorkYears.GetPrevYearEnd(iPrev);
		double payPerDay = gWageTable.ComputeHolidayPrice(payDate.mYear, payDate.mMonth);
		double payPerYear = payPerDay * nDue;
		RememberPayParDay(payPerDay);
		LogLine(L"pay per day in prev year", payPerDay);
		mnDaysToPay += nDue;
		LogLine(L"pay per year", payPerYear);
		mDuePay += payPerYear;
	}
	return nDue;
}
void CHolidays::ComputePayPrevYears()
{
	int nPrevYears = gHolidaysDue.GetNPrevYears();
	LogLine(L"nPrevYears with holidays", nPrevYears);
	if (nPrevYears < 2)
		return;

	int nDays = 0;
	for (int iPrev = 1; iPrev < nPrevYears; iPrev++)
	{
		nDays += AddPay4PrevYear(iPrev);
	}

	// msw->WriteLine("");
	msDue += L" + ";
	msDue += ToString(nPrevYears-1);
	msDue += L" years, ";
	msDue += ToString(nDays);
	msDue += L" days ";
}
void CHolidays::ComputePayPrevYearsOld()
{
		//msw->WriteLine("ComputePayPrevYears");

	int nWorked = 0; // SafeGetIntFromTextBox(*mpNDaysWorkedPrevYearsBox);
	int nPaid = 0; // SafeGetIntFromTextBox(*mpNDaysPaidPrevYearsBox);
	double nYears = 0; // SafeGetDoubleFromTextBox(*mpPrevNYearsBox);
	int nDaysPerYear = nWorked - nPaid;

	LogLine(L"nWorked", nWorked);
	LogLine(L"nPaid", nPaid);
	LogLine(L"nYears", nYears);
	LogLine(L"nDaysPerYear to pay", nDaysPerYear);
	if (nDaysPerYear < 1)
	{
		//msw->WriteLine("No Need to compute Prev Years");
		return;
	}

	int iPrev = 1;
	double rest = nYears;
	while (rest > 0)
	{
		CMyTime payDate = gWorkYears.GetPrevYearEnd(iPrev++);
		double payPerDay;
		//if (mRateSetByUser > 0)
		//	payPerDay = mRateSetByUser;
		//else
			payPerDay = gWageTable.ComputeHolidayPrice(payDate.mYear, payDate.mMonth);
		double payPerYear = payPerDay * nDaysPerYear;
		RememberPayParDay(payPerDay);
		LogLine(L"pay per day in prev year", payPerDay);
		if (rest >= 1)
		{
			mnDaysToPay += nDaysPerYear;
			rest -= 1;
		}
		else
		{
			mnDaysToPay += nDaysPerYear * rest;
			payPerYear *= rest;
			rest = 0;
		}
		LogLine(L"pay per year", payPerYear);
		mDuePay += payPerYear;
	}

	// msw->WriteLine("");
	msDue += L" + ";
	msDue += ToString(nYears);
	msDue += L" years ";
}
void CHolidays::RememberPayParDay(double value)
{
	if (mMinPayPerDay == 0)
	{
		mMinPayPerDay = value;
		mMaxPayPerDay = value;
	}
	else
	{
		if (value > mMaxPayPerDay)
			mMaxPayPerDay = value;
		if (value < mMinPayPerDay)
			mMinPayPerDay = value;
	}
}
bool CHolidays::InitDefinition()
{
	CString sWantedSet(gpDlg->GetHolidaysSet());
	if (mbValid && msSelection == sWantedSet)
		return true;

	msSelection = sWantedSet;
	if (msSelection.IsEmpty())
	{
		msDue = L"Holidays not defined";
		return false;
	}
	if (msSelection == "-")
	{
		msDue = L"Holidays not defined";
		return false;
	}
	if (!InitFromFile(msSelection))
		return false;

	return true;
}
bool CHolidays::Compute(void)
{
	mnDaysToPay = 0;
	mMinPayPerDay = 0;
	mMaxPayPerDay = 0;

	if (!InitDefinition())
		return false;

	mnInLastYear = NInLastYear();
	// TEMP - This edit box role is not clear -
	// Should it be written from SW or the text should be read from user text?????
	mpNDaysInLastYearBox->SetWindowText (ToString(mnInLastYear));
	LogLine(L"n days in last year", mnInLastYear);

	//if (GetIntFromEditBox(mpNDaysPaidLastYearBox, L"mpNDaysPaidLastYearBox", mnPaidLastYear))
	//	LogLine(L"n paid last year", mnPaidLastYear);

	//if (GetIntFromEditBox(mpNDaysWorkedLastYearBox, L"mpNDaysWorkedLastYearBox", mnWorkedLastYear))
	//	LogLine(L"n worked last year", mnWorkedLastYear);

	ComputePayLastYear();
	ComputePayPrevYears();

	msDebug += ToString(mDuePay);

	msDue += L" ==> ";
	msDue += ToString(mDuePay);
	return true;
}
CString CHolidays::GetDecriptionForLetter(void)
{
	CString s = ToString(mnDaysToPay);
	s += L" days * ";
	if (mMinPayPerDay == mMaxPayPerDay)
	{
		s += ToString(mMinPayPerDay);
	}
	else
	{
		s += L"[";
		s += ToString(mMinPayPerDay);
		s += L"-";
		s += ToString(mMaxPayPerDay);
		s += L"]";
	}
	
	return s;
}
CString CHolidays::GetDecriptionForLetterHebrew(void)
{
	CString s = ToString(mnDaysToPay);
	s += L" ימים * ";
	if (mMinPayPerDay == mMaxPayPerDay)
	{
		s += ToString(mMinPayPerDay);
	}
	else
	{
		s += L"[";
		s += ToString(mMinPayPerDay);
		s += L"-";
		s += ToString(mMaxPayPerDay);
		s += L"]";
	}

	return s;
}
bool CHolidays::ComputeHolidayPrice(CHoliday& holiday)
{
	//if (mRateSetByUser > 0)
	//	holiday.mPrice = mRateSetByUser;
	//else
	holiday.mPrice = gWageTable.ComputeHolidayPrice(holiday.mYear, holiday.mMonth);
	return true;
}
