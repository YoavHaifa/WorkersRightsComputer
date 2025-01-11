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
#include "LetterSavedDlg.h"


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

	// Get Worker's name for save dir & files
	if (!gpDlg->GetSaveId(msSaveId))
		return false;

	if (!VerifySaveDirAndSaveToXml())
		return false;

	if (!gWorkPeriod.PeriodIsValid())
	{
		CLetterSavedDlg dlg;
		dlg.SetNewTitle(L"Case Saved - but not enough information to compute rights");
		dlg.SetFileName(msfName);
		dlg.SetWarning(L"Work Period not well defined. Letter can't be prepared.");
		dlg.DoModal();
		return false;
	}

	if (gAllRights.mbComputedOK)
	{
		return WriteLetter();
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

	// Print time of save
	time_t saveTime = time(NULL);
	wchar_t wzBuf[256];
	_wctime_s(wzBuf, 256, &saveTime);
	xmlDump.Write(L"save_time", wzBuf);

	xmlDump.Write(L"software_version", gConfig.msVersion);
	xmlDump.Write(L"i_software_version", gConfig.miVersion);

	xmlDump.Close();
	SaveXmlAsOldCopy(xmlDump.msName);
}
void CSaver::SaveXmlAsOldCopy(const wchar_t* zfName)
{
	CFileName fName(zfName);
	CString sPrivate = fName.Private();
	if (sPrivate == "Last.xml")
		return;

	CString sPath = fName.Path();
	CString sSavePath = sPath + "\\OldSaves";
	if (!CUtils::VerifyDirectory(sSavePath))
		return;

	wchar_t wsBuf[256];
	int MAX_SAVES = 100;
	CString sSavePattern = sSavePath + "\\" + fName.PrivateWithoutType() + "_%02d.xml";
	int i = 1;
	for (; i <= MAX_SAVES; i++)
	{
		swprintf_s(wsBuf, 256, (const wchar_t*)sSavePattern, i);
		if (!CFileName::Exist(wsBuf))
			break;
	}
	if (i > MAX_SAVES)
		return;

	CopyFile(zfName, wsBuf, true);
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
bool CSaver::VerifySaveDirAndSaveToXml()
{
	// Set Target Directory
	msSaveDir = gConfig.msSaveRoot;
	msSaveDir += "\\";
	msSaveDir += msSaveId;
	if (!CUtils::VerifyDirectory(msSaveDir))
		return false;

	msSaveDir += "\\";

	msSaveId += "_";
	msfName = msSaveDir;
	msfName += msSaveId;
	msfName += L"save.xml";
	SaveToXml();
	return true;
}
bool CSaver::WriteLetter()
{
	CString sLogDir = msSaveDir + "Log";
	CUtils::VerifyDirectory(sLogDir);
	sLogDir += "\\";

	CRight::SetSaveDirAndName(sLogDir, msSaveId);
	gpDlg->OnInputChange(); //  Recompute all and save all relevant logs to special dir

	CString sLetterFileName(msSaveDir);
	sLetterFileName += msSaveId;
	sLetterFileName += L"letter_english.html";
	CHtmlWriter writer;
	writer.WriteLetterFromTemplate(sLetterFileName);

	CRight::ResetSaveDirAndName();
	return true;
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
