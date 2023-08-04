#pragma once
#include "VacationUsed.h"

class CMaternityLeave : public CVacationUsed
{
public:
	CMaternityLeave(CMyTime firstDay, CMyTime lastDay, 
		double nPaidWeeks, bool bPaidWeeksDeservePension);

	virtual void SaveToXml(class CXMLDump& xmlDump);
	virtual CString GetText();
	virtual void ShortLog(FILE* pf);
	virtual void LongLog(FILE* pf);

private:
	double mnPaidWeeks;
	bool mbPaidWeeksDeservePension;
};

