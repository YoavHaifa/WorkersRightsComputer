#include "StdAfx.h"
#include "WorkPeriod.h"
#include "Holidays.h"
#include "Utils.h"
#include "VacationsDlg.h"
#include "UsedVacations.h"
#include "HtmlWriter.h"
#include "FamilyPart.h"
#include "CompanyPartPeriod.h"
#include "XMLDump.h"
#include "XMLParse.h"
#include "WorkYears.h"
#include "Wage.h"
#include "HolidaysDue.h"
#include "VacationDaysDue.h"
#include "MonthlyFamilyPart.h"

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

CWorkPeriod::CWorkPeriod()
	: mbSkipNotice(false)
	, mbLiveIn(true)
	, mbCaregiver(false)
	, mbExtraHolidayHoursForLiveInApplied(false)
	, mbPartialVacationForPrevYearsDefined(false)
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
int CWorkPeriod::GetWorkingHoursInFullMonth(CMyTime date)
{
	if (date.mYear < 2018)
		return 186;
	if (date.mYear == 2018 && date.mMonth < 4)
		return 186;
	return 182;
}
int CWorkPeriod::GetWorkingHoursInFullWeek(CMyTime date)
{
	if (date.mYear < 2018)
		return 43;
	if (date.mYear == 2018 && date.mMonth < 4)
		return 43;
	return 42;
}
void CWorkPeriod::Reset()
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
	ComputeWorkingDays();

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
	gWorkYears.DivideWorkPeriodToWorkYears();
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
void CWorkPeriod::ComputeWorkingDays()
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
CString CWorkPeriod::GetPeriodForLetter()
{
	CString s(mFirst.ToString());
	s += " - ";
	s += mLast.ToString();
	return s;
}
CString CWorkPeriod::GetPeriodForLetterHebrew()
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
	xmlDump.Write(L"b_partial_vacation_for_prev_years_defined", mbPartialVacationForPrevYearsDefined);

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
	ComputeWorkingDays();

	gWage.LoadFromXml(pWorkPeriodNode);

	gUsedVacations.LoadFromXml(pWorkPeriodNode);

	gFamilyPart.LoadFromXml(pWorkPeriodNode);
	Compute(L"LoadFromXml1");

	gHolidaysDue.LoadFromXml(pWorkPeriodNode, pRoot);
	gVacationDaysDue.LoadFromXml(pRoot);

	Compute(L"LoadFromXml2");
}
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
void CWorkPeriod::ShortLog(FILE* pf)
{
	if (!pf)
		return;
	CString sPeriod = gWorkPeriod.GetShortSummary();
	fwprintf(pf, L"%s\n\n", (const wchar_t*)sPeriod);
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
	if (!gUsedVacations.IsEmpty())
	{
		s += " (";
		s += gUsedVacations.GetVacationsShortText();
		s += ")";
	}
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
int CWorkPeriod::CountWorkingDaysPerMonth(CMyTime& dayInMonth)
{
	CMyTime firstDay(dayInMonth.FirstDayOfMonth());
	CMyTime lastDay(dayInMonth.LastDayOfMonth());
	return CountWorkingDays(firstDay, lastDay);
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
}*/
/*
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
}*/
int CWorkPeriod::CountDaysWorkedPerMonth(const CMyTime& date)
{
	int n = 0;
	CMyTime check(date);
	check.mDay = 1;
	int month = date.mMonth;

	while (check.mMonth == month)
	{
		if ((check >= mFirst) && (check <= mLast))
		{
			if (check.IsWorkingDay())
				n++;
		}
		check.AddDay();
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
	for (int i = 0; i < MAX_MONTHS; i++)
	{
		//if ((maMonths[i].mYear == year) && (maMonths[i].mMonth == month))
		if (maMonths[i].Contains(date))
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
		if (maMonths[i].mFirstDay == maMonths[i - 1].mFirstDay)
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

	mFirst.LogLine(pfLog, L"First Day");
	mLast.LogLine(pfLog, L"Last Day");
	mNotice.LogLine(pfLog, L"Notice Day");
	fprintf(pfLog, "\n");

	fprintf(pfLog, "Work Days per week: %.2f\n", mnWorkDaysPerWeek);
	for (int i = 0; i < 7; i++)
		fprintf(pfLog, "%d: %s\n", i + 1, maWorkingDays[i] ? "Yes" : "No");;
	fprintf(pfLog, "\n");

	for (int i = 0; i < MAX_MONTHS; i++)
	{
		maMonths[i].Log(pfLog);

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
void CWorkPeriod::SetPartPaidByCompany(class CCompanyPartPeriod *pCompanyPart, class CCompanyPartPeriod *pUntil)
{
	if (!maMonths[0].mbInitializedBeforeVacation)
	{
		CUtils::MessBox(L"<SetPartPaidByCompany> Months array not initialized", L"SW Error");
		return;
	}
	for (int i = 0; i < MAX_MONTHS; i++)
	{
		if (maMonths[i].mLastDay.IsMonthBefore(pCompanyPart->mFrom))
			continue;
		if (pUntil && !maMonths[i].mLastDay.IsMonthBefore(pUntil->mFrom))
			return;
		maMonths[i].mHoursPerWeekPaidByCompany = pCompanyPart->mHoursPerWeek;
		maMonths[i].mRatioPaidByCompany = pCompanyPart->mFraction;
		maMonths[i].mHourlyRateByCompany = pCompanyPart->mHourlyWage;
		if (maMonths[i].mbLast)
			return;
	}
}
double CWorkPeriod::ComputeFamilyPart()
{
	FILE* pfLog = CUtils::OpenOutputFile(L"FamilyPart", L"csv");
	if (pfLog)
	{
		fprintf(pfLog, "<ComputeFamilyPart> for all period\n");
		CMonthlyFamilyPart::LogTitle(pfLog);
	}
	double sumFractions = 0;
	double sumFamilyRatio = 0;

	for (int i = 0; i < MAX_MONTHS; i++)
	{
		CMonthlyFamilyPart monthly(maMonths[i]);

		sumFractions += monthly.mMonthFraction;
		sumFamilyRatio += monthly.mFamilyRatio * monthly.mMonthFraction;

		monthly.LogLine(pfLog, i, sumFractions, sumFamilyRatio);
		if (maMonths[i].mbLast)
			break;
	}

	if (sumFractions > 0)
		sumFamilyRatio = sumFamilyRatio / sumFractions;
	if (pfLog)
	{
		fprintf(pfLog, "==> %5.2f%%\n", sumFamilyRatio*100);
		fclose(pfLog);
	}
	return sumFamilyRatio;
}
double CWorkPeriod::ComputeFamilyPartLastMonths(int nMonthsWanted)
{
	double sumFractions = 0;
	double sumFamilyRatio = 0;
	FILE* pfLog = CUtils::OpenOutputFile(L"FamilyPartLastMonths", L"csv");
	if (pfLog)
	{
		fprintf(pfLog, "<ComputeFamilyPartLastMonths> for %d months\n", nMonthsWanted);
		CMonthlyFamilyPart::LogTitle(pfLog);
	}

	double missingFraction = nMonthsWanted;
	int iLast = mnMonthsDetailed - 1;

	for (int iMonth = iLast; iMonth >= 0 && missingFraction > 0; iMonth--)
	{
		CMonthlyFamilyPart monthly(maMonths[iMonth]);

		double fraction = min(monthly.mMonthFraction, missingFraction);
		sumFractions += fraction;
		sumFamilyRatio += monthly.mFamilyRatio * fraction;
		missingFraction -= fraction;

		monthly.LogLine(pfLog, iMonth, sumFractions, sumFamilyRatio);
	}

	if (sumFractions > 0)
		sumFamilyRatio = sumFamilyRatio / sumFractions;

	if (pfLog)
	{
		fprintf(pfLog, "==> %5.2f%%\n", sumFamilyRatio*100);
		fclose(pfLog);
	}
	return sumFamilyRatio;
}
bool CWorkPeriod::HasFullYearWithNotice()
{
	if (!mLastDayOfNotice.IsValid())
	{
		static int unErrors = 0;
		if (!unErrors++)
			CUtils::MessBox(L"<CWorkPeriod::HasFullYearWithNotice> notice not defned", L"SW Error");
		return false;
	}

	return gWorkYears.HasFullYearWithNotice(mLastDayOfNotice);
}