#pragma once


// CRealHolidaysDlg dialog

class CRealHolidaysDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRealHolidaysDlg)

public:
	CRealHolidaysDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CRealHolidaysDlg();

	bool LoadFromXml(class CXMLParseNode* pRoot);

	bool UpdateDataFromDialog(void);
	CString GetDaysText();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REAL_HOLIDAYS };
#endif

protected:
	friend class CHolidaysByDay;

	// Radio buttons for working days
	CButton mCheckSunday;
	CButton mCheckMonday;
	CButton mCheckTuesday;
	CButton mCheckWednesday;
	CButton mCheckThursday;
	CButton mCheckFriday;
	CButton mCheckSaturday;
	CButton* mapCheckDays[7];

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	CComboBox mComboHolidays;
	void InitHolidaysCombo();
	afx_msg void OnCbnSelchangeComboHolidays();

	DECLARE_MESSAGE_MAP()
};
