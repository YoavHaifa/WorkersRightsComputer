#pragma once
#include "MyTime.h"

class CMonthInfo : public CMyTime
{
public:
	CMonthInfo();
	~CMonthInfo();

	void InitFirst();
	void InitNext(CMonthInfo &prev);
	bool IsPartial(void);

	int mi;
	bool mbLast;
	int mSeniorityMonths;
	int mnAllWorkingDays;
	int mnDaysUnpaidVacation;
	int mnDaysWorked;
	int mHoursPerWeek;
	double mHoursPerWeekPaidByCompany;
	double mFraction;
	void SetUnpaid(int nAnpaidDaysToAdd);
	void SetDaysPerWeek(void);
};

