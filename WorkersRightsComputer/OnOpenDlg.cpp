// COnOpenDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "pch.h"
#include "WorkersRightsComputer.h"
#include "OnOpenDlg.h"
#include "afxdialogex.h"


// COnOpenDlg dialog

IMPLEMENT_DYNAMIC(COnOpenDlg, CDialogEx)

COnOpenDlg::COnOpenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ON_OPEN, pParent)
{

}

COnOpenDlg::~COnOpenDlg()
{
}

void COnOpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void COnOpenDlg::OnCancel()
{
	exit(1);
}

BEGIN_MESSAGE_MAP(COnOpenDlg, CDialogEx)
END_MESSAGE_MAP()


// COnOpenDlg message handlers
