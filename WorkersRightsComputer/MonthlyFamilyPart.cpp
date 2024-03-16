#include "stdafx.h"
#include "MonthlyFamilyPart.h"
#include "MonthInfo.h"
#include "Wage.h"
#include "MinWage.h"
#include "WorkPeriod.h"

CMonthlyFamilyPart::CMonthlyFamilyPart(CMonthInfo& monthInfo)
	: mFirstDay(monthInfo.mFirstDay)
    , mFamilyRatio(1)
    , mHoursPerWeekPaidByCompany(monthInfo.mHoursPerWeekPaidByCompany)
    , mRatioPaidByCompany(monthInfo.mRatioPaidByCompany)
    , mMonthFraction(0)
    , mHourlyRateByCompany(monthInfo.mHourlyRateByCompany)
    , mMonthlyAgreed(0)
    , mFullCompanyPay(0)
    , mRatioParallelEmployment(0)
    , mRatioFamilyAlone(0)
    , mFamilyRatioInParellel(0)
    , mHoursPerWeek(monthInfo.mHoursPerWeek)
{
    mMonthlyAgreed = gWage.GetMonthlyWageFor(mFirstDay);
}
void CMonthlyFamilyPart::Compute()
{
    // If nothing else is defined
    mFamilyRatio = 1;
    char zBuf[128];

    if (mHoursPerWeekPaidByCompany > 0)
    {
        mFamilyRatio = 1 - mHoursPerWeekPaidByCompany / mHoursPerWeek;
        sprintf_s(zBuf, sizeof(zBuf), "%5.2f", mHoursPerWeekPaidByCompany);
        msCompanyRatio = zBuf;
    }
    else if (mRatioPaidByCompany)
    {
        // Defined by ratio
        mFamilyRatio = 1.0 - mRatioPaidByCompany;
        sprintf_s(zBuf, sizeof(zBuf), "%5.2f%%", mRatioPaidByCompany * 100);
        msCompanyRatio = zBuf;
    }

    if (mFamilyRatio < 1.0)
    {
        mMonthlyAgreed = gWage.GetMonthlyWageFor(mFirstDay);
        if (mMonthlyAgreed > 0)
        {
            if (mHourlyRateByCompany > 0)
            {
                int nWorkHoursInFullMonth = gWorkPeriod.GetWorkingHoursInFullMonth(mFirstDay);
                mFullCompanyPay = mHourlyRateByCompany * nWorkHoursInFullMonth;
            }
            else
                mFullCompanyPay = gMinWage.GetMonthlyWage(mFirstDay);

            if (mMonthlyAgreed > mFullCompanyPay)
            {
                mRatioParallelEmployment = mFullCompanyPay / mMonthlyAgreed;
                mRatioFamilyAlone = 1 - mRatioParallelEmployment;
                mFamilyRatioInParellel = mFamilyRatio;
                mFamilyRatio = mRatioParallelEmployment * mFamilyRatio + mRatioFamilyAlone;

                // Prepare explanation
                sprintf_s(zBuf, sizeof(zBuf), ", Agreed %5.2f > %5.2f: Initial Family %5.2f%% of %5.2f%% parallel + %5.2f%%",
                    mMonthlyAgreed, mFullCompanyPay,
                    mFamilyRatioInParellel * 100, mRatioParallelEmployment * 100, mRatioFamilyAlone * 100);
                msCompanyRatioExplained  = zBuf;
            }
        }
    }
}