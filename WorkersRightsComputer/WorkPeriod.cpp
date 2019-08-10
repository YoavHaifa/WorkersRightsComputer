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
	: mbMinWage(true)
	, mbMonthlyWage(false)
	, mMonthlyWage(0)
	, mHourlyWage(0)
	, mHoursPerWeek(0)
{
	Reset();

	mSpanString = "---";
	mnWorkDaysPerWeek = 0;

	mnDaysInMonthForDailySalary = 25;
	mnWorkingHoursInFullMonth = 186;
}
void CWorkPeriod::Reset(void)
{
	mFirst.Reset();
	mLast.Reset();
	mNotice.Reset();

	for (int i = 0; i < 7; i++)
		maWorkingDays[i] = 0;

	SetMinWage();
	gUsedVacations.ClearAllVacations();
	gWorkYears.Clear();
	//ClearFullYears();
}
void CWorkPeriod::SetMinWage(void)
{
	mbMinWage = true;
	mbMonthlyWage = false;
	mMonthlyWage = 0;
	mHourlyWage = 0;
	mHoursPerWeek = 0;
}
void CWorkPeriod::SetMonthlyWage(double wage)
{
	mbMinWage = false;
	mbMonthlyWage = true;
	mMonthlyWage = wage;
	mHourlyWage = 0;
	mHoursPerWeek = 0;
}
void CWorkPeriod::SetHourlyWage(double wagePerHour, double nHoursPerWeek)
{
	mbMinWage = false;
	mbMonthlyWage = false;
	mMonthlyWage = 0;
	mHourlyWage = wagePerHour;
	mHoursPerWeek = nHoursPerWeek;
}
//void CWorkPeriod::SetFirst(CTime date)
//{
//	mFirst.Set(date);
//	Compute();
//}
/*
void CWorkPeriod::SetLast(CTime date)
{
	mLast.Set(date);

	if (mNotice.mbInitialized && mNotice > mLast)
	{
		mNotice = mLast;
		MessageBoxA(NULL, "How to set notice?", "SW", MB_OK);
		//mNoticeDateTimePicker->Value = mNotice;
	}
	else
	{
		if (!mNotice.mbInitialized)
		{
			SetNotice(date);
			MessageBoxA(NULL, "How to set notice?", "SW", MB_OK);
			//mNoticeDateTimePicker->Value = mNotice;
		}
	}

	Compute();
}*/
/*
void CWorkPeriod::SetNotice(CTime date)
{
	mNotice.Set(date);
	if (!mLast.mbInitialized)
		return;

	if (mNotice > mLast)
	{
		static int count = 0;
		mNotice = mLast;
		MessageBox(NULL, L"How to set notice?", L"SW", MB_OK);
		//mNoticeDateTimePicker->Value = mLast;
		mNotice.Reset();
		if (!count++)
			MessageBox(NULL, L"Notice Date can't be after last day at work!", L"Notice", MB_OK);
		return;
	}

	Compute();
}*/
bool CWorkPeriod::Compute(void)
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

	gWorkPeriod.Log(L"Computed");
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
/*
void CWorkPeriod::WriteToLetter(CLogoWriter &lw)
{
	CString s = L"Work Period: ";
	s += mFirst.ToString();
	s += " - ";
	s += mLast.ToString();
	lw.WriteLine(s);
} */
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
		L"חישוב זה אינו כולל שכר אחרון עד לתאריך ");
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

	{
		CXMLDumpScope scope(L"Days", xmlDump);
		for (int iDay = 0; iDay < 7; iDay++)
		{
			xmlDump.Write(uasDaysNames[iDay], maWorkingDays[iDay] > 0 ? 1: 0);
		}
	}
	if (mbMinWage)
	{
		xmlDump.Write(L"bMinWage", mbMinWage);
	}
	else if (mbMonthlyWage)
	{
		xmlDump.Write(L"bMonthlyWage", mbMonthlyWage);
		xmlDump.Write(L"MonthlyWage", mMonthlyWage);
	}
	else
	{
		xmlDump.Write(L"bHourlyWage", true);
		xmlDump.Write(L"HourlyWage", mHourlyWage);
		xmlDump.Write(L"HoursPerWeek", mHoursPerWeek);
	}

	//xmlDump.Write(L"bNotIncludingLastSalary", mbNotIncludingLastSalary);
	//xmlDump.Write(L"last_salary_until", mLastSalaryUntil);

	gUsedVacations.SaveToXml(xmlDump);
	gFamilyPart.SaveToXml(xmlDump);
}
void CWorkPeriod::LoadFromXml(class CXMLParseNode* pRoot)
{
	CXMLParseNode* pMain = pRoot->GetFirst(L"WorkPeriod");
	if (!pMain)
		return;

	pMain->GetValue(L"first", mFirst);
	pMain->GetValue(L"last", mLast);
	pMain->GetValue(L"notice", mNotice);

	{
		CXMLParseNode* pDays = pMain->GetFirst(L"Days");
		if (pDays)
		{
			for (int iDay = 0; iDay < 7; iDay++)
			{
				pDays->GetValue(uasDaysNames[iDay], maWorkingDays[iDay]);
			}
		}
	}

	bool bHourly = false;
	if (pMain->GetValue(L"bMinWage", mbMinWage))
	{
	}
	else if (pMain->GetValue(L"bMonthlyWage", mbMonthlyWage))
	{
		pMain->GetValue(L"MonthlyWage", mMonthlyWage);
	}
	else if (pMain->GetValue(L"bHourlyWage", bHourly))
	{
		pMain->GetValue(L"HourlyWage", mHourlyWage);
		pMain->GetValue(L"HoursPerWeek", mHoursPerWeek);
	}

	//pMain->GetValue(L"bNotIncludingLastSalary", mbNotIncludingLastSalary);
	//pMain->GetValue(L"last_salary_until", mLastSalaryUntil);

	Compute();

	gUsedVacations.LoadFromXml(pMain);
	gFamilyPart.LoadFromXml(pMain);

	Compute();
}
void CWorkPeriod::Save(FILE *pfSave)
{
	fwprintf(pfSave, L"WorkPeriod\n");
	mFirst.Write(pfSave);
	mLast.Write(pfSave);
	mNotice.Write(pfSave);
	fwprintf(pfSave, L"Days\n");
	for (int iDay = 0; iDay < 7; iDay++)
	{
		fwprintf(pfSave, L"%d\n", maWorkingDays[iDay] > 0 ? 1: 0);
	}
	if (mbMinWage)
	{
		fwprintf(pfSave, L"MinWage\n");
	}
	else if (mbMonthlyWage)
	{
		fwprintf(pfSave, L"MonthlyWage\n");
		fwprintf(pfSave, L"%.2f\n", mMonthlyWage);
	}
	else
	{
		fwprintf(pfSave, L"HourlyWage\n");
		fwprintf(pfSave, L"%.2f\n", mHourlyWage);
		fwprintf(pfSave, L"%.2f\n", mHoursPerWeek);
	}

	gUsedVacations.Save(pfSave);
	gFamilyPart.Save(pfSave);
}
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
		SetMinWage();
	}
	else if (s == "MonthlyWage")
	{
		double wage = CUtils::ReadFloat(pfRead);
		SetMonthlyWage(wage);
	}
	else
	{
		double wage = CUtils::ReadFloat(pfRead);
		double nHours = CUtils::ReadFloat(pfRead);
		SetHourlyWage(wage, nHours);
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
	CString sfName(L"WorkPeriod_");
	sfName += zAt;
	FILE *pfLog = CUtils::OpenLogFile(sfName);
	if (!pfLog)
		return;

	CString s(mFirst.ToString());
	fwprintf(pfLog, L"First Day %s\n", (const wchar_t *)s);
	s = mLast.ToString();
	fwprintf(pfLog, L"Last Day %s\n", (const wchar_t *)s);
	s = mNotice.ToString();
	fwprintf(pfLog, L"Notice Day %s\n", (const wchar_t *)s);

	fprintf(pfLog, "\n");
	/*
	if (!mFullYearsStart.IsEmpty())
	{
		fprintf(pfLog, "Start of full years\n");
		int count = 0;
		POSITION pos = mFullYearsStart.GetHeadPosition();
		while (pos)
		{
			CMyTime *pDate = mFullYearsStart.GetNext(pos);
			s = pDate->ToString();
			fwprintf(pfLog, L"%2d: %s\n", ++count, (const wchar_t *)s);
		}
		fprintf(pfLog, "\n");
	} */

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
/*
void CWorkPeriod::ClearFullYears(void)
{
	while (!mFullYearsStart.IsEmpty())
	{
		delete mFullYearsStart.GetTail();
		mFullYearsStart.RemoveTail();
	}
} */
/*
void CWorkPeriod::ComputeFullYears(void)
{
	ClearFullYears();

	//if (gUsedVacations.IsEmpty())
	//	return;

	CMyTime yearStart = mFirst;
	while (yearStart <= mLast)
	{
		mFullYearsStart.AddTail(new CMyTime(yearStart));
		CMyTime nextYearStart(yearStart.mYear + 1, yearStart.mMonth, yearStart.mDay);

		gUsedVacations.UpdateNextYearStart(yearStart, nextYearStart);
		yearStart = nextYearStart;
	}
	if (!mFullYearsStart.IsEmpty())
	{
		mLastYearStart = *mFullYearsStart.GetTail();
		mnFullWorkYears = mFullYearsStart.GetSize() - 1;
	}
	else
	{
		mLastYearStart = mFirst;
		mnFullWorkYears = 0;
	}
} */
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
double CWorkPeriod::ComputeFamilyPart(void)
{
	double sumFractions = 0;
	double sumCompanyRatio = 0;

	for (int i = 0; i < MAX_MONTHS; i++)
	{
		double companyRatio = maMonths[i].GetCompanyRatio(); // mHoursPerWeekPaidByCompany / maMonths[i].mHoursPerWeek;
		sumFractions += maMonths[i].mFraction;
		sumCompanyRatio += companyRatio * maMonths[i].mFraction;
		if (maMonths[i].mbLast)
			break;
	}

	double companyTotalRatio = sumCompanyRatio / sumFractions;
	return 1 - companyTotalRatio;
}