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
void CSaver::ResetAllInputs(void)
{
	if (gpDlg)
		gpDlg->ResetAllInputs();
	gWorkPeriod.Reset();
	CPerson::ClearContacts();
}
void CSaver::Save(const wchar_t *zfName)
{
	if (zfName)
		msfName = zfName;
	else
	{
		msfName = CUtils::GetBaseDir();
		msfName += "Save";
		if (!CUtils::VerifyDirectory(msfName))
			return;
		msfName += L"\\Last.xml";
		gUsedVacations.Log();
	}

	SaveToXml();

	WriteLetter();
}
bool CSaver::Restore(const wchar_t* zfName)
{
	ResetAllInputs();
	if (zfName)
		msfName = zfName;
	else
	{
		msfName = CUtils::GetBaseDir();
		msfName += L"Save\\Last.xml";
	}

	CFileName fName(msfName);
	if (fName.IsOfType(L"xml"))
		return LoadFromXmlFile();

	return LoadFromTxtFile();
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

	gAllRights.SaveToXml(xmlDump);
	xmlDump.Write(L"software_version", gConfig.msVersion);

	xmlDump.Close();
}
/*
void CSaver::SaveToTxtFile(void)
{
	mpfWrite = MyFOpenWithErrorBox(msfName, L"w, ccs=UNICODE", L"for saving");
	if (!mpfWrite)
		return;

	// unsigned char bom[] = { 0xEF,0xBB,0xBF };
	// fwrite((char*)bom, sizeof(bom), 1, mpfWrite);

	POSITION pos =gpDlg->mEditBoxes.GetHeadPosition();
	while (pos)
	{
		CEditRef *pRef = gpDlg->mEditBoxes.GetNext(pos);
		SaveEditBox(mpfWrite, pRef);
	}

	fwprintf(mpfWrite, L"Holidays\n");
	CString sHolidaysSelectText;
	gpDlg->mComboHolidays.GetWindowTextW(sHolidaysSelectText);
	fwprintf(mpfWrite, L"%s\n", (const wchar_t *)sHolidaysSelectText);

	pos = gpDlg->mButtons.GetHeadPosition();
	while (pos)
	{
		CButtonRef *pRef = gpDlg->mButtons.GetNext(pos);
		SaveButton(mpfWrite, pRef);
	}

	gWorkPeriod.Save(mpfWrite);
	
	sw->WriteLine("TextBoxes");
	for (int i = 0; i < N_BOXES; i++)
	{
		sw->WriteLine(i.ToString());
		sw->WriteLine(maBoxes[i]->Name);
		sw->WriteLine(maBoxes[i]->Text);
	}
	sw->WriteLine("end");

	sw->WriteLine("Dates");
	for (int i = 0; i < N_DATES; i++)
	{
		sw->WriteLine(maDates[i]->Name);
		sw->WriteLine(maDates[i]->Value.Year);
		sw->WriteLine(maDates[i]->Value.Month);
		sw->WriteLine(maDates[i]->Value.Day);
	}
	sw->WriteLine("end");

	sw->WriteLine("CheckBoxes");
	for (int i = 0; i < N_CHECK_BOXES; i++)
	{
		sw->WriteLine(i.ToString());
		sw->WriteLine(maCheckBoxes[i]->Name);
		if (maCheckBoxes[i]->CheckState == CheckState::Checked)
			sw->WriteLine("1");
		else
			sw->WriteLine("0");
	}

	sw->WriteLine("end");
	sw->WriteLine("------------");
	mRights->Save(sw);
	sw->WriteLine("end");
	
	fwprintf(mpfWrite, L"end\n");
	fwprintf(mpfWrite, L"------------------\n");

	gAllRights.Save(mpfWrite);
	fclose(mpfWrite);
}*/
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
	CXMLParse XMLParse(msfName, true);
	CXMLParseNode* pRoot = XMLParse.GetRoot();
	if (!pRoot)
		return false;

	gpDlg->mbDisableComputations = true;

	gpDlg->LoadFromXml(pRoot);
	gWorkPeriod.LoadFromXml(pRoot);
	CPerson::LoadContactsFromXml(pRoot);

	gpDlg->mbDisableComputations = false;
	gpDlg->OnInputChange();
	return true;
}
bool CSaver::LoadFromTxtFile()
{
	mpfRead = MyFOpenWithErrorBox(msfName, L"r, ccs=UNICODE", L"for restoring");
	if (!mpfRead)
		return false;

	gpDlg->mbDisableComputations = true;
	//unsigned char bom[] = { 0xEF,0xBB,0xBF };
	//unsigned char rbom[3];
	//fread((char*)rbom, sizeof(rbom), 1, mpfRead);

	CString sText1 = CUtils::ReadLine(mpfRead);
	CString sText2 = CUtils::ReadLine(mpfRead);
	while (!sText1.IsEmpty() && !sText2.IsEmpty())
	{
		bool bFound = false;
		POSITION pos = gpDlg->mEditBoxes.GetHeadPosition();
		while (pos)
		{
			CEditRef *pRef = gpDlg->mEditBoxes.GetNext(pos);
			if (pRef->msName == sText1)
			{
				pRef->mEdit.SetWindowTextW(sText2);
				bFound = true;
				break;
			}
		}

		if (!bFound && sText1 == L"Holidays")
		{
			bFound = true;
			gpDlg->mComboHolidays.SetWindowTextW(sText2);
		}

		if (!bFound)
		{
			pos = gpDlg->mButtons.GetHeadPosition();
			while (pos)
			{
				CButtonRef *pRef = gpDlg->mButtons.GetNext(pos);
				if (pRef->msName == sText1)
				{
					if (sText2 == "checked")
						pRef->mButton.SetCheck(BST_CHECKED);
					else
						pRef->mButton.SetCheck(BST_UNCHECKED);
					bFound = true;
					break;
				}
			}
		}

		sText1 = CUtils::ReadLine(mpfRead);
		if (sText1 == "WorkPeriod")
		{
			gWorkPeriod.Restore(mpfRead);
			break;
		}
		if (sText1.Left(3) == L"end")
			break;
		sText2 = CUtils::ReadLine(mpfRead);
	}
	fclose(mpfRead);

	gpDlg->mbDisableComputations = false;
	gpDlg->OnInputChange();
	return true;
}
