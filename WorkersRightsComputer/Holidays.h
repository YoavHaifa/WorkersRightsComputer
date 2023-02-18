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
	bool InitDefinition();
	virtual bool Compute(void) override;
	virtual CString GetDecriptionForLetter(void)override;
	virtual CString GetDecriptionForLetterHebrew(void)override;

	bool InitFromFile(const wchar_t *zfName);
	bool InitFromFileInternals(FILE *pfRead, FILE *pfLog);
	void PrintLog();
	int NInLastYear(void);
	CString GetSelection() { return msSelection; }
	bool Is(const CString &sSelection) { return msSelection == sSelection; }

	static const int MAX_HOLIDAYS_PER_YEAR = 9;
	static const int MAX_HOLIDAYS_DEFINED = 200;

private:
	bool mbValid;

	CEdit *mpNDaysInLastYearBox;
	CEdit *mpNDaysWorkedLastYearBox;
	CEdit *mpNDaysPaidLastYearBox;

	CEdit *mpNDaysWorkedPrevYearsBox;
	CEdit *mpNDaysPaidPrevYearsBox;
	CEdit *mpPrevYearsFromBox;
	CEdit* mpPrevNYearsBox;
	//CEdit* mpPayRatePerHolidayBox;

	int mn;
	CString msfName;
	CHoliday *map[MAX_HOLIDAYS_DEFINED];
	//int mnWorkedLastYear;
	//int mnPaidLastYear;
	int mnInLastYear;

	double mnDaysToPay;
	double mMinPayPerDay;
	double mMaxPayPerDay;
	//double mRateSetByUser;
	void RememberPayParDay(double value);
	bool ComputeHolidayPrice(class CHoliday& holiday);

	void ComputePayLastYear();
	void ComputePayPrevYearsOld(); // Before the new class CHolidaysDue
	void ComputePayPrevYears();
	int AddPay4PrevYear(int i);

	void AddToDebug(int i, bool bPrice);
	//void SetNPaidLastYear(int nPaid){mnPaidLastYear = nPaid;}
	//void SetNWorkedLastYear(int nWorked){mnWorkedLastYear = nWorked;}
	CString msSelection;
};

