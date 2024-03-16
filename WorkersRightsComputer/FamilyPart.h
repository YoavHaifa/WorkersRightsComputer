#pragma once
#include "MyTime.h"
#include "WorkPeriod.h"

class CFamilyPart
{
public:
	CFamilyPart();
	CFamilyPart(CFamilyPart &other);
	~CFamilyPart();

	void Clear(void);
	void Copy(CFamilyPart &other);
	void ClearLast(void);
	bool CheckStartTime(class CMyTime &startTime);
	bool AddPeriod(class CCompanyPartPeriod* pNewPeriod);
	class CCompanyPartPeriod* GetLastPeriod();

	double GetRatio() { return mRatio; }
	void SetRatio(double ratio);
	static CString Ratio2S(double ratio);
	const wchar_t * GetSRatio() { return msRatio; }

	bool mbAskOnlyForFamilyPart;
	bool mbDefined;
	bool mbLoadingFromXml;

	CList<CCompanyPartPeriod *, CCompanyPartPeriod *> mPeriods;
	CString GetFullText();
	CString GetShortText();
	void Compute();
	void SaveToXml(class CXMLDump &xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

	//void Save(FILE *pfSave);
	void WriteToLetter(class CHtmlWriter &writer);

private:
	double mRatio;
	CString msRatio;
};

extern CFamilyPart gFamilyPart;
