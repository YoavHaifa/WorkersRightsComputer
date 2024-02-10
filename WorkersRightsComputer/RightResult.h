#pragma once

class CRightResult
{
public:
	CRightResult(class CXMLParseNode* pNode);
	void ReadLog(const wchar_t* zfName);
	double SelectOldDue(double newDue);

	CString msName;
	double mDueFromXml;
	double mDueFromLog;
	bool mbLogRead;
	bool mbUseLog;
};
