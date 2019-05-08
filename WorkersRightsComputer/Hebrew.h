#pragma once


class CPair
{
public:
	CPair(const wchar_t *z, const wchar_t *zHebrew)
		: ms(z)
		, msh(zHebrew)
	{
	}
	CString ms;
	CString msh;
	CPair *mpNext;
};

class CHebrew
{
public:
	CHebrew(void);

	CString Get(const wchar_t *z);

private:
	bool InitFromFile(void);
	void WriteToFile(void);
	CPair *mpFirst;
	CPair *mpLast;
	CString msNone;
	bool mbValid;
	int mn;
public:
	void TestRead();
	void TestWrite();
};

extern CHebrew gHebrew;

