#pragma once
#include "MyTime.h"

class CCompanyPartPeriod
{
public:
	CCompanyPartPeriod(const class CFamilyPartDlg& dlg); // Init new period from Dialog
	CCompanyPartPeriod(class CXMLParseNode* pPeriodNode);
	CCompanyPartPeriod(const CCompanyPartPeriod& other);
	CCompanyPartPeriod(CTime start); // Dummy period

	//CCompanyPartPeriod(CTime start, double nHours, double perCent, bool bDummy = false);

	void UpdateFrom(CCompanyPartPeriod& other);
	CString GetLine(CCompanyPartPeriod* pPrev);
	const CMyTime& GetStartTime() { return mFrom; }


	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

private:
	friend class CFamilyPartDlg;

	void ComputeFraction(); // Call from constructors

	friend class CFamilyPart;
	friend class CWorkPeriod;

	CMyTime mFrom;
	double mHoursPerWeek;
	double mPC;
	double mFraction;
	bool mbDummyForApril18;

	// Added fields - February 2024
	bool mbCompanyPaysMinimumWage; // Otherwise hourly wage by company should be defined by GUI
	double mHourlyWage;

	int mWorkingHoursPerWeek;
};

