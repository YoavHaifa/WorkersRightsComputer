#include "stdafx.h"
#include "HolidaysByDay.h"
#include "XmlDump.h"
#include "XmlParse.h"
#include "RealHolidaysDlg.h"
#include "AllRights.h"

CHolidaysByDay gHolidaysByDay;
CString msNotSelected(L"");

CHolidaysByDay::CHolidaysByDay()
	: mbDefined(false)
{
	mpDlg = new CRealHolidaysDlg();
}
void CHolidaysByDay::SetSelectionFromOldSave(const CString& sText)
{
	msSelectedHolidays = sText;
	mbDefined = true;
	//mpDlg->mComboHolidays.SetWindowTextW(sText);
}
bool CHolidaysByDay::LoadFromXml(class CXMLParseNode* pNode)
{
	return true;
}
void CHolidaysByDay::SaveToXml(class CXMLDump& xmlDump)
{
	CXMLDumpScope mainScope(L"HolidaysByDay", xmlDump);
	xmlDump.Write(L"Holidays", (const wchar_t*)msSelectedHolidays);

	/*
	CString sText;
	mpDlg->mComboHolidays.GetWindowText(sText);
	if (sText.IsEmpty())
		sText = L"-";
	xmlDump.Write(L"Holidays", (const wchar_t*)sText);
	*/

}
void CHolidaysByDay::ResetAllInputs()
{
	if (mbDefined)
		msSelectedHolidays = L"Select set of Holidays";
		//mpDlg->mComboHolidays.SetWindowTextW(L"Select set of Holidays");
}
CString CHolidaysByDay::GetHolidaysSet()
{
	if (!mbDefined)
		return CString(L"Holidays not defined");
	return msSelectedHolidays;
	/*
	CString sSelection;
	mpDlg->mComboHolidays.GetWindowTextW(sSelection);
	return sSelection;*/
}
bool CHolidaysByDay::UpdateDataFromDialog()
{
	return mpDlg->UpdateDataFromDialog();
}
CString CHolidaysByDay::GetDaysText()
{
	return mpDlg->GetDaysText();
}
/*
DWORD WINAPI CHolidaysByDay::StaticThreadFunc(LPVOID)
{
	static int count = 0;
	count++;
	while (1)
	{
		Sleep(100);
		CString s = gHolidaysByDay.GetHolidaysSet();
		if (!s.IsEmpty() && s != "Select set of Holidays")
			if (s != gAllRights.GetHolidaysSelection())
				gpDlg->OnInputChange();
	}
	return 0;
}*/
