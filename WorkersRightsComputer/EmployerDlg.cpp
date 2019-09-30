// EmployerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "EmployerDlg.h"
#include "afxdialogex.h"
#include "Utils.h"
#include "Person.h"


// CEmployerDlg dialog

IMPLEMENT_DYNAMIC(CEmployerDlg, CDialogEx)

CEmployerDlg::CEmployerDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_EMPLOYER, pParent)
{

}

CEmployerDlg::~CEmployerDlg()
{
}

void CEmployerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_EMP_COMMENT, mCommentEdit);
	DDX_Control(pDX, IDC_EDIT_CONTACTS, mListDesc);
}
BOOL CEmployerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect r;
	mCommentEdit.GetRect(&r);
	mCommentEdit.SetRect(r);

	UpdateList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CEmployerDlg::OnOK()
{
	CString sName;
	if (GetItemText(IDC_EDIT_EMP_NAME, sName))
		if (!sName.IsEmpty())
		{
			CUtils::MessBox(L"Last contact details were not saved. Please save or cancel.", L"Notice");
			return;
		}
	CMyDialogEx::OnOK();
}

BEGIN_MESSAGE_MAP(CEmployerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_EMPLOYER, &CEmployerDlg::OnBnClickedButtonAddEmployer)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_CONTACTS, &CEmployerDlg::OnBnClickedButtonClearContacts)
END_MESSAGE_MAP()


// CEmployerDlg message handlers


void CEmployerDlg::OnBnClickedButtonAddEmployer()
{
	AddContact(true);
	UpdateList();
}


void CEmployerDlg::AddContact(bool bEmployer)
{
	CPerson* pPerson = new CPerson();
	pPerson->mbEmployer = bEmployer;
	if (!Set(IDC_EDIT_EMP_NAME, pPerson->msPrivateName) || pPerson->msPrivateName.IsEmpty())
	{
		delete pPerson;
		CUtils::MessBox(L"Please enter name", L"Input Error");
		return;
	}
	Set(IDC_EDIT_EMP_ID, pPerson->msPassport);
	Set(IDC_EDIT_EMP_TEL, pPerson->msTelephone);
	Set(IDC_EDIT_EMP_ADDRESS, pPerson->msAddress);
	Set(IDC_EDIT_EMP_EMAIL, pPerson->msEmail);
	Set(IDC_EDIT_EMP_ROLE, pPerson->msRole);
	Set(IDC_EDIT_EMP_COMMENT, pPerson->msComment);
	gContacts.AddTail(pPerson);
	UpdateList();
	ClearEdit();
}


bool CEmployerDlg::Set(int id, CString& s)
{
	if (!GetItemText(id, s))
	{
		s = L"";
		return false;
	}
	return true;
}


void CEmployerDlg::UpdateList()
{
	if (gContacts.IsEmpty())
	{
		SetText(IDC_EDIT_CONTACTS, L"No contacts defined");
		return;
	}
	CString s;
	int nContacts = 0;
	POSITION pos = gContacts.GetHeadPosition();
	while (pos)
	{
		CPerson* pPerson = gContacts.GetNext(pos);
		if (nContacts > 0)
			s += L"\r\n";
		s += pPerson->GetDescription();
		nContacts++;
	}
	SetText(IDC_EDIT_CONTACTS, s);
}
void CEmployerDlg::ClearEdit()
{
	SetText(IDC_EDIT_EMP_NAME, L"");
	SetText(IDC_EDIT_EMP_ID, L"");
	SetText(IDC_EDIT_EMP_TEL, L"");
	SetText(IDC_EDIT_EMP_ADDRESS, L"");
	SetText(IDC_EDIT_EMP_EMAIL, L"");
	SetText(IDC_EDIT_EMP_ROLE, L"");
	SetText(IDC_EDIT_EMP_COMMENT, L"");
}


void CEmployerDlg::OnBnClickedButtonClearContacts()
{
	CPerson::ClearContacts();
	UpdateList();
}
