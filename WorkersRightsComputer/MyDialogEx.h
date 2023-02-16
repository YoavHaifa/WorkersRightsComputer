#pragma once
#include "afxwin.h"
#include <afxdialogex.h>

class CMyDialogEx :
	public CDialogEx
{
public:
	CMyDialogEx(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CMyDialogEx(void);

	void SetParameter(int id, bool bFlag);
	void SetParameter(int id, int value);
	void SetParameter(int id, float value);
	void SetParameter(int id, double value);
	void SetParameter(int id, CString& sText);

	bool GetParameter(int id, int &oValue);
	bool GetParameter(int id, int &oValue, int minValue, int maxValue);
	bool GetParameter(int id, float &oValue, float minValue, float maxValue);
	bool GetParameter(int id, double &oValue, double minValue, double maxValue);
	void GetParameter(int id, bool& bFlag);
	bool GetParameter(int id, CString& sText);

	bool IsChecked(int id);
	void SetCheck(int id, bool bCheck);
	void SetMenuCheck(int id, bool bCheck);

	void SetTitle(const wchar_t * zText);
	bool Clear(int id);
	bool SetText(int id, const wchar_t* zText);
	bool SetText(int id, const wchar_t * zText1, const wchar_t * zText2);
	bool GetItemText(int id, CString &sText);
	CString GetText(int id);
	CString GetTextOptional(int id);
	void SetStatusWindow(int id);

	bool Enable(int id, bool bEnable = true);
	bool Disable(int id);
	bool ClearAndDisable(int id);
	bool SetAndEnable(int id, const wchar_t* zText);
	bool SetInvisible(int id);

private:
	//void OnUserCrash(CString &sText);
	bool mbWriteToPipe;
	int mnWriteToPipeFailed;
	int mnWriteToPipeSucceeded;
	CString msPipeName;
	//void WriteToPipe(const wchar_t *zText);

public:
	bool VerifyAndGetInput(int id, int minValue, int maxValue, int & oValue, const wchar_t * zName);
	bool VerifyAndGetInput(int id, float minValue, float maxValue, float & oValue, const wchar_t * zName);
	//void SetWriteToPipe(const wchar_t *zPipeName);
};
