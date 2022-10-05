#pragma once
#include "WorkPeriod.h"

class CEditRef
{
public:
	CEditRef(const wchar_t *zName, CEdit &edit, const wchar_t *zOldName = NULL, bool bConnect = true)
		: msName(zName)
		, mEdit(edit)
		, msOldName(zOldName)
		, mbConnect(bConnect)
	{}
	CString msName;
	CEdit &mEdit;
	bool mbConnect;
	CString msOldName;
};

class CButtonRef
{
public:
	CButtonRef(const wchar_t *zName, CButton &button, const wchar_t *zOldName = NULL, bool bConnect = true)
		: msName(zName)
		, mButton(button)
		, msOldName(zOldName)
		, mbConnect(bConnect)
	{}
	CString msName;
	CButton &mButton;
	CString msOldName;
	bool mbConnect;
	bool IsChecked(void)
	{
		return mButton.GetCheck() == BST_CHECKED;
	}
};

class CRight abstract
{
public:
	CRight(const wchar_t *zName, const wchar_t *zHebrewName);
	virtual ~CRight(void);

	void Init(void);
	void WriteLineToHtmlTable(class CHtmlWriter &html);

	bool ComputeEnvelop(void);
	virtual bool Compute(void) = 0;
	virtual	bool SetCheckRef(CButtonRef * /*pButton*/) { return false; }
	virtual	bool SetEditRef(CEditRef * /*pRef*/) { return false; }

	virtual bool HasLegalValue();
	virtual bool HasLegalRealValue();
	virtual bool PrintToLetter();

	virtual CString GetRightNameForLetter(void);
	virtual CString GetDecriptionForLetter(void);
	virtual CString GetDecriptionForLetterHebrew(void);

	
	void LogLine(const wchar_t* zText);
	void LogLine(const wchar_t* zText, CString s);
	void LogLine(const wchar_t *zText, int value);
	void LogLine(const wchar_t *zText, int ivalue, const wchar_t *zText2, double dvalue);
	void LogLine(const wchar_t *zText, double value);
	void LogLine(const wchar_t *zText, __int64 value);
	void LogLineSpan(const wchar_t *zText, CMyTime &start, CMyTime &end);
	void LogLine(const wchar_t *zText, CTime value);
	void LogLine(const wchar_t* zText, CMyTime value);
	void LogLine(const wchar_t* zText, CMyTime value, const wchar_t* zText2);
	void LogLine(const wchar_t* zText, CTime value, const wchar_t* zText2);
	void LogLine(const wchar_t *zText, CTime value, const wchar_t *zText2, int value2);
	void LogLine(const wchar_t *zText, CTime value, const wchar_t *zText2, double value2);

	void Save(FILE *pfWrite);
	void SaveToXml(class CXMLDump& xmlDump);

	static bool umbOldStyle;
	CString msName;
	CString msNameHebrew;
	bool mbValid;
	bool mbSkipIfZero;
	double mDuePay;
	CString msDue;
	CString msToLetter;
	CString msDebug;
	CString mLabel;
	int miPrintOrder;
	//static CString mStatusLabel;

	FILE *mpfWrite;
	int mDebug;

	static CString umsName;

	static void SetSaveDirAndName(const wchar_t *zSaveDir, const wchar_t *zName);
	static void ResetSaveDirAndName(void);
	static FILE * OpenFile(const wchar_t *zfName, const wchar_t *zExtension);
	static CString GetSaveFileName(const wchar_t *zfName, const wchar_t *zExtension);
	bool TryReadInt(FILE *pfRead, const wchar_t *zText, int &value);
	static bool TryConvertInt(const wchar_t *zSource, const wchar_t *zText, int &value);
	int SafeGetIntFromTextBox(CEdit &box);
	double SafeGetDoubleFromTextBox(CEdit &box);
	CString  GetSumLineForLetter(void);
	void WriteLine(const wchar_t *zLine = NULL);
	static void WriteLine(FILE *pfWrite, const wchar_t *zLine = NULL);
	static CString ToDayString(int value);
	static CString ToString(int value);
	static CString ToString(double value);
	static CString ToString(__int64 value);
	static CString ToString(double value, int nDigits);
	static CString ToString(CTime value);
	static bool GetIntFromEditBox(CEdit *pEdit, const wchar_t *zName, int &value);

	static CString GetSaveDir(void);
	bool IsNumber(CString& s);
	bool GetInputNumber(CEdit *pEdit, CString& os, double &oValue);

private:
	static CString umsSaveDir;
};

