#pragma once


class CSeniority
{
public:
	CSeniority(const wchar_t *zName);

	CString msName;

	static const int MAX_SENIORITY = 101;
	bool InitFromFile(void);
	bool PrintLog(void);
	bool mbValid;
	int ma[MAX_SENIORITY];
	bool mabDefined[MAX_SENIORITY];
};

