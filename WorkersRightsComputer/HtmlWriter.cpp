#include "stdafx.h"
#include "HtmlWriter.h"
#include "Utils.h"
#include "Person.h"
#include "AllRights.h"


CHtmlWriter::CHtmlWriter()
	: mpfWrite(NULL)
{
}
CHtmlWriter::~CHtmlWriter()
{
	if (mpfWrite)
		fclose(mpfWrite);
}
int CHtmlWriter::Try()
{
	msfName = L"C:\\WorkersRights\\Try\\HtmlTry.html";
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

	WriteTable();

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
void CHtmlWriter::WriteLine(const wchar_t *zText)
{
	fwprintf(mpfWrite, L"%s<br>\n", zText);
}
void CHtmlWriter::WriteHeadline(const wchar_t *zText, int iH)
{
	fwprintf(mpfWrite, L"<h%d style=\"text-align:center;\">", iH);
	fwprintf(mpfWrite, L"%s</h%d>\n", zText, iH);
}


void CHtmlWriter::WriteTable()
{
	WriteL(L"<table style=\"width:50%\">");

	WriteL(L"<tr>");
	WriteL(L"<th>Topic</th>");
	WriteL(L"<th>Computation</th>");
	WriteL(L"<th>Due</th>");
	WriteL(L"<th>בעניין</th>");
	WriteL(L"</tr>");

	gAllRights.WriteLetterToHtml(*this);

	WriteL(L"</table>");
}
