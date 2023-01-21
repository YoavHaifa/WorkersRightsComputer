#include "stdafx.h"
#include "MyDialogEx.h"
#include "Utils.h"
//#include "MyRegistry.h"
//#include "MyWindows.h"
//#include "PipeWriter.h"
//#include "LogFile.h"

CMyDialogEx::CMyDialogEx(UINT nIDTemplate, CWnd* pParentWnd)
: CDialogEx(nIDTemplate, pParentWnd)
, mbWriteToPipe(false)
, mnWriteToPipeFailed(0)
, mnWriteToPipeSucceeded(0)
, msPipeName(_T(""))
{
}
CMyDialogEx::~CMyDialogEx(void)
{
}
void CMyDialogEx::SetParameter(int id, int value)
{
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
    {
        wchar_t zBuf[64];
        swprintf_s (zBuf, 64, L"%3d", value);
        pWnd->SetWindowText(zBuf);
    }
}
void CMyDialogEx::SetParameter(int id, float value)
{
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
    {
        wchar_t zBuf[64];
        swprintf_s (zBuf, 64, L"%.3f", value);
        pWnd->SetWindowText(zBuf);
    }
}
void CMyDialogEx::SetParameter(int id, double value)
{
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
    {
        wchar_t zBuf[64];
        swprintf_s (zBuf, 64, L"%.3f", value);
        pWnd->SetWindowText(zBuf);
    }
}
void CMyDialogEx::SetParameter(int id, bool bFlag)
{
	CButton *pButton=(CButton *)GetDlgItem(id);
	if (pButton)
		pButton->SetCheck(bFlag);
}
bool CMyDialogEx::GetParameter(int id, int &oValue, int minValue, int maxValue)
{
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
    {
        CString s;
        pWnd->GetWindowText(s);
		int n = _wtoi(s);
		if ( n >= minValue && n <= maxValue)
		{
			oValue = n;
			return true;
		}
    }
	return false;
}
bool CMyDialogEx::GetParameter(int id, float &oValue, float minValue, float maxValue)
{
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
    {
        CString s;
        pWnd->GetWindowText(s);
		float n = (float)_wtof(s);
		if ( n >= minValue && n <= maxValue)
		{
			oValue = n;
			return true;
		}
    }
	return false;
}
bool CMyDialogEx::GetParameter(int id, double &oValue, double minValue, double maxValue)
{
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
    {
        CString s;
        pWnd->GetWindowText(s);
		double n = _wtof(s);
		if ( n >= minValue && n <= maxValue)
		{
			oValue = n;
			return true;
		}
    }
	return false;
}
bool CMyDialogEx::GetParameter(int id, int &oValue)
{
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
    {
        CString s;
        pWnd->GetWindowText(s);
		s.TrimLeft(' ');
		CString sPreix(s.Left(2));
		if (sPreix == L"0x" || sPreix == L"0X")
			oValue = wcstol(s,NULL,16);
		else
			oValue = _wtoi(s);
		return true;
    }
	return false;
}
void CMyDialogEx::GetParameter(int id, bool &bFlag)
{
	CButton *pButton=(CButton *)GetDlgItem(id);
	if (pButton)
	{
	    int ChkState=pButton->GetCheck();
		bFlag = ChkState == BST_CHECKED;
	}
}
bool CMyDialogEx::IsChecked(int id)
{
    CButton *pButton = (CButton *)GetDlgItem(id);
	bool bChecked = (pButton->GetCheck() != 0);
    return bChecked;
}
void CMyDialogEx::SetCheck(int id, bool bCheck)
{
    CButton *pWnd=(CButton *)GetDlgItem(id);
	if (pWnd)
	{
		if (bCheck)
			pWnd->SetCheck(BST_CHECKED);
		else
			pWnd->SetCheck(BST_UNCHECKED);
	}
}
void CMyDialogEx::SetMenuCheck(int id, bool bCheck)
{
    CMenu *pMenu=GetMenu();
	if (pMenu)
	{
		if (bCheck)
			pMenu->CheckMenuItem(id, MF_BYCOMMAND | MF_CHECKED);
		else
			pMenu->CheckMenuItem(id, MF_BYCOMMAND | MF_UNCHECKED);
	}
}
void CMyDialogEx::SetTitle(const wchar_t * zText)
{
	SetWindowText(zText);
}
bool CMyDialogEx::SetText(int id, const wchar_t* zText)
{
	CWnd* pWnd = GetDlgItem(id);
	if (!pWnd)
		return false;

	pWnd->SetWindowText(zText);
	return true;
}
bool CMyDialogEx::Clear(int id)
{
	CWnd* pWnd = GetDlgItem(id);
	if (!pWnd)
		return false;

	pWnd->SetWindowText(L"");
	return true;
}
bool CMyDialogEx::SetText(int id, const wchar_t * zText1, const wchar_t * zText2)
{
	CString sText(zText1);
	sText += " ";
	sText += zText2;
	return SetText(id, sText);
}
bool CMyDialogEx::GetItemText(int id, CString &sText)
{
    CWnd *pWnd = GetDlgItem(id);
    if (!pWnd)
		return false;
        
	pWnd->GetWindowText(sText);
	static bool ubOnce = false;
	static bool ubAllowUserToInitiateCrashByText = false;
	/* if (!ubOnce)
	{
		ubOnce = true;
		CMyRegistry::GetAppReg("enable_crash_by_text", ubAllowUserToInitiateCrashByText);
	} */
	/*
	if (ubAllowUserToInitiateCrashByText)
	{
		if (sText.Left(7) == L"crashme")
			OnUserCrash(sText);
	} */
	return true;
}
CString CMyDialogEx::GetTextOptional(int id)
{
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
	{
		CString s;
		pWnd->GetWindowText(s);
		return s;
	}
	return CString(_T(""));
}
CString CMyDialogEx::GetText(int id)
{
	CString s("Dialog Text Not Defined");
    CWnd *pWnd = GetDlgItem(id);
    if (pWnd)
	{      
		pWnd->GetWindowText(s);
	}
	return s;
}
bool CMyDialogEx::Enable(int id, bool bEnable)
{
    CWnd *pWnd = GetDlgItem(id);
    if (!pWnd)
		return false;
		
	pWnd->EnableWindow(bEnable);
	return true;
}
bool CMyDialogEx::Disable(int id)
{
    CWnd *pWnd = GetDlgItem(id);
    if (!pWnd)
		return false;
		
	pWnd->EnableWindow(false);
	return true;
}
bool CMyDialogEx::ClearAndDisable(int id)
{
	CWnd* pWnd = GetDlgItem(id);
	if (!pWnd)
		return false;
	pWnd->SetWindowText(L"");
	pWnd->EnableWindow(false);
	return true;
}
bool CMyDialogEx::SetAndEnable(int id, const wchar_t* zText)
{
	CWnd* pWnd = GetDlgItem(id);
	if (!pWnd)
		return false;
	pWnd->SetWindowText(zText);
	pWnd->EnableWindow(true);
	return true;
}
/*
static int five = 5;
void CMyDialogEx::OnUserCrash(CString &sText)
{
	int four = 4;
	if (sText.Find(L"div") > 0)
	{
		__try
		{
			int zero = five - four - 1;
			int num = 20;
			static int kk = 0;
			kk = num / zero;
		}
		__except(GetExceptionCode() == EXCEPTION_INT_DIVIDE_BY_ZERO ? 
			EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{ 
			CUtils::MessBox(L"Exception caught", L"SW Test");
			FILE *pf = MyFOpen(L"d:\\MyLog\\OnCrashMe.log",L"w");
			if (pf)
			{
				fwprintf (pf, L"Crash in GLinesSet\n");
				fclose(pf);
			}
			throw;
		}
	}

	int *pi = 0;
	*pi = 0;
} */
bool CMyDialogEx::VerifyAndGetInput(int id, int minValue, int maxValue, int & oValue, const wchar_t * zName)
{
	CString sText(_T(""));
	GetItemText(id, sText);
	if (sText.IsEmpty())
	{
		CString sErr(zName);
		sErr += L" is not defined";
		CUtils::MessBox(sErr, L"Missing Input");
		return false;
	}
	int inputValue = 0;
	if (!GetParameter(id, inputValue, minValue, maxValue))
	{
		wchar_t zBuf[256];
		swprintf_s(zBuf,sizeof(zBuf)/sizeof(zBuf[0]),L"%s should be in the range %d - %d", zName, minValue, maxValue);
		CUtils::MessBox(zBuf, L"Unexpected Input");
		return false;
	}
	oValue = inputValue;
	return true;
}
bool CMyDialogEx::VerifyAndGetInput(int id, float minValue, float maxValue, float & oValue, const wchar_t * zName)
{
	CString sText("");
	GetItemText(id, sText);
	if (sText.IsEmpty())
	{
		CString sErr(zName);
		sErr += L" is not defined";
		CUtils::MessBox(sErr, L"Missing Input");
		return false;
	}
	float inputValue = 0;
	if (!GetParameter(id, inputValue, minValue, maxValue))
	{
		wchar_t zBuf[256];
		swprintf_s(zBuf,sizeof(zBuf)/sizeof(zBuf[0]), L"%s should be in the range %.2f - %.2f", zName, minValue, maxValue);
		CUtils::MessBox(zBuf, L"Unexpected Input");
		return false;
	}
	oValue = inputValue;
	return true;
}
/*
void CMyDialogEx::SetStatusWindow(int id)
{
	CWnd *pWnd = GetDlgItem(id);
	if (pWnd)
		CMyWindows::SetStatusWindow(pWnd);
}*/
/*
void CMyDialogEx::SetWriteToPipe(const wchar_t *zPipeName)
{
	CLogFile log("Pipes", "CMyDialogEx__SetWriteToPipe");
	msPipeName = zPipeName;
	if (!msPipeName.IsEmpty())
		mbWriteToPipe = true;
	if (log)
		fprintf(log.mpf,"<SetWriteToPipe> %s\n", msPipeName);
}*/
/*
void CMyDialogEx::WriteToPipe(const wchar_t *zText)
{
	if (!CPipeWriter::WriteOnce(msPipeName, zText))
	{
		mnWriteToPipeFailed++;
		if ((mnWriteToPipeFailed > 1) && (mnWriteToPipeSucceeded == 0))
			mbWriteToPipe = false;
	}
	else
	{
		mnWriteToPipeSucceeded++;
	}
}*/
