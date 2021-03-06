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
	double GetCompanyRatio(void);
	double GetFamilyPart(void);

	int mi;
	bool mbLast;
	int mSeniorityMonths;
	int mnAllDays;
	int mnAllWorkingDays;
	int mnDaysUnpaidVacation;
	int mnDaysWorked;
	int mHoursPerWeek;
	double mHoursPerWeekPaidByCompany;
	double mRatioPaidByCompany;
	double mFraction;
	void SetUnpaid(int nAnpaidDaysToAdd);
	//void SetDaysPerWeek(void);
};

