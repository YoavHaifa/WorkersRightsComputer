#pragma once
#include "MyDialogEx.h"

class CWageDefBaseDlg : public CMyDialogEx
{
public:
	CWageDefBaseDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL);

	virtual BOOL OnInitDialog();
};

