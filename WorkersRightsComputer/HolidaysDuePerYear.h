#pragma once
#include "afxtempl.h"

struct SHolidayData
{
	SHolidayData(int id, int value=0)
		: mValue(value)
		, mGuiId(id)
	{}

	void UpdateGui(class CMyDialogEx* pDlg);
	void OnGuiChange(CMyDialogEx* pDlg);
	void SetInvisible(CMyDialogEx* pDlg);

	int mValue;
	int mGuiId;
};

class CHolidaysDuePerYear
{
public:
	CHolidaysDuePerYear();
	CHolidaysDuePerYear(int idPrompt, int idWorked, int idPaid, int idExist, int idDue);

	// Initializations
	void Zero();
	void Init(class CWorkYear* pWorkYear = NULL);
	void SetIgnore() { mbRelevant = false; }

	// Dialog Functionality
	void UpdateGui(class CMyDialogEx* pDlg);
	void OnGuiChange(CMyDialogEx* pDlg);
	void Add(CHolidaysDuePerYear &other);
	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pNode);
	void SetNInYear(int n) { mInYear.mValue = n; }

	void UpdateShortText(CMyDialogEx* pDlg, int id);

	// Interface for computations
	int GetDue() { return mDue.mValue; }
	int GetNInSum() { return mnInSum; }

	// Load from Old Save
	void SetValues(int nWorked, int nPaid);

	int mId;
	bool mbRelevant;

private:
	void SetInvisible(CMyDialogEx* pDlg);
	bool ValidateValues();

	static int umId;
	CList<SHolidayData*, SHolidayData*> mData;
	SHolidayData mWorked;
	SHolidayData mPaid;
	SHolidayData mInYear;
	SHolidayData mDue;

	int mnInSum;

	int mIdPrompt;
	CString msPrompt;
};

