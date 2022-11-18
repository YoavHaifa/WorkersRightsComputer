#include "stdafx.h"
#include "Config.h"
#include "XmlParse.h"
#include "Utils.h"

CConfig gConfig;

CConfig::CConfig(void)
	: mb14DaysUnpaidVacation4SeveranceDefault(false)
	, mNMonthsForFullVacation(12)
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
	pRoot->GetValue(L"b_14_days_severance", mb14DaysUnpaidVacation4SeveranceDefault);
	pRoot->GetValue(L"n_months_for_full_vacation", mNMonthsForFullVacation);
}
