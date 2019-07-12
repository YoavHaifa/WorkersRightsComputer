#pragma once
class CHtmlWriter
{
public:
	CHtmlWriter();
	~CHtmlWriter();
	int WriteLetter();
	int WriteLetterFromTemplate();
	FILE *mpfWrite;
	FILE *mpfRead;
	CString msfName;
	void BREOL();
	void EOL();
	void Write(const wchar_t *zText);
	void WriteL(const wchar_t *zText);
	void WritePara(const wchar_t *zText);
	void WriteLine(const wchar_t *zText);
	void WriteHeadline(const wchar_t *zText, int iH = 1);
	void WriteTable(bool bUsingTemplate);
	void ReplaceTemplateVariable(void);
	void Print(const CString &s);
	void StartParagraph(void);
	void EndParagraph(void);
	void StartTabLine(void);
	void EndTabLine(void);
	void StartPensionTable(void);
	void EndPensionTable(void);
	void Write2Tab(CString &sItem);
	void Write2Tab(const char *zText);
	void Write2Tab(double value);
	void Write2Tab(const char *zFormat, double value);

};

