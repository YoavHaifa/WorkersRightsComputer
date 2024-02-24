#pragma once
#include "MyTime.h"
#include "MonthInfo.h"

class CWorkPeriod
{
public:
	CWorkPeriod();

	bool LastYearContains(class CHoliday &holiday);
	void Reset();
	void WriteToLetter(class CHtmlWriter &html);
	CString GetPeriodForLetter();
	CString GetPeriodForLetterHebrew();

	bool IsValid(bool bMustDefineDays = true);
	bool Compute(const wchar_t *zAt = NULL);
	bool IsBeforeDate(CMyTime &date, int year, int month, int day);
	bool IsAfterDate(CMyTime &date, int year, int month, int day);

	CMyTime mFirst;
	CMyTime mLast;
	CMyTime mNotice;
	CMyTime mLastDayOfNotice; // Valid only after notice was computed
	CMyTime mLastYearStart;

	CString mSpanString;

	static const int N_WEEK_DAYS = 7;
	double maWorkingDays[N_WEEK_DAYS];
	void SetWorkingDay(int iDay, double fraction);
	void ComputeWorkingDays();

	int mnMonthsDetailed;
	bool mbNegative;
	double mnWorkDaysPerWeek;
	double mnDaysInMonthForDailySalary;
	int GetWorkingHoursInFullMonth(CMyTime date);
	int GetWorkingHoursInFullWeek(CMyTime date);
	bool mbSkipNotice;

	// Special mode for caregivers
	bool mbCaregiver;

	// Special extra hours for "live in" caregivers
	bool mbLiveIn;
	bool mbExtraHolidayHoursForLiveInApplied;

	// Special modes used in this case
	bool mbPartialVacationForPrevYearsDefined;

	int CountDaysToEndOfMonth(CMyTime &date);
	int CountWorkDaysToEndOfMonthFrom(CMyTime &firstDate);
	void SaveToXml(class CXMLDump &xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

	//void Restore(FILE *pfRead);
	CString GetTextSummary();
	CString GetShortSummary();
	void ShortLog(FILE* pf);
	CString GetDaysText();
	int CountWorkingDays(CMyTime& first, CMyTime& last);
	int CountWorkingDaysPerMonth(CMyTime& dayInMonth);
	int CountDaysWorkedPerMonth(const CMyTime& date);
	void InitDetailsForEachMonth();
	CMonthInfo *GetMonthInfoFor(CMyTime &date);
	void Debug(const wchar_t *zAt);
	void Log(const wchar_t *zAt);

	bool IncludesMonthButNotFirst(int year, int month);

	void SetWeekDaysPaidByCompany(class CCompanyPartPeriod * pCompanyPart, class CCompanyPartPeriod *pUntil);
	double ComputeFamilyPart();
	double ComputeFamilyPartLastMonths(int nMonths);
	void WriteLastSalary(class CHtmlWriter& html);
	void SetLastDayOfNotice(const CMyTime &lastDayOfNotice) { mLastDayOfNotice = lastDayOfNotice; }
	bool HasFullYearWithNotice();

private:
	static const int MAX_MONTHS = 600;
	CMonthInfo maMonths[MAX_MONTHS];
};

extern CWorkPeriod gWorkPeriod;
