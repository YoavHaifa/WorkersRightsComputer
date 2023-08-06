
// WorkersRightsComputerDlg.h : header file
//

#pragma once
#include "Right.h"
#include "MyDialogEx.h"

// CWorkersRightsComputerDlg dialog
class CWorkersRightsComputerDlg : public CMyDialogEx
{
// Construction
public:
	CWorkersRightsComputerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WORKERSRIGHTSCOMPUTER_DIALOG };
#endif
	CString msStartFile;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileExit();
	afx_msg void OnBnClickedWorkPeriod();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	bool VerifyThatNotInSave();
	void OnLoad();

	CEdit mEditFirstName;
	CEdit mEditFamilyName;
	CEdit mEditID;
	CEdit mEditTel;
	CEdit mEditLastYearWork;
	CEdit mEditLastYearPaid;
	CEdit mEditLastYearFrom;
	//CEdit mEditPrevYearWork;
	//CEdit mEditPrevYearPaid;
	//CEdit mEditPrevYearFrom;
	CEdit mEditAdditionalDesc;
	CEdit mEditAdditionalSum;
	CEdit mEditPaidDesc;
	CEdit mEditPaidSum;
	CEdit mEditVacationPrevYears;
	CEdit mEditRecuperationPrevYears;
	CEdit mEditNDaysPaidLastYear;

	CList<CEditRef *, CEditRef *> mEditBoxes;
	CComboBox mComboHolidays;
	afx_msg void OnBnClickedButtonReset();
	void ResetAllInputs(bool bLoading);

	CButton mAllowSevLess;
	CButton mDemandVac4Prev;
	CButton mDemandRec4Prev;
	CButton mEntitledOnly2Sev;
	CButton mHadPensionBefore;
	CList<CButtonRef *, CButtonRef *> mButtons;
	void InitHolidaysCombo();
	afx_msg void OnBnClickedCheckSeveranceLessThanYear();
	afx_msg void OnBnClickedCheckVacationYears();
	afx_msg void OnBnClickedCheckRecuperationYears();
	afx_msg void OnBnClickedCheckOnlySeverance();
	afx_msg void OnBnClickedCheckActivePension();
	afx_msg void OnCbnSelchangeComboHolidays();
	afx_msg void OnEnChangeEditHolidaysLastyWork();
	void InitializeAllRights();
	CString GetHolidaysSet();
	void DisplaySummary(const wchar_t *zText);
	CEdit mSummary;
	bool mbInitialized;
	afx_msg void OnComputeAll();
	afx_msg void OnFileSaveas();
	afx_msg void OnFileLoad();

	afx_msg void OnTestCreatedir();
	//afx_msg void OnFileLoadoldcase();
	void OnInputChange();
	void OnInputChange(bool bJustLoaded);
	void OnRadioPaidVacationForLastYearChange();
	bool mbDisableComputations;
	bool mbSaveInProcess;
	//CEdit mEditPrevNYears;
	static DWORD WINAPI StaticThreadFunc(LPVOID);
	afx_msg void OnTestVerifybatch();
	afx_msg void OnTestWritehtml();
	afx_msg void OnBnClickedEmployer();
	afx_msg void OnBnClickedComments();
	CButton mRadiID;
	CButton mRadioPassport;
	CEdit mFilledBy;
	CEdit mFilledByHebrew;
	CEdit mAddress;
	CEdit mEmail;
	void SaveToXml(class CXMLDump& xmlDump);
	bool LoadFromXml(class CXMLParseNode *pRoot);
	afx_msg void OnTestLoadxml();
	void WriteEditorToLetter(class CHtmlWriter& html);
	afx_msg void OnTestLoadtxt();
	// Allows user to set special rate per holidays
	// CEdit mEditPayPerEachHolyDay;
	CButton mVacationPaidAll4LastYear;
	CButton mVacationPaidDays4LastYear;
	CButton mVacationPaidNone4LastYear;
	CButton mRecuperationPaid4LastYear;
	//afx_msg void OnBnClickedCheckPaidVacation();
	//afx_msg void OnBnClickedCheckPaidRecup();
	afx_msg void OnBnClickedCheckLiveIn();
	afx_msg void OnBnClickedButtonPrevYearsHolidays();
	afx_msg void OnBnClickedCheckCaregiver();
	afx_msg void OnBnClickedButtonVacationsPrevYears();
};

extern CWorkersRightsComputerDlg *gpDlg;