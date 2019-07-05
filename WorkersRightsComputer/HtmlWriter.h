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
	void WriteLine(const wchar_t *zText);
	void WriteHeadline(const wchar_t *zText, int iH = 1);
	void WriteTable(bool bUsingTemplate);
	void ReplaceTemplateVariable(void);
	void Print(const CString &s);
	void StartParagraph(void);
	void EndParagraph(void);
};

