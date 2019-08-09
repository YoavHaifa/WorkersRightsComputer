#pragma once
#include "WorkYear.h"

class CWorkYears
{
public:
	CWorkYears(void);

	void Compute(void);
	void Clear(void);
	void Log();

	int mn;

private:
	static const int MAX_YEARS = 50;
	CWorkYear maYears[MAX_YEARS];

};

extern CWorkYears gWorkYears;