#pragma once
#include "MyTime.h"

class CWorkYear
{
public:
	CWorkYear(void);
	~CWorkYear(void);

	int mSeniority;
	CMyTime mFirstDay;
	CMyTime mDayAfter;
	bool mbLast;
	int mnUnpaidVacationDays;

	void InitFirst(void);
	void InitNext(CWorkYear &prev);
	void InitInternals(void);

	void AddUnpaidVacation(class CVacationUsed& vacation);
	void Log(FILE* pfLog);
};

