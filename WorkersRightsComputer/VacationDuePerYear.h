#pragma once

struct SVacationData
{
	SVacationData(int id, bool bRadio, bool bInt, int value = 0)
		: mValue(value)
		, mGuiId(id)
		, mbRadio(bRadio)
		, mbInt(bInt)
	{}

	void UpdateGui(class CMyDialogEx* pDlg);
	void OnGuiChange(CMyDialogEx* pDlg);
	void SetInvisible(CMyDialogEx* pDlg);

	bool mbRadio;
	bool mbInt;
	int mValue;
	int mGuiId;
	CString msText;
};

class CVacationDuePerYear
{
public:
	CVacationDuePerYear(int idPrompt, int idPaid, int idAll, 
		int idDays, int idDaysPaid, int idAllDays, int idNone, 
		int idDueText, int idDaysDue);

	void Zero();
	void Init(class CWorkYear* pWorkYear = NULL);
	void UpdateGui(class CMyDialogEx* pDlg);
	void SetIgnore() { mbRelevant = false; }

private:
	void SetInvisible(CMyDialogEx* pDlg);

	// All relevant GUI element
	SVacationData mPrompt;
	SVacationData mPaid;
	SVacationData mAll;
	SVacationData mDays;
	SVacationData mDaysPaid;
	SVacationData mNAllDays;
	SVacationData mNone;
	SVacationData mDueText;
	SVacationData mDaysDue;

	CList<SVacationData*, SVacationData*> mData;
	static int umId;
	int mId;

	// Relevant Data
	typedef enum EVacPY
	{
		VACPY_UNDEF,
		VACPY_ALL,
		VACPY_DAYS,
		VACPY_NONE
	}EVacPY;

	EVacPY meVacPY;
	int mnPaidDays;
	int mnAllDays;
	int mnDueDays;
	bool mbRelevant;
};

