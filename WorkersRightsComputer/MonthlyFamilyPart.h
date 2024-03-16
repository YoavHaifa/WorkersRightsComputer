#pragma once

class CMonthlyFamilyPart
{
public:
	CMonthlyFamilyPart(class CMonthInfo &monthInfo);
	void Compute();

	CMyTime mFirstDay;
	double mFamilyRatio;
	double mRatioPaidByCompany;
	double mMonthFraction;
	double mHourlyRateByCompany;
	double mHoursPerWeekPaidByCompany;
	double mMonthlyAgreed;
	double mFullCompanyPay;
	double mRatioParallelEmployment;
	double mRatioFamilyAlone;
	double mFamilyRatioInParellel;

	CString msCompanyRatio;
	CString msCompanyRatioExplained;

	int mHoursPerWeek;

};

