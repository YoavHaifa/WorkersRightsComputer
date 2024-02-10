#include "stdafx.h"
#include "WageDefBaseDlg.h"
#include "WorkPeriod.h"
#include "Resource.h"

CWageDefBaseDlg::CWageDefBaseDlg(UINT nIDTemplate, CWnd* pParentWnd)
	: CMyDialogEx(nIDTemplate, pParentWnd)
{

}

BOOL CWageDefBaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (gWorkPeriod.mbCaregiver)
	{
		SetInvisible(IDC_RADIO_HOURLY);
		SetInvisible(IDC_EDIT_HOUR_SALARY);
		SetInvisible(IDC_STATIC_HOURLY_TEXT);
		SetInvisible(IDC_EDIT_HOURS_PER_MONTH);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

