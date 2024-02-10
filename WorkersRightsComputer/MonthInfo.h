#pragma once
#include "WorkSpan.h"

class CMonthInfo : public CWorkSpan
{
public:
	CMonthInfo();
	~CMonthInfo();

	void InitFirst();
	void InitNext(CMonthInfo &prev);
	bool IsPartial(void);
	double GetCompanyRatio(double *poCompanyHours = NULL, double* poCompanyRatio = NULL);
	double GetFamilyPart(double* poCompanyHours, double* poCompanyRatio);
	void Log(FILE* pf);
	void LogFraction(FILE* pf);
	double GetFraction() { return mMonthFraction; }
	double GetFractionForPension() { return mMonthFractionForPension; }

	int mi;
	bool mbInitializedBeforeVacation;
	bool mbLast;
	int mSeniorityMonths;
	int mnDaysUnpaidVacation;
	int mnDaysNoPension;
	int mnDaysWorked;
	int mnDaysWorkedForPension;
	int mHoursPerWeek;
	double mHoursPerWeekPaidByCompany;
	double mRatioPaidByCompany;
	void SetUnpaid(int nAnpaidDaysToAdd);
	void SetNoPension(int nDaysToAdd);
	int GetNRelevantDaysIn(CDaysSpan &span, bool bPension);

private:
	bool Debug(const wchar_t* zAt);

	double mMonthFraction;
	double mMonthFractionForPension;
	void ComputeFractionBeforeVacations();
};

