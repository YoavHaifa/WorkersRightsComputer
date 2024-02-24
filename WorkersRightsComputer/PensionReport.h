#pragma once
#include "DaysSpan.h"

class CPensionReportPeriod : public CDaysSpan
{
public:
	CPensionReportPeriod(const CMyTime &date, double monthlyPay, double part, 
		double pensionRate, double severanceRate, 
		double familyPart, const wchar_t *zCompanyPart);
	~CPensionReportPeriod();

	bool Is(double monthlyPay, double pensionRate, double severanceRate, double familyPart);
	void Add(const CMyTime& date, double part);
	void WriteToLetter(class CHtmlWriter& html, bool bPension);

	double mMonthlyPay;
	double mMonthParts;
	double mPensionRate;
	double mSeveranceRate;
	double mFamilyPart;
	CString msCompanyPart;
	double mDuePension;
	double mDueSeverance;
	double mDueFromFamily;
};


class CPensionReport
{
public:
	CPensionReport();
	~CPensionReport();

	bool IsEmpty(void) { return mPeriods.IsEmpty(); }
	void AddMonth(const CMyTime& Date, double monthlyPay, double part,
		double pensionRate, double severanceRate, 
		double familyPart, const wchar_t* zCompanyPart);
	//void WriteToLetterOld(class CHtmlWriter& html);
	void WriteToLetterPension(class CHtmlWriter& html);
	void WriteToLetterSeverance(class CHtmlWriter& html);

	CList<CPensionReportPeriod *, CPensionReportPeriod *> mPeriods;
	CPensionReportPeriod *mpLast;
	void Clear();
};

