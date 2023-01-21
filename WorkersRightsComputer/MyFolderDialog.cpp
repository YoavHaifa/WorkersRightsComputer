#include "stdafx.h"
#include "MyFolderDialog.h"
#include "MyRegistry.h"
#include "FileName.h"

CMyFolderDialog::CMyFolderDialog(const char* zTitle, const char* zPath, bool bPreferNewPath)
	: msTitle(zTitle)
	, msInitialPath(zPath)
	, mbPreferNewPath(bPreferNewPath)
{
	if (!bPreferNewPath || msInitialPath.IsEmpty())
	{
		CMyRegistry myReg;
		myReg.Get(msTitle, msInitialPath);
	}

	if (CFileName::FindValidPath(msInitialPath))
		m_ofn.lpstrInitialDir = (const wchar_t*)msInitialPath;
	m_ofn.lpstrTitle = msTitle;

}

INT_PTR CMyFolderDialog::DoModal()
{
	INT_PTR rc = CFileDialog::DoModal();
	if (rc != IDOK)
		return 0;

	msFolderName = m_ofn.lpstrFile;

	CString sPath = CFileName::GetPath(msFolderName);
	CMyRegistry myReg(KEY_WRITE);
	myReg.Set(msTitle, sPath);
	return TRUE;
}
