#include "StdAfx.h"
#include "Person.h"
#include "LogoWriter.h"
#include "HtmlWriter.h"
#include "MyDialogEx.h"
#include "resource.h"

CPerson gWorker;

CPerson::CPerson(void)
	: mbIsPassport(true)
{
}
void CPerson::StartLetter(CLogoWriter &logo)
{
	logo.WriteLine(L"To");
	//logo->WriteLine("");
	CString s = msPrivateName;
	s += " ";
	s += msFamilyName;
	logo.WriteLine(s);
	if (msPassport && msPassport != L"0")
	{
		if (mbIsPassport)
			s = L"Pasport: ";
		else
			s = L"ID: ";

		s += msPassport;
		logo.WriteLine(s);
	}
	if (msTelephone && msTelephone != L"0")
	{
		s = L"Tel: ";
		s += msTelephone;
		logo.WriteLine(s);
	}
}
void CPerson::StartLetter(CHtmlWriter &html)
{
	html.WriteLine(L"To");
	//logo->WriteLine("");
	msPrivateName = mpDlg->GetText(miFirstName);
	msFamilyName = mpDlg->GetText(miFamilyName);
	msPassport = mpDlg->GetText(miId);
	msTelephone = mpDlg->GetText(miTel);
	msEmail = mpDlg->GetText(miEmail);
	mbIsPassport = mpDlg->IsChecked(IDC_RADIO_PASSPORT);

	CString s = msPrivateName;
	s += " ";
	s += msFamilyName;
	html.WriteLine(s);
	if (!msPassport.IsEmpty() && msPassport != L"0")
	{
		s = L"Pasport: ";
		s += msPassport;
		html.WriteLine(s);
	}
	if (!msTelephone.IsEmpty() && msTelephone != L"0")
	{
		s = L"Tel: ";
		s += msTelephone;
		html.WriteLine(s);
	}
	if (!msEmail.IsEmpty() && msEmail != L"0")
	{
		s = L"Email: ";
		s += msEmail;
		html.WriteLine(s);
	}
}
void CPerson::SetDlg(CMyDialogEx *pDlg, int iFirstName, int iFamilyName, int iId, int iTel, int iAdd, int iEmail, int iRole)
{
	mpDlg = pDlg;
	miFirstName = iFirstName;
	miFamilyName = iFamilyName;
	miId = iId;
	miTel = iTel;
	miAdd = iAdd;
	miEmail = iEmail;
	miRole = iRole;
}
int CPerson::UpdateFromDlg()
{
	return 0;
}
void CPerson::UpdateDlg()
{
}
void CPerson::SaveToFile()
{
}
void CPerson::LoadFromFile()
{
}
