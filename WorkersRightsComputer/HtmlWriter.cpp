#include "stdafx.h"
#include "HtmlWriter.h"
#include "Utils.h"
#include "Person.h"
#include "MyTime.h"
#include "AllRights.h"
#include "WorkPeriod.h"
#include "FamilyPart.h"
#include "UsedVacations.h"
#include "WorkersRightsComputerDlg.h"
#include "Pension.h"


CHtmlWriter::CHtmlWriter()
	: mpfWrite(NULL)
	, mpfRead(NULL)
	, mpfHebrewWrite(NULL)
	, mbEng(true)
	, mbHeb(true)
{
}
CHtmlWriter::~CHtmlWriter()
{
	if (mpfRead)
		fclose(mpfRead);
	if (mpfWrite)
		fclose(mpfWrite);
	if (mpfHebrewWrite)
		fclose(mpfHebrewWrite);
}
bool CHtmlWriter::CopyLogo(const wchar_t* zfName)
{
	CFileName fName(zfName);
	CString sPath = fName.Path();
	CString sLogoSrc(L"..\\HTML\\Logo.jpg");
	CString sLogoTarget = sPath + L"Logo.jpg";
	CopyFile(sLogoSrc, sLogoTarget, FALSE);
	return true;
}
int CHtmlWriter::WriteLetterFromTemplate(const wchar_t* zfName)
{
	CopyLogo(zfName);

	mpfRead = MyFOpenWithErrorBox(L"..\\HTML\\letterTemplate.html", L"r, ccs=UNICODE", L"letter template");

	msfName = zfName;
	mpfWrite = MyFOpenWithErrorBox(msfName, L"w, ccs=UNICODE", L"HTML Unicode");

	OpenHebrewLetter();

	CString s;
	int n = 0;
	wchar_t ch = (wchar_t)getwc(mpfRead);
	while (ch != 0 && ch != WEOF)
		{
		n++;
		s += ch;
		if (ch == '{')
		{
			ReplaceTemplateVariable();
		}
		else
		{
			if (mbEng)
				fwprintf(mpfWrite, L"%c", ch);
			if (mbHeb)
				fwprintf(mpfHebrewWrite, L"%c", ch);
		}
		ch = (wchar_t)getwc(mpfRead);
	}
	CUtils::MessBox(msfName, L"Letter Saved");
	return n;
}
void CHtmlWriter::ReplaceTemplateVariable(void)
{
	wchar_t ch = (wchar_t)getwc(mpfRead);
	if (ch == '{')
		ch = (wchar_t)getwc(mpfRead);
	CString sToken(ch);
	ch = (wchar_t)getwc(mpfRead);
	while (ch != 0 && ch != WEOF && ch != '}')
	{
		sToken += ch;
		ch = (wchar_t)getwc(mpfRead);
	}

	if (ch == '}')
		ch = (wchar_t)getwc(mpfRead); // Get rid of second '}'

	if (sToken == L"ENG")
	{
		mbEng = true;
		mbHeb = false;
	}
	else if (sToken == L"HEB")
	{
		mbEng = false;
		mbHeb = true;
	}
	else if (sToken == L"BOTH")
	{
		mbEng = true;
		mbHeb = true;
	}
	else if (sToken == L"headerAddress")
		WriteParaLTR(L"iris.bar@kavlaoved.org.il - טל. 04-8643350 פקס 04-8644238 דואל");
	else if (sToken == L"startLetter")
		gWorker.StartLetter(*this);
	else if (sToken == L"workPeriod")
		PrintEH(gWorkPeriod.GetPeriodForLetter(), gWorkPeriod.GetPeriodForLetterHebrew());
	else if (sToken == L"table")
		WriteTable(true);
	else if (sToken == L"pension")
		gpPension->WriteToLetter(*this);
	else if (sToken == L"vacations")
		gUsedVacations.WriteToLetter(*this);
	else if (sToken == L"familyPart")
		gFamilyPart.WriteToLetter(*this);
	else if (sToken == L"lastSalary")
		gWorkPeriod.WriteLastSalary(*this);
	else if (sToken == L"editor")
		gpDlg->WriteEditorToLetter(*this);
	else if (sToken == L"date")
	{
		CMyTime now;
		now.SetNow();
		WriteL(now.ToString());
	}

	else
	{
		if (mbEng)
			fwprintf(mpfWrite, L"{{Unexpeted Token: %s}}", (const wchar_t*)sToken);
		if (mbHeb)
			fwprintf(mpfHebrewWrite, L"{{Unexpeted Token: %s}}", (const wchar_t*)sToken);
	}
}
void CHtmlWriter::Print(const CString &s)
{
	if (mbEng)
		fwprintf(mpfWrite, L"%s", (const wchar_t*)s);
	if (mbHeb)
		fwprintf(mpfHebrewWrite, L"%s", (const wchar_t*)s);
}
void CHtmlWriter::PrintEH(const CString& s, const CString& sh)
{
	if (mbEng)
		fwprintf(mpfWrite, L"%s", (const wchar_t*)s);
	if (mbHeb)
		fwprintf(mpfHebrewWrite, L"%s", (const wchar_t*)sh);
}
void CHtmlWriter::Write(const wchar_t *zText)
{
	if (mbEng)
		fwprintf(mpfWrite, L"%s", zText);
	if (mbHeb)
		fwprintf(mpfHebrewWrite, L"%s", zText);
}
void CHtmlWriter::WriteL(const wchar_t* zText)
{
	if (mbEng)
		fwprintf(mpfWrite, L"%s\n", zText);
	if (mbHeb)
		fwprintf(mpfHebrewWrite, L"%s\n", zText);
}
void CHtmlWriter::WriteLEH(const wchar_t* zText, const wchar_t* zHebrewText)
{
	if (mbEng)
		fwprintf(mpfWrite, L"%s\n", zText);
	if (mbHeb)
		fwprintf(mpfHebrewWrite, L"%s\n", zHebrewText);
}
void CHtmlWriter::WritePara(const wchar_t* zText)
{
	if (mbEng)
		fwprintf(mpfWrite, L"<p> %s </p>\n", zText);
	if (mbHeb)
		fwprintf(mpfHebrewWrite, L"<p> %s </p>\n", zText);
}
void CHtmlWriter::WriteParaLTR(const wchar_t* zText)
{
	if (mbEng)
		fwprintf(mpfWrite, L"<p> %s </p>\n", zText);
	if (mbHeb)
		fwprintf(mpfHebrewWrite, L"<p dir=""ltr""> %s </p>\n", zText);
}
void CHtmlWriter::WriteLine(const wchar_t *zText)
{
	if (mbEng)
	{
		fwprintf(mpfWrite, L"%s<br>\n", zText);
		fflush(mpfWrite);
	}
	if (mbHeb)
	{
		fwprintf(mpfHebrewWrite, L"%s<br>\n", zText);
		fflush(mpfHebrewWrite);
	}
}
void CHtmlWriter::WriteLineEH(const wchar_t* zText, const wchar_t* zHebrewText)
{
	if (mbEng)
	{
		fwprintf(mpfWrite, L"%s<br>\n", zText);
		fflush(mpfWrite);
	}
	if (mbHeb)
	{
		fwprintf(mpfHebrewWrite, L"%s<br>\n", zHebrewText);
		fflush(mpfHebrewWrite);
	}
}
void CHtmlWriter::WriteLineEH(const wchar_t* zText, const wchar_t* zHebrewText, const wchar_t* zExtraText)
{
	if (mbEng)
	{
		fwprintf(mpfWrite, L"%s %s<br>\n", zText, zExtraText);
		fflush(mpfWrite);
	}
	if (mbHeb)
	{
		fwprintf(mpfHebrewWrite, L"%s %s<br>\n", zHebrewText, zExtraText);
		fflush(mpfHebrewWrite);
	}
}
void CHtmlWriter::WriteHeadline(const wchar_t *zText, int iH)
{
	if (mbEng)
	{
		fwprintf(mpfWrite, L"<h%d style=\"text-align:center;\">", iH);
		fwprintf(mpfWrite, L"%s</h%d>\n", zText, iH);
	}
	if (mbHeb)
	{
		fwprintf(mpfHebrewWrite, L"<h%d style=\"text-align:center;\">", iH);
		fwprintf(mpfHebrewWrite, L"%s</h%d>\n", zText, iH);
	}
}
void CHtmlWriter::WriteTable(bool bUsingTemplate)
{
	WriteL(L"<table style=\"width:50%\" border=1>");

	WriteL(L"<tr>");
	WriteL(L"<th>Topic</th>");
	WriteL(L"<th>Computation</th>");
	WriteL(L"<th>Due</th>");
	WriteL(L"<th>בעניין</th>");
	WriteL(L"</tr>");

	gAllRights.WriteLetterToHtml(*this, bUsingTemplate);

	WriteL(L"</table>");
}
void CHtmlWriter::StartParagraph(void)
{
	WriteL(L"<p>");
}
void CHtmlWriter::EndParagraph(void)
{
	WriteL(L"</p>");
}
void CHtmlWriter::StartTabLine(void)
{
	WriteL(L"<tr>");
}
void CHtmlWriter::EndTabLine(void)
{
	WriteL(L"</tr>");
}
void CHtmlWriter::StartPensionTable(void)
{
	WriteL(L"<table style=\"width:50%\" border=1>");

	WriteL(L"<tr>");
	WriteL(L"<th>From</th>");
	WriteL(L"<th>Till</th>");
	WriteL(L"<th>Salary</th>");
	WriteL(L"<th>Months</th>");
	WriteL(L"<th>Total</th>");
	WriteL(L"<th>Pension</th>");
	WriteL(L"<th>Due</th>");
	if (gpPension->mbSeverance)
	{
		WriteL(L"<th>Severance</th>");
		WriteL(L"<th>Due</th>");
		WriteL(L"<th>All</th>");
	}
	WriteL(L"</tr>");
}
void CHtmlWriter::EndPensionTable(void)
{
	WriteL(L"</table>");
}
void CHtmlWriter::Write2Tab(CString &sItem)
{
	CString s("<td>");
	s += sItem;
	s += "</td>";
	WriteL(s);
}
void CHtmlWriter::Write2Tab(const char *zText)
{
	CString s("<td>");
	s += zText;
	s += "</td>";
	WriteL(s);
}
void CHtmlWriter::Write2Tab(double value)
{
	char zBuf[128];
	sprintf_s(zBuf, 128, "%.2f", value);
	Write2Tab(zBuf);
}
void CHtmlWriter::Write2Tab(const char *zFormat, double value)
{
	char zBuf[128];
	sprintf_s(zBuf, 128, zFormat, value);
	Write2Tab(zBuf);
}


bool CHtmlWriter::OpenHebrewLetter()
{
	if (msfName.Find(L"english") < 1)
		return false;

	msfHebrewName = msfName;
	msfHebrewName.Replace(L"english", L"hebrew");
	mpfHebrewWrite = MyFOpenWithErrorBox(msfHebrewName, L"w, ccs=UNICODE", L"HTML Unicode");
	return true;
}
