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
			s = L"Passport: ";
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
CString CPerson::GetFirstName(void)
{
	return mpDlg->GetText(miFirstName);
}
CString CPerson::GetLastName(void)
{
	return mpDlg->GetText(miFamilyName);
}
CString CPerson::GetPassport(void)
{
	return mpDlg->GetText(miId);
}
CString CPerson::GetTel(void)
{
	return mpDlg->GetText(miTel);
}

void CPerson::StartLetter(CHtmlWriter &html)
{
	msPrivateName = mpDlg->GetText(miFirstName);
	msFamilyName = mpDlg->GetText(miFamilyName);
	msPassport = mpDlg->GetText(miId);
	msTelephone = mpDlg->GetText(miTel);
	msEmail = mpDlg->GetText(miEmail);
	mbIsPassport = mpDlg->IsChecked(IDC_RADIO_PASSPORT);

	CString s(L"To: ");
	s += msPrivateName;
	s += " ";
	s += msFamilyName;
	CString sh(L"לכבוד: ");
	sh += msPrivateName;
	sh += " ";
	sh += msFamilyName;
	html.WriteLineEH(s, sh);

	if (!msPassport.IsEmpty() && msPassport != L"0")
	{
		if (mbIsPassport)
		{
			s = L"Passport: ";
			sh = L"פספורט: ";
		}
		else
		{
			s = L"Id: ";
			sh = L"ת.ז.: ";
		}
		html.WriteLineEH(s,sh,msPassport);
	}
	if (!msTelephone.IsEmpty() && msTelephone != L"0")
	{
		html.WriteLineEH(L"Tel: ", L"טל: ", msTelephone);
	}
	if (!msEmail.IsEmpty() && msEmail != L"0")
	{
		html.WriteLineEH(L"Email: ", L"דואל: ", msEmail);
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
void CPerson::SaveToTxtFile()
{
}
void CPerson::LoadFromTxtFile()
{
}
