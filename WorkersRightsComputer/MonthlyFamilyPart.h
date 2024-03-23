#pragma once
#include "MyTime.h"
#include "StringsList.h"

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
	bool mbCompanyRatioExplained;

	int mHoursPerWeek;

	// Static members for comments explaining (optional) complex computations
	// Interface for static comments
	static void StartNewSeries();
	static void StartComputingPension();
	static void EndComputingPension();

	static void WriteCommentsToLetter(class CHtmlWriter& html);

private:
	static int umiCommentIndex;
	static CString umsLastExplanation;
	static bool umbComputingPension;
	static CStringsList umEnglishComments;
	static CStringsList umHebrewComments;
};

