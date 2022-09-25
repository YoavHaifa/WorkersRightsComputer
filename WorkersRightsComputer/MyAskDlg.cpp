// CMyAskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "MyAskDlg.h"
#include "afxdialogex.h"


// CMyAskDlg dialog

IMPLEMENT_DYNAMIC(CMyAskDlg, CDialogEx)

CMyAskDlg::CMyAskDlg(const wchar_t *zTitle, const wchar_t *zText, CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_ASK, pParent)
	, msTitle(zTitle)
	, msText(zText)
	, mbOK(false)
{
}

CMyAskDlg::~CMyAskDlg()
{
}

void CMyAskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BOOL CMyAskDlg::OnInitDialog()
{
	SetWindowText(msTitle);
	SetText(IDC_STATIC_ASK, msText);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

bool CMyAskDlg::Ask(void)
{
	DoModal();
	return mbOK;
}
void CMyAskDlg::OnOK()
{
	mbOK = true;
	CDialogEx::OnOK();
}
void CMyAskDlg::OnCancel()
{
	mbOK = false;
	CDialogEx::OnCancel();
}

BEGIN_MESSAGE_MAP(CMyAskDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyAskDlg message handlers
