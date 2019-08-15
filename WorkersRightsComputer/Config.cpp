#include "stdafx.h"
#include "Config.h"
#include "XmlParse.h"
#include "Utils.h"

CConfig gConfig;

CConfig::CConfig(void)
{
}
void CConfig::InitFromXml()
{
	CString sfName = CUtils::GetBaseDir();
	sfName += L"Config.xml";
	CXMLParse xmlParse(sfName);
	CXMLParseNode* pRoot = xmlParse.GetRoot();
	if (!pRoot)
		return;

	pRoot->GetValue(L"version", msVersion);
	pRoot->GetValue(L"save_dir", msSaveRoot);
}
