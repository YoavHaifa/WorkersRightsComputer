#pragma once
#include "MyTime.h"

class CMonthlyFamilyPart
{
public:
	CMonthlyFamilyPart(class CMonthInfo& monthInfo);
	void Compute();

	static void LogTitle(FILE* pf);
	void LogLine(FILE* pf, int i, double sumFractions, double sumFamilyRatio);

	CMyTime mFirstDay;

	double mFamilyRatio;
	double mRatioPaidByCompany;
	double mMonthFraction;
	double mHourlyRateByCompany;
	double mHoursPerWeekPaidByCompany;
	double mMonthlyWage;
	double mFullCompanyPay;
	double mRatioParallelEmployment;
	double mRatioFamilyAlone;
	double mFamilyRatioInParellel;

	CString msCompanyRatio;
	CString msCompanyRatioExplained;

	int mHoursPerWeek;

};

