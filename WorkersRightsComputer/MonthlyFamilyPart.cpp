#include "stdafx.h"
#include "MonthlyFamilyPart.h"
#include "FamilyPart.h"
#include "MonthInfo.h"
#include "Wage.h"
#include "MinWage.h"
#include "WorkPeriod.h"

CMonthlyFamilyPart::CMonthlyFamilyPart(CMonthInfo& monthInfo)
    : mFirstDay(monthInfo.mFirstDay)
    , mFamilyRatio(1)
    , mHoursPerWeekPaidByCompany(monthInfo.mHoursPerWeekPaidByCompany)
    , mRatioPaidByCompany(monthInfo.mRatioPaidByCompany)
    , mMonthFraction(monthInfo.GetFraction())
    , mHourlyRateByCompany(monthInfo.mHourlyRateByCompany)
    , mMonthlyWage(0)
    , mFullCompanyPay(0)
    , mRatioParallelEmployment(0)
    , mRatioFamilyAlone(0)
    , mFamilyRatioInParellel(0)
    , mHoursPerWeek(monthInfo.mHoursPerWeek)
{
    mMonthlyWage = gWage.GetMonthlyWageFor(mFirstDay);
    if (!mMonthlyWage)
        mMonthlyWage = gMinWage.GetMonthlyWage(mFirstDay);
    if (gFamilyPart.mbAskOnlyForFamilyPart)
        Compute();
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
        if (mMonthlyWage > 0)
        {
            if (mHourlyRateByCompany > 0)
            {
                int nWorkHoursInFullMonth = gWorkPeriod.GetWorkingHoursInFullMonth(mFirstDay);
                mFullCompanyPay = mHourlyRateByCompany * nWorkHoursInFullMonth;
            }
            else
                mFullCompanyPay = gMinWage.GetMonthlyWage(mFirstDay);

            if (mMonthlyWage > mFullCompanyPay)
            {
                mRatioParallelEmployment = mFullCompanyPay / mMonthlyWage;
                mRatioFamilyAlone = 1 - mRatioParallelEmployment;
                mFamilyRatioInParellel = mFamilyRatio;
                mFamilyRatio = mRatioParallelEmployment * mFamilyRatio + mRatioFamilyAlone;

                // Prepare explanation
                msCompanyRatio += "(*)";
                sprintf_s(zBuf, sizeof(zBuf), "Initial Family %5.2f%% of %5.2f%% parallel + %5.2f%%",
                    mFamilyRatioInParellel * 100, mRatioParallelEmployment * 100, mRatioFamilyAlone * 100);
                msCompanyRatioExplained  = zBuf;
            }
        }
    }
}
void CMonthlyFamilyPart::LogTitle(FILE* pf)
{
    if (pf)
        fprintf(pf, "iMonth, family, fraction, sumFrac, sum family, company, wage, hourly wage, full, computation\n");
}
void CMonthlyFamilyPart::LogLine(FILE* pf, int i, double sumFractions, double sumFamilyRatio)
{
    if (!pf)
        return;

    fwprintf(pf, L"%d, %5.2f%%, %5.2f, %5.2f, %5.2f, %s", i+1, mFamilyRatio * 100,
        mMonthFraction, sumFractions, sumFamilyRatio, (const wchar_t*)msCompanyRatio);

    if (mHourlyRateByCompany > 0)
    {
        fwprintf(pf, L", %5.2f", mMonthlyWage);
        fwprintf(pf, L", %5.2f", mHourlyRateByCompany);
        fwprintf(pf, L", %5.2f", mFullCompanyPay);
        if (!msCompanyRatioExplained.IsEmpty())
            fwprintf(pf, L", %s", (const wchar_t*)msCompanyRatioExplained);
    }

    fprintf(pf, "\n");
}