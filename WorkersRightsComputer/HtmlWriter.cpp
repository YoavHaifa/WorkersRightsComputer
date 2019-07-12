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
{
}
CHtmlWriter::~CHtmlWriter()
{
	if (mpfRead)
		fclose(mpfRead);
	if (mpfWrite)
		fclose(mpfWrite);
}
int CHtmlWriter::WriteLetterFromTemplate()
{
	mpfRead = MyFOpenWithErrorBox(L"..\\HTML\\letterTemplate.html", L"r, ccs=UNICODE", L"letter template");

	msfName = CRight::GetSaveFileName(L"logo_letter_temp", L"html");
	mpfWrite = MyFOpenWithErrorBox(msfName, L"w, ccs=UNICODE", L"HTML Unicode");
	//mpfWrite = MyFOpenWithErrorBox(msfName, L"wb", L"HTML Unicode");

	CString s;
	int n = 0;
	//wchar_t prevChar = 0;
	wchar_t ch = (wchar_t)getwc(mpfRead);
	//while (fread(&ch, sizeof(ch), 1, mpfRead))
	while (ch != 0 && ch != WEOF)
		{
		n++;
		s += ch;
		if (ch == '{')
		{
			ReplaceTemplateVariable();
		}
		//fwrite(&ch,sizeof(ch), 1, mpfWrite);
		//prevChar = ch;
		else
			fwprintf(mpfWrite, L"%c", ch);
		ch = (wchar_t)getwc(mpfRead);
	}
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

	if (sToken == L"headerAddress")
		WritePara(L"iris.bar@kavlaoved.org.il - טל. 04-8643350 פקס 04-8644238 דואל");
	else if (sToken == L"startLetter")
		gWorker.StartLetter(*this);
	else if (sToken == L"workPeriod")
		Print(gWorkPeriod.GetPeriodForLetter());
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
		fwprintf(mpfWrite, L"{{Unexpeted Token: %s}}", (const wchar_t *)sToken);
}
void CHtmlWriter::Print(const CString &s)
{
	fwprintf(mpfWrite, L"%s", (const wchar_t *)s);
}
int CHtmlWriter::WriteLetter()
{
	//msfName = L"C:\\WorkersRights\\Try\\HtmlTry.html";
	msfName = CRight::GetSaveFileName(L"logo_letter", L"html");
	mpfWrite = MyFOpenWithErrorBox(msfName, L"w, ccs=UNICODE", L"HTML Unicode");
	//unsigned char1 = 0xff;
	//unsigned char2 = 0xfe;
	//fwprintf(&char1, 1, 1, mpfWrite);
	//fwprintf(&char2, 1, 1, mpfWrite);

	WriteL(L"<!DOCTYPE html>");
	WriteL(L"<html>");
	WriteL(L"<head>");
	WriteL(L"<title>Page Title </title>");
	WriteL(L"<style>");
	WriteL(L"table, th, td{");
	WriteL(L"border: 1px solid black;");
	WriteL(L"}");
	WriteL(L"</style>");
	WriteL(L"</head>");
	WriteL(L"<body>");

	WriteHeadline(L"*** Worker's Hotline - عنوان للعامل - קו לעובד ***");
	WriteHeadline(L"iris.bar@kavlaoved.org.il - טל. 04-8643350 פקס 04-8644238 דוא\"ל", 2);
	WriteLine(L"");
	WriteHeadline(L"Computation of Due Payments חישוב זכאות");
	WriteLine(L"");

	gWorker.StartLetter(*this);
	WriteLine(L"");

	WriteTable(false);

	WriteL(L"</body>");
	WriteL(L"</html>");
	return 0;
}
void CHtmlWriter::BREOL()
{
	wchar_t zEOL[] = L"<br>";
	//size_t nWritten = fwprintf(zEOL, sizeof(zEOL[0]), wcslen(zEOL) + 1, mpfWrite);
	fwprintf(mpfWrite, L"%s\n", zEOL);
}
void CHtmlWriter::EOL()
{
	fwprintf(mpfWrite, L"\n");
}
void CHtmlWriter::Write(const wchar_t *zText)
{
	fwprintf(mpfWrite, L"%s", zText);
}
void CHtmlWriter::WriteL(const wchar_t *zText)
{
	fwprintf(mpfWrite, L"%s\n", zText);
	EOL();
}
void CHtmlWriter::WritePara(const wchar_t *zText)
{
	fwprintf(mpfWrite, L"<p> %s </p>\n", zText);
}
void CHtmlWriter::WriteLine(const wchar_t *zText)
{
	fwprintf(mpfWrite, L"%s<br>\n", zText);
	fflush(mpfWrite);
}
void CHtmlWriter::WriteHeadline(const wchar_t *zText, int iH)
{
	fwprintf(mpfWrite, L"<h%d style=\"text-align:center;\">", iH);
	fwprintf(mpfWrite, L"%s</h%d>\n", zText, iH);
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
