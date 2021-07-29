#pragma once

class CPensionReportPeriod
{
public:
	CPensionReportPeriod(int year, int month, double monthlyPay, double part, double pensionRate, double severanceRate, double familyPart);
	~CPensionReportPeriod();

	bool Is(double monthlyPay, double pensionRate, double severanceRate, double familyPart);
	void Add(int year, int month, double part);
	void WriteToLetter(class CHtmlWriter& html, bool bPension);

	int mFromYear;
	int mFromMonth;
	int mTillYear;
	int mTillMonth;
	double mMonthlyPay;
	double mMonthParts;
	double mPensionRate;
	double mSeveranceRate;
	double mFamilyPart;
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
	void AddMonth(int year, int month, double monthlyPay, double part, double pensionRate, double severanceRate, double familyPart);
	void WriteToLetterOld(class CHtmlWriter& html);
	void WriteToLetterPension(class CHtmlWriter& html);
	void WriteToLetterSeverance(class CHtmlWriter& html);

	CList<CPensionReportPeriod *, CPensionReportPeriod *> mPeriods;
	CPensionReportPeriod *mpLast;
	void Clear();
};

