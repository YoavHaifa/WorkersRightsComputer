#pragma once


// CRealHolidaysDlg dialog

class CRealHolidaysDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRealHolidaysDlg)

public:
	CRealHolidaysDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CRealHolidaysDlg();

	bool LoadFromXml(class CXMLParseNode* pRoot);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REAL_HOLIDAYS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	CComboBox mComboHolidays;
	void InitHolidaysCombo();

	DECLARE_MESSAGE_MAP()
};
