#include "StdAfx.h"
#include "Right.h"
#include "Utils.h"
#include "HtmlWriter.h"
#include "XMLDump.h"

CString CRight::umsSaveDir = L"";
CString CRight::umsName = L"__";
bool CRight::umbOldStyle = false;

CRight::CRight(const wchar_t *zName, const wchar_t *zHebrewName)
	: msName(zName)
	, msNameHebrew(zHebrewName)
	, mbSkipIfZero(false)
	, miPrintOrder(-1)
	, mDebug(3)
{
	if (!umsSaveDir)
		ResetSaveDirAndName();

	Init();
}
CRight::~CRight(void)
{
}
CString CRight::GetSaveDir(void)
{
	if (umsSaveDir.IsEmpty())
		ResetSaveDirAndName();
	return umsSaveDir;
}
void CRight::SetSaveDirAndName(const wchar_t *zSaveDir, const wchar_t *zName)
{
	umsSaveDir = zSaveDir;
	umsName = zName;
}
void CRight::ResetSaveDirAndName(void)
{
	umsSaveDir = CUtils::GetBaseDir();
	umsSaveDir += "Log";
	CUtils::VerifyDirectory(umsSaveDir);
	umsSaveDir += "\\";
	// umsSaveDir = L"..\\release\\Log\\";
	umsName = L"__";
}
void CRight::Init(void)
{
	mbValid = false;
	mDuePay = 0;
	msDue = msName + L": ";
	msDebug = msName + L" Debug: ";
}
void CRight::Save(FILE *pfWrite)
{
	WriteLine(pfWrite, msName);
	WriteLine(pfWrite, msDue);
	WriteLine(pfWrite, msDebug);
	WriteLine(pfWrite, L"*");
}
void CRight::SaveToXml(CXMLDump& xmlDump)
{
	CXMLDumpScope scope(msName, xmlDump);

	xmlDump.Write(L"Due", mDuePay);
	xmlDump.Write(L"sDue", msDue);
	xmlDump.Write(L"Debug", msDebug);
}
CString CRight::GetSaveFileName(const wchar_t *zfName, const wchar_t *zExtension)
{
	CString sfName = umsSaveDir;
	sfName += umsName;
	sfName += zfName;
	sfName += ".";
	sfName += zExtension;
	return sfName;
}
FILE * CRight::OpenFile(const wchar_t *zfName, const wchar_t *zExtension)
{
	CString sfName = umsSaveDir;
	sfName += umsName;
	sfName += zfName;
	sfName += ".";
	sfName += zExtension;
	FILE *pf = NULL;
	_wfopen_s(&pf, (const wchar_t *)sfName, L"w");
	return pf;
}
bool CRight::ComputeEnvelop(void)
{
	mpfWrite = CUtils::OpenSpecialLogFile((const wchar_t *)msName);

	CString sLine = L"Computing ";
	sLine += msName;
	if (mpfWrite)
		fwprintf(mpfWrite, L"%s\n\n", (const wchar_t *)sLine);

	bool bOK = Compute();
	if (mpfWrite)
	{
		fwprintf(mpfWrite, L"\n");
		fwprintf(mpfWrite, L"%s\n", (const wchar_t*)msDue);
		fwprintf(mpfWrite, L"%s\n", (const wchar_t*)msDebug);

		if (!bOK)
		{
			WriteLine(L"");
			sLine = msName;
			sLine += L" not computed!";
			WriteLine(sLine);
		}
		fclose(mpfWrite);
		mpfWrite = NULL;
	}

	return bOK;
}
bool CRight::HasLegalValue()
{
	if (mDuePay > 0)
		return true;

	if ((mDuePay == 0) && !mbSkipIfZero)
		return true;

	return false;
}
bool CRight::HasLegalRealValue()
{
	if (mDuePay > 0)
		return true;

	return false;
}
bool CRight::PrintToLetter()
{
	return mDuePay > 0;
}
void CRight::LogLine(const wchar_t *zText)
{
	WriteLine(zText);
}
void CRight::LogLine(const wchar_t* zText, CString s)
{
	CString str(zText);
	str += " ";
	str += s;
	WriteLine(str);
}
void CRight::LogLine(const wchar_t *zText, int value)
{
	CString s(zText);
	s += " ";
	s += ToString(value);
	WriteLine(s);
}
void CRight::LogLine(const wchar_t *zText, double value)
{
	CString s(zText);
	s += " ";
	s += ToString(value);
	WriteLine(s);
}
void CRight::LogLine(const wchar_t *zText, __int64 value)
{
	CString s(zText);
	s += " ";
	s += ToString(value);
	WriteLine(s);
}
void CRight::LogLine(const wchar_t *zText, CTime value)
{
	CString s(zText);
	s += " ";
	s += ToString(value);
	WriteLine(s);
}
void CRight::LogLine(const wchar_t* zText, CMyTime myTime)
{
	CString s(zText);
	s += " ";
	s += myTime.ToString();
	WriteLine(s);
}
void CRight::LogLine(const wchar_t* zText, CMyTime myTime, const wchar_t* zText2)
{
	CString s(zText);
	s += " ";
	s += myTime.ToString();
	s += " ";
	s += zText2;
	WriteLine(s);
}
void CRight::LogLine(const wchar_t *zText, CTime value, const wchar_t *zText2)
{
	CString s(zText);
	s += " ";
	s += ToString(value);
	s += " ";
	s += zText2;
	WriteLine(s);
}
void CRight::LogLine(const wchar_t *zText, CTime value, const wchar_t *zText2, int value2)
{
	CString s(zText);
	s += " ";
	s += ToString(value);
	s += " ";
	s += zText2;
	s += " ";
	s += ToString(value2);
	WriteLine(s);
}
void CRight::LogLine(const wchar_t *zText, CTime value, const wchar_t *zText2, double value2)
{
	CString s(zText);
	s += " ";
	s += ToString(value);
	s += " ";
	s += zText2;
	s += " ";
	s += ToString(value2);
	WriteLine(s);
}
void CRight::LogLine(const wchar_t *zText, int ivalue, const wchar_t *zText2, double dvalue)
{
	CString s(zText);
	s += " ";
	s += ToString(ivalue);
	s += " ";
	s += zText2;
	s += " ";
	s += ToString(dvalue);
	WriteLine(s);
}
void CRight::LogLineSpan(const wchar_t *zText, CMyTime& start, CMyTime& end)
{
	int nDays = start.GetNDaysBefore(end);
	CString s(zText);
	s += L" TIME SPAN days ";
	s += ToString(nDays);
	//s += L" houres ";
	//s += ToString(span.GetHours());
	//s += L" minutes ";
	//s += ToString(span.GetMinutes());
	WriteLine(s);
}
bool CRight::TryReadInt(FILE *pfRead, const wchar_t *zText, int &value)
{
	wchar_t zBuf[256];
	if (fgetws(zBuf, sizeof(zBuf), pfRead) == NULL)
		return false;

	if (mDebug & 0x100)
	{
		CString sDeb(L"Read ");
		sDeb += zText;
		sDeb += L": ";
		sDeb += zBuf;
		WriteLine(sDeb);
	}

	if (!isdigit(zBuf[0]))
	{
		CString s (L"Read Integer Error: <");
		s += zText;
		s += L"> Found: ";
		s += zBuf;
		MessageBox(NULL, s, L"Read Int Error", MB_OK);
		return false;
	}
		
	value = _wtoi(zBuf);
	return true;
}
bool CRight::TryConvertInt(const wchar_t *zSource, const wchar_t *zText, int &value)
{
	if (!isdigit(zSource[0]))
	{
		CString s(L"Convert Integer Error: <");
		s += zText;
		s += L"> Found: ";
		s += zSource;
		MessageBox(NULL, s, L"Read Int Error", MB_OK);
		return false;
	}
	value = _wtoi(zSource);
	return true;
}
int CRight::SafeGetIntFromTextBox(CEdit &box)
{
	CString sText;
	box.GetWindowText(sText);
	if (sText.IsEmpty())
		return 0;

	if (!isdigit(sText[0]))
		return 0;

	int value = _wtoi(sText);
	return value;
}
double CRight::SafeGetDoubleFromTextBox(CEdit &box)
{
	CString sText;
	box.GetWindowText(sText);
	if (sText.IsEmpty())
		return 0;

	if (!isdigit(sText[0]))
		return 0;

	double value = _wtof(sText);
	return value;
}
CString CRight::GetSumLineForLetter(void)
{
	CString s (GetRightNameForLetter());
	s += " - ";
	s += GetDecriptionForLetter();
	s += " = ";
	CString sPay;
	sPay.Format(L"%.2f", mDuePay);
	s += sPay;
	s += "     ";
	s += msNameHebrew;
	return s;
}
CString CRight::GetRightNameForLetter()
{
	return msName;
}
CString CRight::GetDecriptionForLetter()
{
	if (!msToLetter.IsEmpty())
		return msToLetter;
	return msDue;
}
CString CRight::GetDecriptionForLetterHebrew()
{
	return GetDecriptionForLetter();
}
void CRight::WriteLine(const wchar_t *zLine)
{
	if (!mpfWrite)
		return;
	if (zLine)
		fwprintf(mpfWrite, L"%s\n", zLine);
	else
		fwprintf(mpfWrite, L"\n");
}
void CRight::WriteLine(FILE *pfWrite, const wchar_t *zLine)
{
	if (!pfWrite)
		return;

	if (zLine)
		fwprintf(pfWrite, L"%s\n", zLine);
	else
		fwprintf(pfWrite, L"\n");
}
CString CRight::ToDayString(int value)
{
	switch (value)
	{
	case 1:
		return CString(L"Sunday");
	case 2:
		return CString(L"Monday");
	case 3:
		return CString(L"Tuesday");
	case 4:
		return CString(L"Wednesday");
	case 5:
		return CString(L"Thursday");
	case 6:
		return CString(L"Friday");
	case 7:
		return CString(L"Saturday");
	}
	return CString(L"???day");
}
CString CRight::ToString(int value)
{
	CString s;
	s.Format(L"%d", value);
	return s;
}
CString CRight::ToString(double value)
{
	CString s;
	s.Format(L"%.2f", value);
	return s;
}
CString CRight::ToString(__int64 value)
{
	CString s;
	s.Format(L"%I64d", value);
	return s;
}
CString CRight::ToString(double value, int nDigits)
{
	CString sFormat;
	sFormat.Format(L"%%.%df", nDigits);
	CString s;
	s.Format(sFormat, value);
	return s;
}
CString CRight::ToString(CTime value)
{
	CString s;
	s += ToDayString(value.GetDayOfWeek());
	s += L" ";
	s += ToString(value.GetDay());
	s += L".";
	s += ToString(value.GetMonth());
	s += L".";
	s += ToString(value.GetYear());
	return s;
}
bool CRight::GetIntFromEditBox(CEdit *pEdit, const wchar_t *zName, int &value)
{
	value = 0;
	CString sText;
	pEdit->GetWindowText(sText);
	if (sText.IsEmpty())
		return false;

	return TryConvertInt(sText, zName, value);
}
void CRight::WriteLineToHtmlTable(CHtmlWriter &html)
{
	html.WriteL(L"<tr>");
	html.WriteItemToHtmlTable(msName, msNameHebrew);

	CString sDesc = GetDecriptionForLetter();
	CString sDescHebrew = GetDecriptionForLetterHebrew();
	html.WriteItemToHtmlTable(sDesc, sDescHebrew);

	html.WriteNumericItemToHtmlTable(mDuePay);

	html.WriteItemToHtmlTable(msNameHebrew, msName, true);
	html.WriteL(L"</tr>");
}
bool CRight::IsNumber(CString& s)
{
	int len = s.GetLength();
	if (len < 1)
		return false;
	int nDot = 0;
	int nDig = 0;
	for (int i = 0; i < len; i++)
	{
		wchar_t ch = s[i];
		if (isdigit(ch))
			nDig++;
		else if (ch == '.')
		{
			nDot++;
			if (nDot > 1)
				return false;
		}
		else
			return false;
	}
	return true;
}
bool CRight::GetInputNumber(CEdit* pEdit, CString& os, double& oValue)
{
	pEdit->GetWindowTextW(os);
	if (!IsNumber(os))
	{
		pEdit->SetWindowTextW(L"");
		return false;
	}
	oValue = _wtof(os);
	return true;
}