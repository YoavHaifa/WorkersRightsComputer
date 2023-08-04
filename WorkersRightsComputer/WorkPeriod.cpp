#include "StdAfx.h"
#include "WorkPeriod.h"
#include "Holidays.h"
#include "Utils.h"
#include "VacationsDlg.h"
#include "UsedVacations.h"
#include "HtmlWriter.h"
#include "FamilyPart.h"
#include "XMLDump.h"
#include "XMLParse.h"
#include "WorkYears.h"
#include "Wage.h"
#include "HolidaysDue.h"

CWorkPeriod gWorkPeriod;

const wchar_t *uasDaysNames[7] = 
{
	L"Sunday",
	L"Monday",
	L"Tuesday",
	L"Wednesday",
	L"Thursday",
	L"Friday",
	L"Saturday"
};

CWorkPeriod::CWorkPeriod(void)
	: mbSkipNotice(false)
	, mbLiveIn(true)
	, mbCaregiver(false)
	, mbExtraHolidayHoursForLiveInApplied(false)
{
	Reset();

	mSpanString = "---";
	mnWorkDaysPerWeek = 0;

	mnDaysInMonthForDailySalary = 25;
}
bool CWorkPeriod::IsValid(bool bMustDefineDays)
{
	mbNegative = false;
	if (!mFirst.mbInitialized || !mLast.mbInitialized)
		return false;

	if (mFirst > mLast)
	{
		mbNegative = true;
		return false;
	}
	if (bMustDefineDays)
	{
		if (mnWorkDaysPerWeek < 0.05)
			return false;
	}

	return true;
}
int CWorkPeriod::GetWorkingHoursInFullMonth(int year, int month)
{
	if (year < 2018)
		return 186;
	if (year == 2018 && month < 4)
		return 186;
	return 182;
}
int CWorkPeriod::GetWorkingHoursInFullWeek(int year, int month)
{
	if (year < 2018)
		return 43;
	if (year == 2018 && month < 4)
		return 43;
	return 42;
}
void CWorkPeriod::Reset(void)
{
	mFirst.Reset();
	mLast.Reset();
	mNotice.Reset();

	for (int i = 0; i < 7; i++)
		maWorkingDays[i] = 0;

	gWage.SetMinWage();
	gUsedVacations.ClearAllVacations();
	gWorkYears.Clear();
	gFamilyPart.Clear();
	gHolidaysDue.Reset();

	mbSkipNotice = false;
}

bool CWorkPeriod::Compute(const wchar_t* zAt)
{
	CountWorkingDays();

	// Init for invalid work span
	mnMonthsDetailed = 0;
	mSpanString = "";
	// TEMP _ CORRECT!
	//mLastYearStartLabel->Text = "???";

	mbNegative = false;

	if (!IsValid())
	{
		if (!mFirst.mbInitialized)
			mSpanString = "Enter first day at work";
		if (!mLast.mbInitialized)
			mSpanString = "Enter last day at work";
		else if (mbNegative)
			mSpanString = "Illegal Input";
		else if (mnWorkDaysPerWeek < 0.05)
			mSpanString = "Select Working Days in Week";

		else
			mSpanString = "---";
		// TEMP _ CORRECT!
		// mSpanTextBox->Text = mSpanString;
		return false;
	}

	InitDetailsForEachMonth();

	gUsedVacations.Compute();
	gWorkYears.Compute();
	mSpanString = gWorkYears.PrepareSpanString();

	gFamilyPart.Compute();

	Log(zAt? zAt : L"Computed");
	return true;
}
bool CWorkPeriod::IsBeforeDate(CMyTime &date, int year, int month, int day)
{
	if (year < date.mYear)
		return true;
	if (year > date.mYear)
		return false;

	if (month < date.mMonth)
		return true;
	if (month > date.mMonth)
		return false;

	return day < date.mDay;
}
bool CWorkPeriod::IsAfterDate(CMyTime &date, int year, int month, int day)
{
	if (year > date.mYear)
		return true;
	if (year < date.mYear)
		return false;

	if (month > date.mMonth)
		return true;
	if (month < date.mMonth)
		return false;

	return day > date.mDay;
}
bool CWorkPeriod::LastYearContains(CHoliday & holiday)
{
	if (holiday.mYear > 2050)
	{
		static bool bOnce = false;
		if (!bOnce)
		{
			bOnce = true;
			CUtils::MessBox(L"holiday.mYear > 2050", L"Input error");
		}
		return false;
	}
	holiday.mbInLastYear = false;
	if (!IsValid())
		return false;

	if (gWorkYears.LastYearDoContains(holiday))
		holiday.mbInLastYear = true;

	return holiday.mbInLastYear;
}
void CWorkPeriod::SetWorkingDay(int iDay, double fraction)
{
	maWorkingDays[iDay] = fraction;
	Compute();
}
void CWorkPeriod::CountWorkingDays(void)
{
	mnWorkDaysPerWeek = 0;
	for (int i = 0; i < N_WEEK_DAYS; i++)
	{
		mnWorkDaysPerWeek += maWorkingDays[i];
	}

	if (mnWorkDaysPerWeek <= 5)
		mnDaysInMonthForDailySalary = 21.6666;
	else
		mnDaysInMonthForDailySalary = 25;
}
int CWorkPeriod::CountDaysToEndOfMonth(CMyTime &date)
{
	CMyTime dayAfter = date;
	dayAfter.AddDay();
	for (int nDays = 1; nDays < 35; nDays++)
	{
		if (dayAfter.mMonth != date.mMonth)
			return nDays;
		dayAfter.AddDay();
	}
	return 0;
}
int CWorkPeriod::CountWorkDaysToEndOfMonthFrom(CMyTime &firstDate)
{
	int n = 0;
	CMyTime date = firstDate;
	while (date.mMonth == firstDate.mMonth && date <= mLast)
	{
		if (date.IsWorkingDay())
			n++;
		date.AddDay();
	}
	return n;
}
void CWorkPeriod::WriteToLetter(class CHtmlWriter &html)
{
	CString s = L"Work Period: ";
	s += mFirst.ToString();
	s += " - ";
	s += mLast.ToString();
	html.WritePara(s);
}
void CWorkPeriod::WriteLastSalary(class CHtmlWriter& html)
{
	//if (!mbNotIncludingLastSalary)
	//	return;
	html.StartParagraph();
	html.WriteEH(L"This computation does not include last salary until ", 
		L"חישוב זה אינו כולל משכורת אחרונה עד לתאריך ");
	html.WriteLineEH(gWorkPeriod.mLast.ToString(), gWorkPeriod.mLast.ToHebrewString());
	//s += mLastSalaryUntil.ToString();
	//html.WritePara(s);
	html.EndParagraph();
}
CString CWorkPeriod::GetPeriodForLetter(void)
{
	CString s(mFirst.ToString());
	s += " - ";
	s += mLast.ToString();
	return s;
}
CString CWorkPeriod::GetPeriodForLetterHebrew(void)
{
	CString s(mFirst.ToHebrewString());
	s += " - ";
	s += mLast.ToHebrewString();
	return s;
}
void CWorkPeriod::SaveToXml(CXMLDump &xmlDump)
{
	CXMLDumpScope mainScope(L"WorkPeriod", xmlDump);

	xmlDump.Write(L"first", mFirst);
	xmlDump.Write(L"last", mLast);
	xmlDump.Write(L"notice", mNotice);
	xmlDump.Write(L"b_skip_notice", mbSkipNotice);
	xmlDump.Write(L"b_live_in", mbLiveIn);
	xmlDump.Write(L"b_caregiver", mbCaregiver);
	xmlDump.Write(L"b_extra_holiday_hours_for_live_in_applied", mbExtraHolidayHoursForLiveInApplied);

	{
		CXMLDumpScope scope(L"Days", xmlDump);
		for (int iDay = 0; iDay < 7; iDay++)
		{
			xmlDump.Write(uasDaysNames[iDay], maWorkingDays[iDay] > 0 ? 1: 0);
		}
	}
	gWage.SaveToXml(xmlDump);

	gUsedVacations.SaveToXml(xmlDump);
	gFamilyPart.SaveToXml(xmlDump);
	gHolidaysDue.SaveToXml(xmlDump);
}
void CWorkPeriod::LoadFromXml(class CXMLParseNode* pRoot)
{
	CXMLParseNode* pWorkPeriodNode = pRoot->GetFirst(L"WorkPeriod");
	if (!pWorkPeriodNode)
		return;

	pWorkPeriodNode->GetValue(L"first", mFirst);
	pWorkPeriodNode->GetValue(L"last", mLast);
	pWorkPeriodNode->GetValue(L"notice", mNotice);
	pWorkPeriodNode->GetValue(L"b_skip_notice", mbSkipNotice);
	pWorkPeriodNode->GetValue(L"b_live_in", mbLiveIn);
	pWorkPeriodNode->GetValue(L"b_caregiver", mbCaregiver);

	CXMLParseNode* pDays = pWorkPeriodNode->GetFirst(L"Days");
	if (pDays)
	{
		for (int iDay = 0; iDay < 7; iDay++)
		{
			pDays->GetValue(uasDaysNames[iDay], maWorkingDays[iDay]);
		}
	}

	gWage.LoadFromXml(pWorkPeriodNode);

	/*
	bool bHourly = false;
	if (pWorkPeriodNode->GetValue(L"bMinWage", mbMinWage))
	{
	}
	else if (pWorkPeriodNode->GetValue(L"bMonthlyWage", mbMonthlyWage))
	{
		pWorkPeriodNode->GetValue(L"MonthlyWage", mMonthlyWage);
	}
	else if (pWorkPeriodNode->GetValue(L"bHourlyWage", bHourly))
	{
		pWorkPeriodNode->GetValue(L"HourlyWage", mHourlyWage);
		pWorkPeriodNode->GetValue(L"HoursPerWeek", mHoursPerWeek);
	}*/

	Compute(L"LoadFromXml1");

	gUsedVacations.LoadFromXml(pWorkPeriodNode);
	gFamilyPart.LoadFromXml(pWorkPeriodNode);
	gHolidaysDue.LoadFromXml(pWorkPeriodNode, pRoot);

	Compute(L"LoadFromXml2");
}
/*
void CWorkPeriod::Restore(FILE *pfRead)
{
	mFirst.Read(pfRead);
	mLast.Read(pfRead);
	mNotice.Read(pfRead);
	CString s = CUtils::ReadLine(pfRead);
	if (s == "Days")
	{
		for (int iDay = 0; iDay < 7; iDay++)
		{
			int value = CUtils::ReadInt(pfRead);
			SetWorkingDay(iDay, value);
		}
	}
	s = CUtils::ReadLine(pfRead);
	if (s == "MinWage")
	{
		gWage.SetMinWage();
	}

	InitDetailsForEachMonth();

	s = CUtils::ReadLine(pfRead);
	if (s == "Vacations")
	{
		gUsedVacations.Restore(pfRead);
		s = CUtils::ReadLine(pfRead);
	}
	if (s == "FamilyPart")
		gFamilyPart.Restore(pfRead);
	gWorkPeriod.Compute();
}*/
CString CWorkPeriod::GetShortSummary()
{
	CString s(L"Work period: ");
	s += mFirst.GetShortString();
	s += L" - ";
	s += mLast.GetShortString();
	s += ", ";
	s += CRight::ToString((int)mnWorkDaysPerWeek);
	s += " work days/week";
	return s;
}
CString CWorkPeriod::GetTextSummary()
{
	CString s(L"Work period: ");
	s += mFirst.GetShortString();
	s += L" - ";
	s += mLast.GetShortString();
	s += L", ";
	s += GetDaysText();
	s += L", Notice: ";
	s += mNotice.GetShortString();

	s += "\r\n";
	s += gWorkPeriod.mSpanString;
	if (!gWage.IsAllMin())
	{
		s += " (";
		s += gWage.GetShortText();
		s += ")";
	}
	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		s += "\r\n";
		s += gFamilyPart.GetShortText();
	}
	return s;
}
CString CWorkPeriod::GetDaysText()
{
	int nDays = 0;
	CString sDays(_T("("));
	for (int iDay = 0; iDay < 7; iDay++)
	{
		if (maWorkingDays[iDay] > 0)
		{
			nDays++;
			if (nDays > 1)
				sDays += L", ";
			switch (iDay)
			{
			case 0:
				sDays += L"Sun";
				break;
			case 1:
				sDays += L"Mon";
				break;
			case 2:
				sDays += L"Tue";
				break;
			case 3:
				sDays += L"Wed";
				break;
			case 4:
				sDays += L"Thu";
				break;
			case 5:
				sDays += L"Fri";
				break;
			case 6:
				sDays += L"Sat";
				break;
			}
		}
	}
	sDays += L")";
	if (nDays < 1)
		return CString();

	wchar_t zBuf[512];
	swprintf_s(zBuf, sizeof(zBuf) / sizeof(wchar_t), L"%d days/week ", nDays);
	CString s(zBuf);
	s += sDays;
	return s;
}
int CWorkPeriod::CountWorkingDays(CMyTime &first, CMyTime &last)
{
	int n = 0;
	CMyTime check = first;
	while (check <= last)
	{
		if (check.IsWorkingDay())
			n++;
		check.AddDay();
	}
	return n;
}
/*
int CWorkPeriod::CountAllDaysWithoutUnpaidVacation(CMyTime& first, CMyTime& last)
{
	int n = 0;
	CMyTime check = first;
	while (check <= last)
	{
		if (!check.IsUnpaidVacation())
			n++;
		check.AddDay();
	}
	return n;
} */
void CWorkPeriod::CountNWorkingDaysFrom(CMyTime &first, int nToSkip, CMyTime &dateAfter)
{
	int n = 0;
	CMyTime check = first;
	while (n <= nToSkip)
	{
		if (check.IsWorkingDay())
			n++;
		check.AddDay();
	}
	dateAfter = check;
}
int CWorkPeriod::CountAllDaysPerMonth(int year, int month)
{
	int n = 0;
	CMyTime date(year, month, 1);
	while (date.mMonth == month)
	{
		n++;
		date.AddDay();
	}
	return n;
}
int CWorkPeriod::CountWorkingDaysPerMonth(int year, int month)
{
	int n = 0;
	CMyTime date(year, month, 1);
	while (date.mMonth == month)
	{
		if (date.IsWorkingDay())
			n++;
		date.AddDay();
	}
	return n;
}
int CWorkPeriod::CountDaysWorkedPerMonth(int year, int month)
{
	int n = 0;
	CMyTime date(year, month, 1);
	while (date.mMonth == month)
	{
		if ((date >= mFirst) && (date <= mLast))
		{
			if (date.IsWorkingDay())
				n++;
		}
		date.AddDay();
	}
	return n;
}
void CWorkPeriod::InitDetailsForEachMonth()
{
	maMonths[0].InitFirst();
	mnMonthsDetailed = 1;

	while (!maMonths[mnMonthsDetailed - 1].mbLast)
	{
		maMonths[mnMonthsDetailed].InitNext(maMonths[mnMonthsDetailed - 1]);
		mnMonthsDetailed++;
	}
}

CMonthInfo *CWorkPeriod::GetMonthInfoFor(CMyTime &date)
{
	return GetMonthInfoFor(date.mYear, date.mMonth);
}
CMonthInfo *CWorkPeriod::GetMonthInfoFor(int year, int month)
{
	for (int i = 0; i < MAX_MONTHS; i++)
	{
		if ((maMonths[i].mYear == year) && (maMonths[i].mMonth == month))
		{
			return &maMonths[i];
		}
		if (maMonths[i].mbLast)
		{
			CUtils::MessBox(L"<CWorkPeriod::GetMonthInfoFor> no proper period", L"SW Error");
			return &maMonths[i];
		}
	}
	CUtils::MessBox(L"<CWorkPeriod::GetMonthInfoFor> no last period", L"SW Error");
	return &maMonths[MAX_MONTHS - 1];
}

void CWorkPeriod::Debug(const wchar_t *zAt)
{
	if (mnMonthsDetailed < 2)
		return;

	for (int i = 1; i < MAX_MONTHS; i++)
	{
		if ((maMonths[i].mYear == maMonths[i - 1].mYear) && (maMonths[i].mMonth == maMonths[i - 1].mMonth))
		{
			CUtils::MessBox(L"<CWorkPeriod::Debug> error", zAt);
			return;
		}
		if (maMonths[i].mbLast)
		{
			return;
		}
	}
	CUtils::MessBox(L"<CWorkPeriod::GetMonthInfoFor> no last period", L"SW Error");
}
void CWorkPeriod::Log(const wchar_t *zAt)
{
	FILE *pfLog = CUtils::TryOpenLogFile(L"WorkPeriod", zAt);
	if (!pfLog)
		return;

	CString s(mFirst.ToString());
	fwprintf(pfLog, L"First Day %s\n", (const wchar_t *)s);
	s = mLast.ToString();
	fwprintf(pfLog, L"Last Day %s\n", (const wchar_t *)s);
	s = mNotice.ToString();
	fwprintf(pfLog, L"Notice Day %s\n", (const wchar_t *)s);

	fprintf(pfLog, "\n");

	for (int i = 0; i < MAX_MONTHS; i++)
	{
		if (maMonths[i].mFraction < 1)
			fwprintf(pfLog, L"%3d: %d/%02d - %0.2f\n", maMonths[i].mi, 
				maMonths[i].mYear, maMonths[i].mMonth, maMonths[i].mFraction);
		if (maMonths[i].mbLast)
			break;
	}

	fclose(pfLog);

	gUsedVacations.Log();
	gWorkYears.Log();
}
bool CWorkPeriod::IncludesMonthButNotFirst(int year, int month)
{
	if (!mFirst.IsMonthBefore(year, month))
		return false;
	if (mLast.IsMonthBefore(year, month))
		return false;
	return true;
}
void CWorkPeriod::SetWeekDaysPaidByCompany(class CCompanyPartPeriod *pFrom, class CCompanyPartPeriod *pUntil)
{
	if (!maMonths[0].mbInitialized)
	{
		CUtils::MessBox(L"<SetWeekDaysPaidByCompany> Months array not initialized", L"SW Error");
		return;
	}
	for (int i = 0; i < MAX_MONTHS; i++)
	{
		if (maMonths[i].IsMonthBefore(pFrom->mFrom))
			continue;
		if (pUntil && !maMonths[i].IsMonthBefore(pUntil->mFrom))
			return;
		maMonths[i].mHoursPerWeekPaidByCompany = pFrom->mCompanyHoursPerWeek;
		maMonths[i].mRatioPaidByCompany = pFrom->mCompanyPart;
		if (maMonths[i].mbLast)
			return;
	}
}
double CWorkPeriod::ComputeFamilyPart()
{
	double sumFractions = 0;
	double sumCompanyRatio = 0;

	for (int i = 0; i < MAX_MONTHS; i++)
	{
		double companyRatio = maMonths[i].GetCompanyRatio();
		sumFractions += maMonths[i].mFraction;
		sumCompanyRatio += companyRatio * maMonths[i].mFraction;
		if (maMonths[i].mbLast)
			break;
	}

	double companyTotalRatio = sumCompanyRatio / sumFractions;
	return 1 - companyTotalRatio;
}
double CWorkPeriod::ComputeFamilyPartLastMonths(int nMonthsWanted)
{
	double sumFractions = 0;
	double sumCompanyRatio = 0;

	double missingFraction = nMonthsWanted;
	int iLast = mnMonthsDetailed - 1;

	for (int iMonth = iLast; iMonth >= 0 && missingFraction > 0; iMonth--)
	{
		double companyRatio = maMonths[iMonth].GetCompanyRatio();
		double fraction = min(maMonths[iMonth].mFraction, missingFraction);

		sumFractions += fraction;
		missingFraction -= fraction;
		sumCompanyRatio += companyRatio * fraction;
	}

	double companyTotalRatio = 0;
	if (sumFractions > 0)
		companyTotalRatio = sumCompanyRatio / sumFractions;
	return 1 - companyTotalRatio;
}
