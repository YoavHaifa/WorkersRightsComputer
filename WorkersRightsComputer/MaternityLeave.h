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

private:
	int mnPaidWeeks;
	double mnWeeks;
	bool mbPaidWeeksDeservePension;
};

