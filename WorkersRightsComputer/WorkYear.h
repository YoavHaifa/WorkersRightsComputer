#pragma once
#include "MyTime.h"
#include "WorkSpan.h"

class CWorkYear : public CWorkSpan
{
public:
	CWorkYear(void);
	~CWorkYear(void);

	int mSeniority;
	double mFraction; // Only the last year may be < 1

	void InitFirst(void);
	void InitNext(CWorkYear &prev);
	void InitInternals(CMyTime &firtsDay);
	int GetNFullMonths(int* pnExtraDays, double* pExtraDaysFraction);
	bool Contains(class CHoliday& holiday);
	bool Contains(CMyTime &time);

	void Log(FILE* pfLog);
	int mDebug;
};

