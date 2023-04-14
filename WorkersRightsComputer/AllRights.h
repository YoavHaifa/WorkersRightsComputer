#pragma once
#include "Right.h"

class CAllRights
{
public:
	CAllRights(void);
	~CAllRights(void);

	bool Init(void);
	void Clear(void);

	// bool SetLabel(const wchar_t *zName, const wchar_t *zLabel);
	// void SetLabelForStatus(const wchar_t *zLabel);
	bool SetCheckRef(CButtonRef *pButton);
	bool SetEditRef(CEditRef *pRef);

	void Save(FILE* pfWrite);
	void SaveToXml(class CXMLDump &xmlDump);

	void WriteLetterToHtml(class CHtmlWriter &html);
	void WriteTotalLineToHtmlTable(class CHtmlWriter &html);

	bool Compute(void);
	bool AllInputDefined();
	bool ComputeInternal();

	CList <CRight *, CRight *> mRights;
	double mSumDue;
	CString msLabelTotal;

	class CHolidays *mpHolidays;
	CString GetHolidaysSelection();
	class CHolidays* GetHolidays();
};

extern CAllRights gAllRights;