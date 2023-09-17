#pragma once
#include "DaysSpan.h"

class CVacationUsed : public CDaysSpan
{
public:
	CVacationUsed(CMyTime &firstDay, CMyTime &lastDay);
	~CVacationUsed();

	virtual void SaveToXml(class CXMLDump& xmlDump);
	virtual CString GetText();
	virtual void ShortLog(FILE* pf, bool bNewLine = true);
	virtual void LongLog(FILE* pf);

	bool mbIsMaternityLeave;

	CDaysSpan mUnpaidSpan;
	//CMyTime mFirstDayUnpaid;
	int mnWorkDays;
	void Compute();
	int mnPaid;
	int mnUnPaid;
	//int mnUnpaidCalendarDays;
	int mnUnpaidUsedForSeverance;
	void SetPartiallyPaid(int nPaidDays);

	void FindUnpaidSpan();
	int CountDaysOfUnpaidVacation(CMyTime& first, CMyTime& last);
	virtual void AddToWorkSpan(class CWorkSpan& workSpan);
};

