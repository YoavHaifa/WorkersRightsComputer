#pragma once
#include "MyTime.h"
#include "MonthInfo.h"

class CWorkPeriod
{
public:
	CWorkPeriod(void);

	bool LastYearContains(class CHoliday &holiday);
	void Reset(void);
	void WriteToLetter(class CHtmlWriter &html);
	CString GetPeriodForLetter(void);
	CString GetPeriodForLetterHebrew(void);

	bool IsValid(bool bMustDefineDays = true);
	bool Compute(const wchar_t *zAt = NULL);
	bool IsBeforeDate(CMyTime &date, int year, int month, int day);
	bool IsAfterDate(CMyTime &date, int year, int month, int day);

	CMyTime mFirst;
	CMyTime mLast;
	CMyTime mNotice;
	CMyTime mLastYearStart;

	CString mSpanString;

	static const int N_WEEK_DAYS = 7;
	double maWorkingDays[N_WEEK_DAYS];
	void SetWorkingDay(int iDay, double fraction);
	void ComputeWorkingDaysInMonth(void);

	int mnMonthsDetailed;
	bool mbNegative;
	double mnWorkDaysPerWeek;
	double mnDaysInMonthForDailySalary;
	int GetWorkingHoursInFullMonth(int year, int month);
	int GetWorkingHoursInFullWeek(int year, int month);
	bool mbSkipNotice;

	// Special mode for caregivers
	bool mbCaregiver;

	// Special extra hours for "live in" caregivers
	bool mbLiveIn;
	bool mbExtraHolidayHoursForLiveInApplied;

	int CountDaysToEndOfMonth(CMyTime &date);
	int CountWorkDaysToEndOfMonthFrom(CMyTime &firstDate);
	void SaveToXml(class CXMLDump &xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

	//void Restore(FILE *pfRead);
	CString GetTextSummary();
	CString GetShortSummary();
	CString GetDaysText();
	int CountWorkingDays(CMyTime& first, CMyTime& last);
	//int CountAllDaysWithoutUnpaidVacation(CMyTime& first, CMyTime& last);
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
	double ComputeFamilyPart();
	double ComputeFamilyPartLastMonths(int nMonths);
	void WriteLastSalary(class CHtmlWriter& html);

private:
	static const int MAX_MONTHS = 600;
	CMonthInfo maMonths[MAX_MONTHS];
};

extern CWorkPeriod gWorkPeriod;
