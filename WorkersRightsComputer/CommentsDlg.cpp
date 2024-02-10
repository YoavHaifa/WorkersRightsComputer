// CommentsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "CommentsDlg.h"
#include "afxdialogex.h"
#include "Comments.h"

// CCommentsDlg dialog

IMPLEMENT_DYNAMIC(CCommentsDlg, CDialogEx)

CCommentsDlg::CCommentsDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_COMMENTS, pParent)
{

}

CCommentsDlg::~CCommentsDlg()
{
}

void CCommentsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCommentsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_COMMENTS, &CCommentsDlg::OnBnClickedButtonClearComments)
END_MESSAGE_MAP()

BOOL CCommentsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	gComments.UpdateGUI(this);

	GotoDlgCtrl(GetDlgItem(IDC_EDIT_COMMENT1));
	return FALSE;  // return TRUE  unless you set the focus to a control
}
void CCommentsDlg::OnOK()
{
	gComments.UpdateFromGUI(this);
	CDialogEx::OnOK();
}
void CCommentsDlg::OnBnClickedButtonClearComments()
{
	Clear(IDC_EDIT_COMMENT1);
	Clear(IDC_EDIT_COMMENT2);
	Clear(IDC_EDIT_COMMENT3);
	SetCheck(IDC_CHECK_DISPLAY_COMMENTS, true);
}
