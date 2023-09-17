#include "stdafx.h"
#include "Saver.h"
#include "Utils.h"
#include "Right.h"
#include "WorkersRightsComputerDlg.h"
#include "Resource.h"
#include "AllRights.h"
#include "UsedVacations.h"
#include "XmlDump.h"
#include "XmlParse.h"
#include "HtmlWriter.h"
#include "Config.h"
#include "Person.h"
#include "FileName.h"
#include "Comments.h"
#include "HolidaysDue.h"


CSaver::CSaver()
	: mpfWrite(NULL)
	, mpfRead(NULL)
{
}
CSaver::~CSaver()
{
	if (mpfWrite)
		fclose(mpfWrite);
	if (mpfRead)
		fclose(mpfRead);
}
void CSaver::ResetAllInputs(bool bLoading)
{
	if (gpDlg)
		gpDlg->ResetAllInputs(bLoading);
	gWorkPeriod.Reset();
	CPerson::ClearContacts();
	gComments.Clear();
}
bool CSaver::Save(const wchar_t *zfName)
{
	if (zfName)
		msfName = zfName;
	else
	{
		msfName = CUtils::GetBaseDir();
		msfName += "Save";
		if (!CUtils::VerifyDirectory(msfName))
			return false;
		msfName += L"\\Last.xml";
		gUsedVacations.Log();
	}

	SaveToXml();

	if (gAllRights.mbComputedOK)
	{
		WriteLetter();
		return true;
	}

	CString s(L"Last Computation Failed, Letter Not Saved.\r\n");
	s += gAllRights.msError;
	CUtils::MessBox(s, L"Warning");
	return false;
}
bool CSaver::Restore(const wchar_t* zfName)
{
	if (zfName)
		msfName = zfName;
	else
	{
		msfName = CUtils::GetBaseDir();
		msfName += L"Save\\Last.xml";
	}

	CFileName fName(msfName);
	if (fName.IsOfType(L"xml"))
	{
		ResetAllInputs(true /*bLoading*/);
		bool bOK = LoadFromXmlFile();

		gHolidaysDue.UpdateMainDialog();
		gComments.OnLoad();
		gpDlg->OnLoad();

		return bOK;
	}

	CUtils::MessBox(L"Saved file should be XML", L"Input Error");
	return false;
}
void CSaver::SaveToXml(void)
{
	CFileName fName(msfName);
	fName.ChangeType(L"xml");
	CXMLDump xmlDump((const wchar_t *)fName, L"WorkersRights");
	if (!xmlDump)
		return;

	gpDlg->SaveToXml(xmlDump);

	gWorkPeriod.SaveToXml(xmlDump);
	CPerson::SaveContactsToXml(xmlDump);
	gComments.SaveToXml(xmlDump);

	gAllRights.SaveToXml(xmlDump);
	xmlDump.Write(L"software_version", gConfig.msVersion);

	xmlDump.Close();
}
void CSaver::SaveEditBox(FILE *pfSave, CEditRef *pRef)
{
	CString sText;
	pRef->mEdit.GetWindowText(sText);
	if (sText.IsEmpty())
		sText = L"0";
	fwprintf(pfSave, L"%s\n%s\n", (const wchar_t *)pRef->msName, (const wchar_t *)sText);
}
void CSaver::SaveButton(FILE *pfSave, CButtonRef *pRef)
{
	fwprintf(pfSave, L"%s\n", (const wchar_t *)pRef->msName);
	if (pRef->mButton.GetCheck() == BST_CHECKED)
		fwprintf(pfSave, L"checked\n");
	else
		fwprintf(pfSave, L"not_checked\n");
}
void CSaver::WriteLetter()
{
	// Get Worker's name for save dir & files
	msSaveId = gpDlg->GetText(IDC_EDIT_FIRST_NAME);
	msSaveId += "_";
	msSaveId += gpDlg->GetText(IDC_EDIT_FAMILY_NAME);
	msSaveId += "_";
	msSaveId += gpDlg->GetText(IDC_EDIT_ID);
	msSaveId.Replace(L" ", L"_");

	// SAVE
	// Set Target Directory
	CString sSaveDir(gConfig.msSaveRoot);
	sSaveDir += "\\";
	sSaveDir += msSaveId;
	CUtils::VerifyDirectory(sSaveDir);
	sSaveDir += "\\";

	msSaveId += "_";
	msfName = sSaveDir;
	msfName += msSaveId;
	msfName += L"save.xml";
	SaveToXml();

	if (!gWorkPeriod.IsValid())
		return;

	CString sLogDir = sSaveDir + "Log";
	CUtils::VerifyDirectory(sLogDir);
	sLogDir += "\\";

	CRight::SetSaveDirAndName(sLogDir, msSaveId);
	gpDlg->OnInputChange(); //  Recompute all and save all relevant logs to special dir

	CString sLetterFileName(sSaveDir);
	sLetterFileName += msSaveId;
	sLetterFileName += L"letter_english.html";
	CHtmlWriter writer;
	writer.WriteLetterFromTemplate(sLetterFileName);

	CRight::ResetSaveDirAndName();
}
bool CSaver::LoadFromXmlFile()
{
	if (!CFileName::Exist(msfName))
	{
		CUtils::MessBox(msfName, L"Failed to find input file");
		return false;
	}
	if (CFileName::GetFileLength(msfName) < 10)
	{
		CUtils::MessBox(msfName, L"Input file seems to be empty");
		return false;
	}
	CXMLParse XMLParse(msfName, true);
	CXMLParseNode* pRoot = XMLParse.GetRoot();
	if (!pRoot)
	{
		CUtils::MessBox(msfName, L"Failed to parse input file");
		return false;
	}

	gpDlg->mbDisableComputations = true;

	if (!gpDlg->LoadFromXml(pRoot))
	{
		CUtils::MessBox(msfName, L"Input file is not a properly saved file");
		return false;
	}
	gWorkPeriod.LoadFromXml(pRoot);
	CPerson::LoadContactsFromXml(pRoot);
	gComments.LoadFromXml(pRoot);

	gpDlg->mbDisableComputations = false;
	gpDlg->OnInputChange(true);
	return true;
}
