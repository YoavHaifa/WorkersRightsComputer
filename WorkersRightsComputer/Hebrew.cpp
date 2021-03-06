#include "StdAfx.h"
#include "Hebrew.h"
#include "Utils.h"

CHebrew gHebrew;

CHebrew::CHebrew(void)
	: mbValid(false)
	, mn(0)
	, msNone (_T("Missing-Hebrew-Word"))
{
	if (InitFromFile())
		WriteToFile();
}
bool CHebrew::InitFromFile(void)
{
	FILE *pfRead = CUtils::OpenInputFile(L"Hebrew",L"txt");
	if (!pfRead)
		return false;

	CString s = CUtils::TryReadLine(pfRead);
	CString sh = CUtils::TryReadLine(pfRead);
	while (!s.IsEmpty() && !sh.IsEmpty())
	{
		CPair *p = new CPair(s, sh);
		if (mn == 0)
		{
			mpFirst = p;
			mpLast = p;
		}
		else
		{
			mpLast->mpNext = p;
			mpLast = p;
		}
		mn++;
		s = CUtils::TryReadLine(pfRead);
		sh = CUtils::TryReadLine(pfRead);
	}
	fclose(pfRead);
	return true;
}
void CHebrew::WriteToFile(void)
{
	FILE *pfWrite = CUtils::OpenLogFile(L"Hebrew");
	CPair *p = mpFirst;
	while (p)
	{
		fwprintf(pfWrite, L"%s - %s\n", (const wchar_t *)p->ms, (const wchar_t *)p->msh);
		p = p->mpNext;
	}
	fclose(pfWrite);
}
CString CHebrew::Get(const wchar_t *z)
{
	CPair *p = mpFirst;
	while (p)
	{
		if (p->ms == z)
			return p->msh;
		p = p->mpNext;
	}
	return msNone;
}



void CHebrew::TestRead()
{
	CString sfName(L"..\\release\\Input\\Hebrew.txt");
	FILE *pfRead = MyFOpenWithErrorBox(sfName, L"rb", L"Hebrew Dictionary");
	if (!pfRead)
		return;

	static const int N_CHAR = 100;
	unsigned char zBuf[N_CHAR];
	//size_t nRead = fread(zBuf, 1, N_CHAR, pfRead);
	fread(zBuf, 1, N_CHAR, pfRead);
	fclose(pfRead);

	FILE *pfLog = CUtils::OpenLogFile(L"CHebrew__TestRead");
	if (!pfLog)
		return;
	for (int i = 0; i < N_CHAR; i++)
	{
		fprintf(pfLog, "%3i: %02x - %c\n", i, zBuf[i], zBuf[i]);
	}
	fclose(pfLog);
}
void CHebrew::TestWrite()
{
	CString sDirName(L"C:\\WorkersRights\\Try\\עברית-גם");
	//int rc = _wmkdir(sDirName);
	_wmkdir(sDirName);
	return;

	/*

	CString sfName(L"..\\release\\Log\\Hebrew__unicode.log");
	FILE *pfWrite = MyFOpenWithErrorBox(sfName, L"w, ccs=UNICODE", L"Hebrew Log");
	if (!pfWrite)
		return;

	// fprintf(pfWrite, "English Text\n");
	fwprintf(pfWrite, L"Wide English Text\n");
	//fprintf(pfWrite, "טקסט בעברית\n");
	fwprintf(pfWrite, L"טקסט רחב בעברית\n");

	fclose(pfWrite);*/
}
