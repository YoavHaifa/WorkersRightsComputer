#pragma once
#include "MyTime.h"

class CVacationUsed
{
public:
	CVacationUsed(CMyTime &firstDay, CMyTime &lastDay);
	~CVacationUsed();

	virtual void SaveToXml(class CXMLDump& xmlDump);
	virtual CString GetText();
	virtual void ShortLog(FILE* pf, bool bNewLine = true);
	virtual void LongLog(FILE* pf);

	bool mbIsMaternityLeave;

	CMyTime mFirstDay;
	CMyTime mFirstDayUnpaid;
	CMyTime mLastDay;
	int mnDays;
	int mnWorkDays;
	void Compute();
	int mnPaid;
	int mnUnPaid;
	int mnUnpaidCalendarDays;
	int mnUnpaidUsedForSeverance;
	void SetPartiallyPaid(int nPaidDays);

	int CountDaysOfUnpaidVacation(CMyTime& first, CMyTime& last);
	void AddToWorkSpan(class CWorkSpan& workSpan);
};

