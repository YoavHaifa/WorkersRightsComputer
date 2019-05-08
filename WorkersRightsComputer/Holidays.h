#pragma once

#include "right.h"

class CHoliday
{
public:
	CHoliday(const wchar_t *zName)
		: msName(zName)
		, mbAllYears(false)
		, mbInLastYear(false)
	{
	}
	CString msName;
	int mYear;
	int mMonth;
	int mDay;
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
	virtual bool Compute(void) override;
	virtual CString GetDecriptionForLetter(void)override;

	bool InitFromFile(const wchar_t *zfName);
	bool InitFromFileInternals(FILE *pfRead, FILE *pfLog);
	void PrintLog();
	int NInLastYear(void);

	static const int MAX_HOLIDAYS_PER_YEAR = 9;
	static const int MAX_HOLIDAYS_DEFINED = 200;

	bool mbValid;

	CEdit *mpNDaysInLastYearBox;
	CEdit *mpNDaysWorkedLastYearBox;
	CEdit *mpNDaysPaidLastYearBox;

	CEdit *mpNDaysWorkedPrevYearsBox;
	CEdit *mpNDaysPaidPrevYearsBox;
	CEdit *mpPrevYearsFromBox;
	CEdit *mpPrevNYearsBox;

	int mn;
	CString msfName;
	CHoliday *map[MAX_HOLIDAYS_DEFINED];
	int mnWorkedLastYear;
	int mnPaidLastYear;
	int mnInLastYear;

	double mnDaysToPay;
	double mMinPayPerDay;
	double mMaxPayPerDay;
	void RememberPayParDay(double value);

	void ComputePayLastYear(void);
	void ComputePayPrevYears(void);

	void AddToDebug(int i, bool bPrice);
	void SetNPaidLastYear(int nPaid){mnPaidLastYear = nPaid;}
	void SetNWorkedLastYear(int nWorked){mnWorkedLastYear = nWorked;}
	CString msSelection;
};

