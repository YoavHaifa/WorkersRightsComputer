#include "stdafx.h"
#include "Saver.h"
#include "Utils.h"
#include "Right.h"
#include "WorkersRightsComputerDlg.h"
#include "Resource.h"
#include "AllRights.h"
#include "UsedVacations.h"
#include "XmlDump.h"
#include "HtmlWriter.h"


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
void CSaver::Save(const wchar_t *zfName)
{
	if (zfName)
		msfName = zfName;
	else
	{
		msfName = L"..\\release\\Save\\Last.txt";
		gUsedVacations.Log();
	}

	SaveToFile();
	SaveToXml();

	WriteLetter();
}
void CSaver::Restore(const wchar_t *zfName)
{
	if (zfName)
		msfName = zfName;
	else
		msfName = L"..\\release\\Save\\Last.txt";

	LoadFromFile();

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

	xmlDump.Close();
}
void CSaver::SaveToFile(void)
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
	/*
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
	*/
	fwprintf(mpfWrite, L"end\n");
	fwprintf(mpfWrite, L"------------------\n");

	gAllRights.Save(mpfWrite);
	fclose(mpfWrite);
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
	CString sName(gpDlg->GetText(IDC_EDIT_FIRST_NAME));
	sName += "_";
	sName += gpDlg->GetText(IDC_EDIT_FAMILY_NAME);
	sName += "_";
	sName.Replace(L" ", L"_");

	// SAVE
	// Set Target Directory
	CString sSaveDir(L"C:\\WorkersRights\\Save\\");
	sSaveDir += sName;
	sSaveDir += gpDlg->GetText(IDC_EDIT_ID);
	CUtils::VerifyDirectory(sSaveDir);
	sSaveDir += "\\";

	msfName = sSaveDir;
	msfName += L"Save.txt";
	SaveToFile();
	SaveToXml();

	CRight::SetSaveDirAndName(sSaveDir, sName);
	gpDlg->OnInputChange(); //  Recompute all and save all relevant logs to special dir
	{
		CHtmlWriter writer;
		writer.WriteLetter();
	}
	{
		CHtmlWriter writer;
		writer.WriteLetterFromTemplate();
	}

	CRight::ResetSaveDirAndName();
}
void CSaver::LoadFromFile()
{
	mpfRead = MyFOpenWithErrorBox(msfName, L"r, ccs=UNICODE", L"for restoring");
	if (!mpfRead)
		return;

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
		if (sText1 == L"end")
			break;
		sText2 = CUtils::ReadLine(mpfRead);
	}
	fclose(mpfRead);

	gpDlg->mbDisableComputations = false;
	gpDlg->OnInputChange();
}
