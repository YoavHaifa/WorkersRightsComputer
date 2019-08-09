#pragma once
#include "LogoWriter.h"
#include "MyTime.h"
#include "MonthInfo.h"



class CWorkPeriod
{
public:
	CWorkPeriod(void);

	//void SetFirst(CTime date);
	//void SetLast(CTime date);
	//void SetNotice(CTime date);
	void SetMinWage(void);
	void SetMonthlyWage(double wage);
	void SetHourlyWage(double wagePerHour, double nHoursPerWeek);

	bool LastYearContains(class CHoliday &holiday);
	bool LastYearDoContains(int year, int month, int day);
	void ComputeLastYearsStart(void);
	void ComputeWorkDays(void);
	void Reset(void);
	// void WriteToLetter(CLogoWriter &lw);
	void WriteToLetter(class CHtmlWriter &html);
	CString GetPeriodForLetter(void);
	CString GetPeriodForLetterHebrew(void);

	bool IsValid(void)
	{
		mbNegative = false;
		if (!mFirst.mbInitialized || !mLast.mbInitialized)
			return false;

		if (mFirst > mLast)
		{
			mbNegative = true;
			return false;
		}
		if (mnWorkDaysPerWeek < 0.05)
			return false;

		return true;
	}
	bool Compute(void);
	bool IsBeforeDate(CMyTime &date, int year, int month, int day);
	bool IsAfterDate(CMyTime &date, int year, int month, int day);

	CMyTime mFirst;
	CMyTime mLast;
	CMyTime mNotice;
	CMyTime mLastYearStart;

	CString mSpanString;
	CString msLastYearStart;


	static const int N_WEEK_DAYS = 7;
	double maWorkingDays[N_WEEK_DAYS];
	void SetWorkingDay(int iDay, double fraction);
	void CountWorkingDays(void);

	int mnCalendarYears;
	int mnFullWorkYears;
	int mnMonths;
	int mnMonthsDetailed;
	int mnDays;
	bool mbNegative;
	int mnWorkingDays;
	double mnWorkDaysPerWeek;
	double mYearsForSeverance;
	double mnDaysInMonthForDailySalary;
	double mnWorkingHoursInFullMonth;

	void ComputeForSeverance(void);
	void PrepareSpanString(void);
	double GetLastYearAsFractionMinusUnpaidvacation(void);
	bool WorkedAtLeastNMonths(int nMonths);
	int CountDaysToEndOfMonth(CMyTime &date);
	int CountWorkDaysToEndOfMonthFrom(CMyTime &firstDate);
	void Save(FILE *pfSave);
	void SaveToXml(class CXMLDump &xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

	void Restore(FILE *pfRead);
	bool mbMinWage;
	bool mbMonthlyWage;
	double mMonthlyWage;
	double mHourlyWage;
	double mHoursPerWeek;
	CString GetTextSummary();
	CString GetShortSummary();
	CString GetDaysText();
	int CountWorkingDays(CMyTime& first, CMyTime& last);
	//int CountAllDaysWithoutUnpaidVacation(CMyTime& first, CMyTime& last);
	void CountNWorkingDaysFrom(CMyTime &first, int nToSkip, CMyTime &dateAfter);
	int CountAllDaysPerMonth(int year, int month);
	int CountWorkingDaysPerMonth(int year, int month);
	int CountDaysWorkedPerMonth(int year, int month);
	void InitDetailsForEachMonth();
	CMonthInfo *GetMonthInfoFor(CMyTime &date);
	CMonthInfo *GetMonthInfoFor(int year, int month);
	void Debug(const wchar_t *zAt);
	void Log(const wchar_t *zAt);

	bool IncludesMonthButNotFirst(int year, int month);

	void SetWeekDaysPaidByCompany(class CCompanyPartPeriod *pFrom, class CCompanyPartPeriod *pUntil);
	double ComputeFamilyPart(void);
	void WriteLastSalary(class CHtmlWriter& html);

private:
	static const int MAX_MONTHS = 600;
	CMonthInfo maMonths[MAX_MONTHS];
};

extern CWorkPeriod gWorkPeriod;
