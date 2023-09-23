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
	double GetCompanyRatio(double *poCompanyHours = NULL, double* poCompanyRatio = NULL);
	double GetFamilyPart(double* poCompanyHours, double* poCompanyRatio);

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

