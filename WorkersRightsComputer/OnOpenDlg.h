#pragma once


// COnOpenDlg dialog

class COnOpenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COnOpenDlg)

public:
	COnOpenDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~COnOpenDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ON_OPEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};
