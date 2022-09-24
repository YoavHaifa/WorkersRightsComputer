#include "StdAfx.h"
#include "Person.h"
#include "LogoWriter.h"
#include "HtmlWriter.h"
#include "MyDialogEx.h"
#include "resource.h"
#include "XMLDump.h"
#include "XMLParse.h"

CPerson gWorker;
CList<CPerson*, CPerson*> gContacts;

CPerson::CPerson(void)
	: mbIsPassport(true)
	, mbEmployer(false)
{
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
			sh = L"דרכון: ";
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
CString CPerson::GetDescription()
{
	CString s(msPrivateName);
	if (!msFamilyName.IsEmpty())
	{
		s += " ";
		s += msFamilyName;
	}
	if (!msPassport.IsEmpty())
	{
		s += ", ";
		s += msPassport;
	}
	if (!msTelephone.IsEmpty())
	{
		s += ", tel ";
		s += msTelephone;
	}
	if (!msAddress.IsEmpty())
	{
		s += ", add ";
		s += msAddress;
	}
	if (!msEmail.IsEmpty())
	{
		s += ", email ";
		s += msEmail;
	}
	if (!msRole.IsEmpty())
	{
		s += ", ";
		s += msRole;
	}
	if (!msComment.IsEmpty())
	{
		s += "\r\n";
		s += msComment;
	}
	return s;
}
void CPerson::SaveToXml(CXMLDump& xmlDump)
{
	CXMLDumpScope scope(L"contact", xmlDump);
	xmlDump.Write(L"private_name", msPrivateName);
	xmlDump.Write(L"family_name", msFamilyName);
	xmlDump.Write(L"telephone", msTelephone);
	xmlDump.Write(L"id", msPassport);
	xmlDump.Write(L"address", msAddress);
	xmlDump.Write(L"email", msEmail);
	xmlDump.Write(L"role", msRole);
	xmlDump.Write(L"comment", msComment);
}
void CPerson::SaveContactsToXml(CXMLDump& xmlDump)
{
	if (gContacts.IsEmpty())
		return;

	CXMLDumpScope scope(L"contacts", xmlDump);

	POSITION pos = gContacts.GetHeadPosition();
	while (pos)
	{
		CPerson* pPerson = gContacts.GetNext(pos);
		pPerson->SaveToXml(xmlDump);
	}
}
void CPerson::ClearContacts(void)
{
	while (!gContacts.IsEmpty())
	{
		delete gContacts.GetTail();
		gContacts.RemoveTail();
	}
}
void CPerson::LoadContactsFromXml(CXMLParseNode* pRoot)
{
	CXMLParseNode* pMain = pRoot->GetFirst(L"contacts");
	if (!pMain)
		return;

	CXMLParseNode* pContact = pMain->GetFirst(L"contact");
	while (pContact)
	{
		CPerson* pPerson = new CPerson();
		pPerson->LoadFromXml(pContact);
		gContacts.AddTail(pPerson);
		pContact = pMain->GetNext(L"contact", pContact);
	}
}
void CPerson::LoadFromXml(CXMLParseNode* pContact)
{
	pContact->GetValue(L"private_name", msPrivateName);
	pContact->GetValue(L"family_name", msFamilyName);
	pContact->GetValue(L"telephone", msTelephone);
	pContact->GetValue(L"id", msPassport);
	pContact->GetValue(L"address", msAddress);
	pContact->GetValue(L"email", msEmail);
	pContact->GetValue(L"role", msRole);
	pContact->GetValue(L"comment", msComment);
}

