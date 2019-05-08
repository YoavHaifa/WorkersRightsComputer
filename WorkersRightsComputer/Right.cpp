#include "StdAfx.h"
#include "Right.h"
#include "Utils.h"
#include "HtmlWriter.h"

CString CRight::umsSaveDir = L"..\\release\\Log\\";
CString CRight::umsName = L"__";
bool CRight::umbOldStyle = false;

CRight::CRight(const wchar_t *zName, const wchar_t *zHebrewName)
	: msName(zName)
	, msNameHebrew(zHebrewName)
	, mbSkipIfZero(false)
	, mDebug(3)
{
	if (!umsSaveDir)
		ResetSaveDirAndName();

	Init();
}
CRight::~CRight(void)
{
}
void CRight::SetSaveDirAndName(const wchar_t *zSaveDir, const wchar_t *zName)
{
	umsSaveDir = zSaveDir;
	umsName = zName;
}
void CRight::ResetSaveDirAndName(void)
{
	umsSaveDir = L"..\\release\\Log\\";
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
	fwprintf(mpfWrite, L"%s\n\n", (const wchar_t *)sLine);

	bool bOK = Compute();
	fwprintf(mpfWrite, L"\n");
	fwprintf(mpfWrite, L"%s\n", (const wchar_t *)msDue);
	fwprintf(mpfWrite, L"%s\n", (const wchar_t *)msDebug);

	if (!bOK)
	{
		WriteLine(L"");
		sLine = msName;
		sLine += L" not computed!";
		WriteLine (sLine);
	}
	fclose(mpfWrite);
	mpfWrite = NULL;

	return bOK;
}
void CRight::LogLine(const wchar_t *zText)
{
	WriteLine(zText);
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
void CRight::LogLine(const wchar_t *zText, CTimeSpan span)
{
	CString s(zText);
	s += L" TIME SPAN days ";
	s += ToString(span.GetDays());
	s += L" houres ";
	s += ToString(span.GetHours());
	s += L" minutes ";
	s += ToString(span.GetMinutes());
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
CString CRight::GetRightNameForLetter(void)
{
	return msName;
}
CString CRight::GetDecriptionForLetter(void)
{
	return CString(" ");
}
void CRight::WriteLine(const wchar_t *zLine)
{
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
void CRight::WriteItemToHtmlTable(class CHtmlWriter &html, CString &sItem)
{
	CString s("<td>");
	s += sItem;
	s += "</td>";
	html.WriteL(s);
}
void CRight::WriteLineToHtmlTable(class CHtmlWriter &html)
{
	html.WriteL(L"<tr>");
	WriteItemToHtmlTable(html, msName);

	CString sDesc = GetDecriptionForLetter();
	WriteItemToHtmlTable(html, sDesc);

	CString sPay = ToString(mDuePay);
	WriteItemToHtmlTable(html, sPay);

	WriteItemToHtmlTable(html, msNameHebrew);
	html.WriteL(L"</tr>");
}
