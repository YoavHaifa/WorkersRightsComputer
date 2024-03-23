#pragma once
#include "MyTime.h"

class CDaysSpan
{
public:
	CDaysSpan();
	CDaysSpan(const CMyTime& firstDay, const CMyTime& lastDay);
	CDaysSpan(const CDaysSpan& other);
	CDaysSpan(class CXMLParseNode* pNode);

	void InitDaysSpan(const CMyTime& firstDay, const CMyTime& lastDay);
	void SetMonth(const CMyTime &date);

	void SaveToXml(class CXMLDump& xmlDump);

	bool ClipByWorkPeriodEnd();

	bool Overlaps(CDaysSpan& other);
	bool Contains(const CDaysSpan& other);
	bool Contains(const CMyTime& day);
	bool Intersect(CDaysSpan& other, CDaysSpan& oCommon);

	void UpdateMonthlyInfo4Unpaid(bool bNoPension);

	void Log(FILE* pf, const char* zText=NULL);

	CMyTime mFirstDay;
	CMyTime mLastDay;
	CMyTime mDayAfter;
	int mnDays;
	int mnWorkDays;
};

