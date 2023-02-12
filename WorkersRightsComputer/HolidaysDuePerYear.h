#pragma once
#include "afxtempl.h"

struct SHolidayData
{
	SHolidayData(int id, int value=0)
		: mValue(value)
		, mGuiId(id)
	{}

	void UpdateGui(class CPrevYearsHolidaysDlg* pDlg);
	void OnGuiChange(class CPrevYearsHolidaysDlg* pDlg);


	int mValue;
	int mGuiId;
};

class CHolidaysDuePerYear
{
public:
	CHolidaysDuePerYear(int idWorked, int idPaid, int idExist, int idDue);
	void UpdateGui(class CPrevYearsHolidaysDlg* pDlg);
	void OnGuiChange(CPrevYearsHolidaysDlg* pDlg);
	void Zero();
	void Add(CHolidaysDuePerYear &other);
	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pNode);

	int mId;

private:
	bool ValidateValues();

	static int umId;
	CList<SHolidayData*, SHolidayData*> mData;
	SHolidayData mWorked;
	SHolidayData mPaid;
	SHolidayData mInYear;
	SHolidayData mDue;
};

