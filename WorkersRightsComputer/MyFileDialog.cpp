#include "stdafx.h"
#include "MyFileDialog.h"
#include "MyRegistry.h"
#include "FileName.h"

CMyFileDialog::CMyFileDialog(EType type, const wchar_t *zName, const wchar_t *zPath, bool bPreferNewPath)
: CFileDialog(type == FD_OPEN ? TRUE : FALSE,NULL,NULL,OFN_ENABLESIZING | OFN_EXPLORER)
, msName(zName)
, msInitialPath(zPath)
, msRegName(zName)
, msDefExt(_T(""))
, mFilterLen(0)
{
	if (!bPreferNewPath || msInitialPath.IsEmpty())
	{
		CMyRegistry myReg;
		myReg.Get(msRegName,msInitialPath);
	}
	if (CFileName::FindValidPath(msInitialPath))
		m_ofn.lpstrInitialDir = (const wchar_t *)msInitialPath;
	m_ofn.lpstrTitle = msName;

	memset(maFilterBuf,0,sizeof(maFilterBuf));
}

CMyFileDialog::~CMyFileDialog(void)
{
}
INT_PTR CMyFileDialog::DoModal()
{
	if (mFilterLen > 0)
		m_ofn.lpstrFilter = maFilterBuf;
		//m_ofn.lpstrCustomFilter = "Excel Files\0*.xls\0\0\0";;

	INT_PTR rc = CFileDialog::DoModal();
	if (rc != IDOK)
		return 0;

	mSelectedFileName.Set(m_ofn.lpstrFile);
	if (!msDefExt.IsEmpty())
	{
		if (!mSelectedFileName.HasExtension())
			mSelectedFileName.ChangeType(msDefExt);
	}
	CMyRegistry myReg(KEY_WRITE);
	myReg.Set(msRegName,mSelectedFileName.Path());
	return TRUE;
}
void CMyFileDialog::SetFilter(const wchar_t * zDescription, const wchar_t * zExtension)
{
	wcscpy_s(maFilterBuf,sizeof(maFilterBuf),zDescription);
	mFilterLen = (int)wcslen(zDescription);
	maFilterBuf[mFilterLen++] = 0;

	wcscpy_s(maFilterBuf+mFilterLen,sizeof(maFilterBuf)-mFilterLen,zExtension);
	mFilterLen += (int)wcslen(zExtension);
	maFilterBuf[mFilterLen++] = 0;
	maFilterBuf[mFilterLen++] = 0;
	maFilterBuf[mFilterLen++] = 0;
}
void CMyFileDialog::SetDefaultExtention(const wchar_t * zExtension)
{
	//SetDefExt(zExtension);
	msDefExt = zExtension;
}
