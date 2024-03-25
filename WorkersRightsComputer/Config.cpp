#include "stdafx.h"
#include "Config.h"
#include "XmlParse.h"
#include "XmlDump.h"
#include "Utils.h"

CConfig gConfig;

CConfig::CConfig(void)
	: mb14DaysUnpaidVacation4SeveranceDefault(false)
	, miVersion(136)
	, miLegacyVersion(0)
	, mbBackwardCompatibilityMode(false)
	, mbCaregiversOnly(true)
	, mbAllowPartialPrevYearsVacation(false)
{
}
void CConfig::InitFromXml()
{
	CString sfName = CUtils::GetBaseDir();
	sfName += L"Config.xml";
	CXMLParse xmlParse(sfName, true /*bUnicode*/);
	CXMLParseNode* pRoot = xmlParse.GetRoot();
	if (!pRoot)
		return;

	// CString sHebrew;
	// pRoot->GetValue(L"try_hebrew", sHebrew);

	pRoot->GetValue(L"version", msVersion);
	pRoot->GetValue(L"save_dir", msSaveRoot);

	// Control internal computationsand GUI
	pRoot->GetValue(L"b_14_days_severance", mb14DaysUnpaidVacation4SeveranceDefault);
	pRoot->GetValue(L"b_caregivers_only", mbCaregiversOnly);
	pRoot->GetValue(L"b_partial_prev_years_vacation", mbAllowPartialPrevYearsVacation);

	//pRoot->GetValue(L"n_months_for_full_vacation", mNMonthsForFullVacation);

	RestoreState();
}

bool CConfig::SaveState()
{
	CString sDir = CUtils::GetBaseDir();
	CXMLDump dump((const wchar_t*)sDir, L"State", L"latest");

	dump.Write(L"save_dir", msSaveRoot);
	dump.Write(L"filled_by_english",msFilledBy );
	dump.Write(L"filled_by_hebrew", msFilledByHebrew);
	dump.Write(L"contact_phone", msContactPhone);
	dump.Write(L"contact_fax", msContactFax);
	dump.Write(L"contact_email", msContactEmail);

	return true;
}

bool CConfig::RestoreState()
{
	CString sDir = CUtils::GetBaseDir();
	CXMLParse xmlParse((const wchar_t*)sDir, L"State", L"latest", 
		true /*bUnicode*/, true /*bOptional*/);
	//CXMLParse xmlParse(sDir, true /*bUnicode*/);
	CXMLParseNode* pRoot = xmlParse.GetRoot();
	if (!pRoot)
		return false;

	pRoot->GetValue(L"save_dir", msSaveRoot);
	pRoot->GetValue(L"filled_by_english", msFilledBy);
	pRoot->GetValue(L"filled_by_hebrew", msFilledByHebrew);
	pRoot->GetValue(L"contact_phone", msContactPhone);
	pRoot->GetValue(L"contact_fax", msContactFax);
	pRoot->GetValue(L"contact_email", msContactEmail);
	return true;
}
