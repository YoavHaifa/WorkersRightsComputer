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
	void LogFraction(FILE* pf);
	double GetFraction() { return mMonthFraction; }
	double GetFractionForPension() { return mMonthFractionForPension; }

	int mi;
	bool mbLast;
	int mSeniorityMonths;
	int mnAllDays;
	int mnAllWorkingDays;
	int mnDaysUnpaidVacation;
	int mnDaysNoPension;
	int mnDaysWorked;
	int mnDaysWorkedForPension;
	int mHoursPerWeek;
	double mHoursPerWeekPaidByCompany;
	double mRatioPaidByCompany;
	void SetUnpaid(int nAnpaidDaysToAdd);
	void SetNoPension(int nDaysToAdd);
	int GetNRelevantDays(bool bPension);

private:
	double mMonthFraction;
	double mMonthFractionForPension;
	void ComputeFractionBeforeVacations();
};

