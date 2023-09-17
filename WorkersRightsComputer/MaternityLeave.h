#pragma once
#include "VacationUsed.h"

class CMaternityLeave : public CVacationUsed
{
public:
	CMaternityLeave(CMyTime firstDay, CMyTime lastDay, 
		int nPaidWeeks, bool bPaidWeeksDeservePension);

	virtual void SaveToXml(class CXMLDump& xmlDump);
	virtual CString GetText();
	virtual void ShortLog(FILE* pf);
	virtual void LongLog(FILE* pf);

	virtual void AddToWorkSpan(class CWorkSpan& workSpan);

private:
	void Compute();

	CDaysSpan mPaidSpan;

	int mnPaidWeeks;
	double mnWeeks;
	bool mbPaidWeeksDeservePension;
};

