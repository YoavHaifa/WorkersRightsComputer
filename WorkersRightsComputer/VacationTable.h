#pragma once
#include "MyTime.h"

struct CVacationVetek
{
	int mYears;
	int mFromYear;
	int mFromMonth;
	int m6;
	int m5;
};


class CVacationTable
{
public:
	CVacationTable();
	~CVacationTable();

	double GetNDaysPerMonth(int seniority, double nDaysPerWeek, int year, int month);

	bool InitFromTextFile(void);
	bool InitFromXmlFile(void);
	void SaveXmlFile(void);
	bool PrintLog(const char *zAt);

	static const int MAX_VETEK = 30;

	int mn;
	CVacationVetek * map[MAX_VETEK];

	bool StartComputingForUsedVacations();
	CMyTime mNextComputeFrom;
	double mDueVacationLeft;
	int mnVacationsComputed;
	FILE *mpfLog;
	bool ComputeNextVacation(class CVacationUsed &vacation);
};

extern CVacationTable gVacationTable;
