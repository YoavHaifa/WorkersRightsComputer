#pragma once
#include "LogoWriter.h"
#include "MyTime.h"
#include "MonthInfo.h"

/*
class CMonthInfo
{
};*/

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
	void WriteToLetter(CLogoWriter &lw);
	void WriteToLetter(class CHtmlWriter &html);

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

	//System::TimeSpan mSpan;
	CString mSpanString;
	CString msLastYearStart;

	// CEdit &mSpanTextBox;
	// DateTimePicker ^ mNoticeDateTimePicker;

	//System::Windows::Forms::Label ^ mWorkingDaysLabel;
	//System::Windows::Forms::Label ^ mLastYearStartLabel;

	static const int N_WEEK_DAYS = 7;
	double maWorkingDays[N_WEEK_DAYS];
	void SetWorkingDay(int iDay, double fraction);

	static const int MAX_MONTHS = 600;
	CMonthInfo maMonths[MAX_MONTHS];

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
	double GetLastYearAsFraction(void);
	bool WorkedAtLeastNMonths(int nMonths);
	int CountDaysToEndOfMonth(CMyTime &date);
	int CountWorkDaysToEndOfMonthFrom(CMyTime &firstDate);
	void Save(FILE *pfSave);
	void Restore(FILE *pfRead);
	bool mbMinWage;
	bool mbMonthlyWage;
	double mMonthlyWage;
	double mHourlyWage;
	double mHoursPerWeek;
	CString GetTextSummary();
	CString GetShortSummary();
	CString GetDaysText();
	int CountWorkingDays(CMyTime &first, CMyTime &last);
	void CountNWorkingDaysFrom(CMyTime &first, int nToSkip, CMyTime &dateAfter);
	int CountWorkingDaysPerMonth(int year, int month);
	int CountDaysWorkedPerMonth(int year, int month);
	void InitDetailsForEachMonth();
	CMonthInfo *GetMonthInfoFor(CMyTime &date);
	CMonthInfo *GetMonthInfoFor(int year, int month);
	void Debug(const wchar_t *zAt);
	void Log(const wchar_t *zAt);

	CList<CMyTime *, CMyTime *> mFullYearsStart;
	void ClearFullYears(void);
	void ComputeFullYears(void);
	bool IncludesMonthButNotFirst(int year, int month);

	void SetWeekDaysPaidByCompany(class CCompanyPartPeriod *pFrom, class CCompanyPartPeriod *pUntil);
	double ComputeFamilyPart(void);
};

extern CWorkPeriod gWorkPeriod;
