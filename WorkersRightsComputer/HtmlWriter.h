#pragma once
class CHtmlWriter
{
public:
	CHtmlWriter();
	~CHtmlWriter();
	int WriteLetter();
	FILE *mpfWrite;
	CString msfName;
	void BREOL();
	void EOL();
	void Write(const wchar_t *zText);
	void WriteL(const wchar_t *zText);
	void WriteLine(const wchar_t *zText);
	void WriteHeadline(const wchar_t *zText, int iH = 1);
	void WriteTable();
};

