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
	double GetNDaysPerYear(int seniority, double nDaysPerWeek);

	bool InitFromTextFile(void);
	bool InitFromXmlFile(void);
	void SaveXmlFile(void);
	bool PrintLog(const char *zAt);

	static const int MAX_VETEK = 30;

	int mn;
	CVacationVetek * map[MAX_VETEK];

	bool StartComputingForUsedVacations();
	CMyTime mNextMonthToComputeFrom;
	double mDueVacationLeft;
	int mnVacationsComputed;
	FILE *mpfLog;
	bool ComputeNextVacation(class CVacationUsed &vacation);
};

extern CVacationTable gVacationTable;
