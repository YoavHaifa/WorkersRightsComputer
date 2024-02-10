#include "stdafx.h"
#include "RightResult.h"
#include "XmlParse.h"
#include "Utils.h"

CRightResult::CRightResult(CXMLParseNode* pNode)
	: msName(pNode->msName)
	, mDueFromXml(0)
	, mDueFromLog(0)
	, mbLogRead(false)
	, mbUseLog(false)
{
	pNode->GetValue(L"Due", mDueFromXml);
}
void CRightResult::ReadLog(const wchar_t* zfName)
{
	FILE* pfRead = MyFOpenWithErrorBox(zfName, L"r", L"Saved Log File");
	if (!pfRead)
		return;
	CString sTitle(msName);
	sTitle += ":";

	CString sLine = CUtils::ReadLine(pfRead);
	while (!sLine.IsEmpty())
	{
		if (sLine.Find(sTitle) == 0)
		{
			int i = sLine.Find(L"==>");
			if (i > 0)
			{
				const wchar_t* zw = (const wchar_t*)sLine;
				mDueFromLog = _wtof(zw + i + 4);
			}
			else
				mDueFromLog = 0;
			mbLogRead = true;
			break;
		}
		sLine = CUtils::ReadLine(pfRead);
	}

	fclose(pfRead);
}
double CRightResult::SelectOldDue(double newDue)
{
	double oldDue = mDueFromXml;
	if (mbLogRead)
	{
		double xmlDelta = abs(mDueFromXml - newDue);
		double logDelta = abs(mDueFromLog - newDue);
		if (logDelta < xmlDelta)
		{
			oldDue = mDueFromLog;
			mbUseLog = true;
		}
	}
	return oldDue;
}
