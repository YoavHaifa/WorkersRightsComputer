#pragma once


class CMonthlyRates
{
public:
	CMonthlyRates(const wchar_t *zName, int firstYear);
	//bool InitFromTextFile();
	//void SaveMonthlyXmlFile();
	bool InitFromXmlFile();
	bool PrintLog(const wchar_t* zAt);
	double RatePerMonth(const class CMyTime &date);

	CString msName;

	static const int MAX_RATES = 1212; // 101 years * 12 months
	const int mFirstYear;

	bool mbValid;
	double ma[MAX_RATES]; 
	bool mabDefined[MAX_RATES]; 

	// Remember definitions for convert to monthly rates from text to XML
	//int mnDefs;
	//int maDefYear[MAX_RATES];
	//int maDefMonth[MAX_RATES];
	//double maDefRate[MAX_RATES];
};

