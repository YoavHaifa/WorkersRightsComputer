#include "StdAfx.h"
#include "XmlDump.h"
#include "FileName.h"
#include "Utils.h"
#include "MyTime.h"

int CXMLDump::umSetId = 0;
bool CXMLDump::umbDebug = false;
bool CXMLDump::umbReportError = true;
CString CXMLDump::umsDefaultPath(_T(""));

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

CXMLDumpScope::CXMLDumpScope(const wchar_t *zName, CXMLDump &XMLDump)
: msName(zName)
, mpXMLDump(&XMLDump)
{
	if (mpXMLDump)
	    mpXMLDump->Start(msName);
}
CXMLDumpScope::CXMLDumpScope(const wchar_t *zName, CXMLDump *pXMLDump)
: msName(zName)
, mpXMLDump(pXMLDump)
{
	if (mpXMLDump)
		mpXMLDump->Start(msName);
}

CXMLDumpScope::~CXMLDumpScope(void)
{
	if (mpXMLDump)
	    mpXMLDump->End(msName);
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

CXMLDump::CXMLDump(const wchar_t *zDir, const wchar_t *zName, const wchar_t *zRoot)
: msName(zName)
, msRoot(zRoot)
, mpf(NULL)
, mnIdent(0)
, mId(++umSetId)
{
    CFileName fName(msName);
	if (CUtils::IsDirectory(zDir))
		fName.ChangePath(zDir);
	//else
	//{
	//	CString sDir = CMyWindows::GetLogDir(zDir);
	//	fName.ChangePath(sDir);
	//}
	msName = fName;
	OnBuild(fName);
}
CXMLDump::CXMLDump(const wchar_t *zName, const wchar_t *zRoot)
: msName(zName)
, msRoot(zRoot)
, mpf(NULL)
, mnIdent(0)
, mId(++umSetId)
{
    CFileName fName(msName);
	OnBuild(fName);
}

CXMLDump::~CXMLDump(void)
{
	Close();
}
void CXMLDump::SetDeafultPath(const wchar_t *zPath)
{
	if (zPath)
		umsDefaultPath = zPath;
	//else
	//	umsDefaultPath = CMyWindows::GetLogDir();
}
void CXMLDump::OnBuild(CFileName &fName)
{
    if (!fName.IsOfType(L"xml"))
    {
		if (!msRoot.IsEmpty() && !fName.IsOfType(msRoot))
        {
            msName += ".";
            msName += msRoot;
        }
        msName += ".xml";
    }
	if (!CUtils::VerifyDirectory(fName.Path()))
		return;

	if (umbReportError)
	    mpf = MyFOpenWithErrorBox (msName, L"w, ccs=UNICODE", L"Xml Dump");
	else
	    mpf = MyFOpen (msName, L"w, ccs=UNICODE");

	if (mpf && !msRoot.IsEmpty())
    {
        fwprintf (mpf, L"<%s>\n", (const wchar_t *)msRoot);
        mnIdent++;
    }
}
void CXMLDump::Close(void)
{
    if (mpf)
    {
		if (!msRoot.IsEmpty())
	        fwprintf (mpf, L"</%s>\n", (const wchar_t *)msRoot);
        fclose (mpf);
		mpf = NULL;
    }
}
void CXMLDump::Start(const wchar_t * zName)
{
    if (!mpf)
        return;
    Ident();
    fwprintf (mpf, L"<%s>\n", zName);
    mnIdent++;
}

void CXMLDump::Ident(void)
{
    for (int i = 0; i < mnIdent; i++)
        fwprintf (mpf, L"  ");
}

void CXMLDump::End(const wchar_t * zName)
{
    if (!mpf)
        return;
    if (mnIdent > 0)
        mnIdent--;
    Ident();
    fwprintf (mpf, L"</%s>\n", zName);

	if (umbDebug)
		fflush(mpf);
}

void CXMLDump::Write(const wchar_t * zName, bool bValue)
{
    if (!mpf)
        return;
    Ident();
    fwprintf (mpf, L"<%s> %s </%s>\n", zName, bValue ? L"true" : L"false", zName);
	if (umbDebug)
		fflush(mpf);
}
void CXMLDump::Write(const wchar_t * zName, int value)
{
    if (!mpf)
        return;
    Ident();
    fwprintf (mpf, L"<%s> %d </%s>\n", zName, value, zName);
	if (umbDebug)
		fflush(mpf);
}
void CXMLDump::Write(const wchar_t * zName, ULONGLONG value)
{
    if (!mpf)
        return;
    Ident();
    fwprintf (mpf, L"<%s> %I64d </%s>\n", zName, value, zName);
	if (umbDebug)
		fflush(mpf);
}
void CXMLDump::Write(const wchar_t * zName, unsigned int value)
{
    if (!mpf)
        return;
    Ident();
    fwprintf (mpf, L"<%s> %u </%s>\n", zName, value, zName);
	if (umbDebug)
		fflush(mpf);
}
void CXMLDump::Write(const wchar_t * zName, float value)
{
	if (!mpf)
		return;
	Ident();
	fwprintf(mpf, L"<%s> %f </%s>\n", zName, value, zName);
	if (umbDebug)
		fflush(mpf);
}
void CXMLDump::Write(const wchar_t * zName, double value)
{
	if (!mpf)
		return;
	Ident();
	fwprintf(mpf, L"<%s> %f </%s>\n", zName, value, zName);
	if (umbDebug)
		fflush(mpf);
}
void CXMLDump::Write(const wchar_t * zName, const wchar_t * zValue)
{
    if (!mpf)
        return;
    Ident();
    fwprintf (mpf, L"<%s> %s </%s>\n", zName, zValue, zName);
	if (umbDebug)
		fflush(mpf);
}
void CXMLDump::SaveColor(COLORREF color)
{
    if (!mpf)
        return;
    Ident();
    fwprintf (mpf, L"<color> 0x%06x </color>\n", color);
}
void CXMLDump::Write(const wchar_t * zName, class CMyTime &time)
{
    if (!mpf)
        return;
    Ident();
    fwprintf (mpf, L"<%s> ", zName);
	time.Write(mpf);
    fwprintf (mpf, L" </%s>\n", zName);
}
