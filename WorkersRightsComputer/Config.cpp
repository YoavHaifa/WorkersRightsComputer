#include "stdafx.h"
#include "Config.h"
#include "XmlParse.h"

CConfig gConfig;

CConfig::CConfig(void)
{
}
void CConfig::InitFromXml()
{
	CXMLParse xmlParse(L"..\\release\\Config.xml");
	CXMLParseNode* pRoot = xmlParse.GetRoot();
	if (!pRoot)
		return;

	pRoot->GetValue(L"version", msVersion);
	pRoot->GetValue(L"save_dir", msSaveRoot);
}
