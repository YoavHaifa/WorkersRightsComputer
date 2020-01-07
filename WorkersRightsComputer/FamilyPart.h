#pragma once
#include "MyTime.h"
#include "WorkPeriod.h"

class CCompanyPartPeriod
{
public:
	CCompanyPartPeriod(CTime start, double nHours, double perCent, bool bDummy = false)
		: mFrom(start)
		, mCompanyHoursPerWeek(nHours)
		, mbDummyForApril18(bDummy)
	{
		mHoursPerWeek = gWorkPeriod.GetWorkingHoursInFullWeek(mFrom.mYear, mFrom.mMonth);
		//if (mFrom.mYear < 2018 || (mFrom.mYear == 2018 && mFrom.mMonth < 4))
		//	mHoursPerWeek = 43;
		//else
		//	mHoursPerWeek = 42;

		if (mCompanyHoursPerWeek > 0)
			mCompanyPart = mCompanyHoursPerWeek / mHoursPerWeek;
		else
			mCompanyPart = perCent / 100;

		if (mCompanyPart > 1)
			mCompanyPart = 1;
	}
	void UpdateFrom(CCompanyPartPeriod &other)
	{
		mCompanyHoursPerWeek = other.mCompanyHoursPerWeek;
		mCompanyPart = other.mCompanyPart;
		mbDummyForApril18 = false;
	}
	CString GetLine(CCompanyPartPeriod *pPrev)
	{
		char zBuf[128];
		double hours = mCompanyHoursPerWeek;
		double part = mCompanyPart;
		if (mbDummyForApril18)
		{
			hours = pPrev->mCompanyHoursPerWeek;
			if (hours)
				part = hours / mHoursPerWeek;
			else
				part = pPrev->mCompanyPart;
		}
		if (hours > 0)
			sprintf_s(zBuf,sizeof(zBuf), 
				"From %2d/%4d - %5.2f / %d --> %5.2f%%", mFrom.mMonth, mFrom.mYear, hours, mHoursPerWeek, part * 100);
		else 
			sprintf_s(zBuf,sizeof(zBuf), 
				"From %2d/%4d --> %5.2f%%", mFrom.mMonth, mFrom.mYear, part * 100);

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
	void ClearLast(void);
	bool CheckStartTime(class CMyTime &startTime);
	bool AddPeriod(CMyTime& startTime, CCompanyPartPeriod *pNewPeriod);
	bool AddPeriod(CTime startTime, double hoursPerWeekByCompany);
	bool AddPeriodPC(CTime startTime, double perCentByCompany); // By Percent

	bool mbAskOnlyForFamilyPart;
	double mRatio;
	bool mbDefined;

	CList<CCompanyPartPeriod *, CCompanyPartPeriod *> mPeriods;
	CString GetFullText();
	CString GetShortText();
	void Compute();
	void SaveToXml(class CXMLDump &xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

	void Save(FILE *pfSave);
	void Restore(FILE *pfRead);
	void WriteToLetter(class CHtmlWriter &writer);
};

extern CFamilyPart gFamilyPart;
