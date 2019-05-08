#include "StdAfx.h"
#include "LogoWriter.h"
#include "Utils.h"
#include "Right.h"


CLogoWriter::CLogoWriter(const wchar_t *zfName)
	: mbOpen (false)
	, mpf(NULL)
{
	if (!ReadLinePattern())
		return;

	mpf = CRight::OpenFile(zfName,L"xml");
	if (mpf)
	{
		CopyFile(L"logo_paper.prologue");
		mbOpen = true;
	}
}
CLogoWriter::~CLogoWriter(void)
{
	Close();
}
void CLogoWriter::Close(void)
{
	if (mbOpen && mpf)
	{
		CopyFile(L"logo_paper.epilogue");
		fclose(mpf);
		mpf = NULL;
		mbOpen = false;
	}
}
void CLogoWriter::WriteLine(const wchar_t *zText)
{
	if (!mpf)
		return;

	CString s = msLinePattern;
	s.Replace(L"__text__", zText);
	fwprintf(mpf, L"%s\n", (const wchar_t *)s);
}
bool CLogoWriter::ReadLinePattern(void)
{
	FILE * pfRead = CUtils::OpenLetterInput(L"logo_line",L"txt");
	if (!pfRead)
		return false;

	msLinePattern = CUtils::ReadLine(pfRead);
	int count = 0;
	while (count++ < 100)
	{
		CString s = CUtils::TryReadLine(pfRead);
		if (s.IsEmpty())
			break;
		msLinePattern += s;
	}
	fclose(pfRead);
	return true;
}
bool CLogoWriter::CopyFile(const wchar_t *zfName)
{
	FILE *pfRead = CUtils::OpenLetterInput(zfName,L"txt");
	if (!pfRead)
		return false;

	wchar_t ch = 0;
	fwscanf_s(pfRead, L"%c", &ch, sizeof(ch));
	int count = 0;
	while (ch != -1 && count++ < 1000000)
	{
		fwprintf(mpf, L"%c", ch);
		ch = (wchar_t)-1;
		int nRead = fwscanf_s(pfRead, L"%c", &ch, sizeof(ch));
		if (nRead < 1)
			break;
	}
	fclose(pfRead);
	return true;
}
