#pragma once

class CPensionReportPeriod
{
public:
	CPensionReportPeriod(int year, int month, double monthlyPay, double part, double pensionRate, double severanceRate);
	~CPensionReportPeriod();

	bool Is(double monthlyPay, double pensionRate, double severanceRate);
	void Add(int year, int month, double part);
	void WriteToLetter(class CHtmlWriter& html);

	int mFromYear;
	int mFromMonth;
	int mTillYear;
	int mTillMonth;
	double mMonthlyPay;
	double mMonthParts;
	double mPensionRate;
	double mSeveranceRate;
	double mDuePension;
	double mDueSeverance;
};


class CPensionReport
{
public:
	CPensionReport();
	~CPensionReport();

	bool IsEmpty(void) { return mPeriods.IsEmpty(); }
	void AddMonth(int year, int month, double monthlyPay, double part, double pensionRate, double severanceRate);
	void WriteToLetter(class CHtmlWriter& html);

	CList<CPensionReportPeriod *, CPensionReportPeriod *> mPeriods;
	CPensionReportPeriod *mpLast;
	void Clear();
};

