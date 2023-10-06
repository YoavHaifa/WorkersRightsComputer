#pragma once

#include "right.h"
#include "MyTime.h"

class CHoliday : public CMyTime
{
public:
	CHoliday(const wchar_t *zName)
		: msName(zName)
		, mbAllYears(false)
		, mbInLastYear(false)
	{
	}
	CString msName;
	bool mbAllYears;
	bool mbInLastYear;
	bool mbInLastYearPaySum;
	double mPrice;
	void Log(FILE*pf);
};

class CHolidays : public CRight
{
public:
	CHolidays(void);
	bool IsValid(void) {return mbValid;}
	virtual	bool SetEditRef(class CEditRef *pRef) override;
	bool InitDefinition();
	virtual bool Compute(void) override;
	virtual CString GetDecriptionForLetter(void)override;
	virtual CString GetDecriptionForLetterHebrew(void)override;

	void PrintLog();
	int NInLastYear(void);
	CString GetSelection() { return msSelection; }
	bool Is(const CString &sSelection) { return msSelection == sSelection; }

	static const int MAX_HOLIDAYS_PER_YEAR = 9;
	static const int MAX_HOLIDAYS_DEFINED = 300;

private:
	bool InitFromFile(const wchar_t *zfName);
	bool InitFromFileInternals(FILE *pfRead, FILE *pfLog);

	bool mbValid;

	CEdit *mpNDaysInLastYearBox;
	CEdit *mpNDaysWorkedLastYearBox;
	CEdit *mpNDaysPaidLastYearBox;

	CEdit *mpNDaysWorkedPrevYearsBox;
	CEdit *mpNDaysPaidPrevYearsBox;
	CEdit *mpPrevYearsFromBox;
	CEdit* mpPrevNYearsBox;

	int mn;
	CString msfName;
	CHoliday *map[MAX_HOLIDAYS_DEFINED];
	int mnInLastYear;

	double mnDaysToPay;
	double mMinPayPerDay;
	double mMaxPayPerDay;
	void RememberPayParDay(double value);
	bool ComputeHolidayPrice(class CHoliday& holiday);

	void ComputePayLastYear();
	void ComputePayPrevYears();
	int AddPay4PrevYear(int i);
	int GetNWorkedHolidays(class CWorkYear* pYear);

	void AddToDebug(int i, bool bPrice);
	CString msSelection;
};

