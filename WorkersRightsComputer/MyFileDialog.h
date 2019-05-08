//----------------------------------------------------------------------
// MyFileDialog.h
// --------------
// Easy way to use MFC's File Dialog
//
// Example of Usage:
// -----------------
//	CMyFileDialog dlg(CMyFileDialog::FD_OPEN, 
//		"Browse for Excel application file",
//		"c:\\Program Files");
//	dlg.SetFilter("Executable File","*.exe");
//	if (dlg.DoModal())
//	{
//		sExcelPath = dlg.GetPathName();
//		if (CFileName::Exist(sExcelPath))
//		{
//      }
//   }
//----------------------------------------------------------------------

#pragma once

#include "FileName.h"

class CMyFileDialog : public CFileDialog
{
public:
	typedef enum EType
	{
		FD_OPEN,
		FD_SAVE
	} EType;

	CMyFileDialog(EType type, const wchar_t *zTitle, const wchar_t *zPath = NULL, bool bPreferNewPath = false);
	~CMyFileDialog(void);
	virtual INT_PTR DoModal(); // Returns TRUE if Selection Done

private:
	CString msName;
	CString msInitialPath;
	wchar_t maFilterBuf[1024];
	int mFilterLen;
	CString msDefExt;

public:
	CFileName mSelectedFileName;
	CString msRegName;
	void SetFilter(const wchar_t * zDescription, const wchar_t * zExtension);
	void SetDefaultExtention(const wchar_t * zExtension);
};
