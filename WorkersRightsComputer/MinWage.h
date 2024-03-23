#pragma once

struct CLevel
{
	int mYear;
	int mMonth;
	double mWage;

	void Log(FILE *pf)
	{
		fprintf(pf, "%4d %2d - %.2f\n", mYear, mMonth, mWage);
	}
};

class CMinWage
{
public:
	CMinWage(void);

	bool IsValid(void);

	bool InitFromXmlFile(void);
	//bool InitFromTextFile(void);
	//void PrintLog(void);
	//double ComputeMonthlyPay(const class CMyTime& date);
	double GetMonthlyWage(const class CMyTime& date);

private:
	//void SaveMonthlyXmlFile();
	class CMonthlyRates* mpMonthlyRates;

	//int mn;
	//CLevel **map;
};

extern CMinWage gMinWage;
