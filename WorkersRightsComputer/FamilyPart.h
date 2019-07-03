#pragma once
#include "MyTime.h"

class CCompanyPartPeriod
{
public:
	CCompanyPartPeriod(CTime start, double nHours, bool bDummy = false)
		: mFrom(start)
		, mCompanyHoursPerWeek(nHours)
		, mbDummyForApril18(bDummy)
	{
		if (mFrom.mYear < 2018 || (mFrom.mYear == 2018 && mFrom.mMonth < 4))
			mHoursPerWeek = 43;
		else
			mHoursPerWeek = 42;
		mCompanyPart = mCompanyHoursPerWeek / mHoursPerWeek;
		if (mCompanyPart > 1)
			mCompanyPart = 1;
	}
	void UpdateHours(double nHours)
	{
		mCompanyHoursPerWeek = nHours;
		mbDummyForApril18 = false;
		mCompanyPart = mCompanyHoursPerWeek / mHoursPerWeek;
		if (mCompanyPart > 1)
			mCompanyPart = 1;
	}
	CString GetLine(CCompanyPartPeriod *pPrev)
	{
		char zBuf[128];
		double hours = mCompanyHoursPerWeek;
		double part = mCompanyPart;
		if (mbDummyForApril18)
		{
			hours = pPrev->mCompanyHoursPerWeek;
			part = hours / mHoursPerWeek;
		}
		sprintf_s(zBuf,sizeof(zBuf), 
			"From %2d/%4d - %5.2f / %d --> %5.2f%%", mFrom.mMonth, mFrom.mYear, hours, mHoursPerWeek, part * 100);
		if (mbDummyForApril18)
		{
			CString s("(");
			s += zBuf;
			s += ")";
			return s;
		}
		return CString(zBuf);
	}
	CMyTime mFrom;
	double mCompanyHoursPerWeek;
	int mHoursPerWeek;
	double mCompanyPart;
	bool mbDummyForApril18;
};

class CFamilyPart
{
public:
	CFamilyPart();
	~CFamilyPart();

	void Clear(void);
	bool AddPeriod(CTime startTime, double hoursPerWeek);

	bool mbAskOnlyForFamilyPart;
	double mRatio;
	bool mbDefined;

	CList<CCompanyPartPeriod *, CCompanyPartPeriod *> mPeriods;
	CString GetFullText();
	void Compute();
	void Save(FILE *pfSave);
	void Restore(FILE *pfRead);
};

extern CFamilyPart gFamilyPart;
