// LetterSavedDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "pch.h"
#include "WorkersRightsComputer.h"
#include "LetterSavedDlg.h"
#include "afxdialogex.h"


// CLetterSavedDlg dialog

IMPLEMENT_DYNAMIC(CLetterSavedDlg, CDialogEx)

CLetterSavedDlg::CLetterSavedDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_LETTER_SAVED, pParent)
{

}

CLetterSavedDlg::~CLetterSavedDlg()
{
}

void CLetterSavedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CLetterSavedDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetText(IDC_STATIC_FILE_NAME, msFileName);
	SetText(IDC_STATIC_NO_LETTER, msWarning); // It may be empty
	if (!msTitle.IsEmpty())
		SetTitle(msTitle);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CLetterSavedDlg, CDialogEx)
	ON_STN_CLICKED(IDC_STATIC_NO_LETTER, &CLetterSavedDlg::OnStnClickedStaticNoLetter)
END_MESSAGE_MAP()


// CLetterSavedDlg message handlers


void CLetterSavedDlg::SetFileName(CString& sfName)
{
	msFileName = sfName;
}
void CLetterSavedDlg::SetWarning(const wchar_t* zWarning)
{
	msWarning = zWarning;
}
void CLetterSavedDlg::OnStnClickedStaticNoLetter()
{
	// TODO: Add your control notification handler code here
}
void CLetterSavedDlg::SetNewTitle(const wchar_t* zTitle)
{
	msTitle = zTitle;
}