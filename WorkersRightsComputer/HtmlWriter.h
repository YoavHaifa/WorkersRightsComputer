#pragma once
class CHtmlWriter
{
public:
	CHtmlWriter();
	~CHtmlWriter();
	bool CopyLogo(const wchar_t* zfName);
	int WriteLetterFromTemplate(const wchar_t* zfName);
	FILE* mpfWrite;
	FILE* mpfHebrewWrite;
	FILE *mpfRead;
	CString msfName;
	CString msfHebrewName;
	void WriteInt(int value);
	void Write(const wchar_t* zText);
	void WriteEH(const wchar_t* zText, const wchar_t* zHebrewText);
	void WriteL(const wchar_t* zText);
	void WriteLTH_EH(const wchar_t* zText, const wchar_t* zHebrewText);
	void WriteLEH(const wchar_t* zText, const wchar_t* zHebrewText);
	void WritePara(const wchar_t* zText);
	void WriteParaLTR(const wchar_t* zText);
	void WriteLine(const wchar_t* zText);
	void WriteLineEH(const wchar_t* zText, const wchar_t* zHebrewText);
	void WriteLineEH(const wchar_t* zText, const wchar_t* zHebrewText, const wchar_t* zExtraText);
	void WriteHeadline(const wchar_t *zText, int iH = 1);
	void WriteTable(void);
	void ReplaceTemplateVariable(void);
	void Print(const CString& s);
	void PrintEH(const CString& s, const CString& sh);
	void StartParagraph(void);
	void EndParagraph(void);
	void StartTabLine(void);
	void EndTabLine(void);
	void StartPensionTable(void);
	void EndPensionTable(void);
	void Write2Tab(CString &sItem);
	void Write2Tab(const char* zText);
	void Write2TabEH(const wchar_t* zText, const wchar_t* zHebrewText);
	void Write2Tab(double value);
	void Write2Tab(const char *zFormat, double value);

	bool OpenHebrewLetter();
	bool mbEng;
	bool mbHeb;
	void WriteItemToHtmlTable(CString sItem, CString sItemHebrew);

	static CString umsHtmlDir;
};

