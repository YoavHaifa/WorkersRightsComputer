#include "stdafx.h"
#include "HolidaysByDay.h"
#include "XmlDump.h"
#include "XmlParse.h"

CHolidaysByDay gHolidaysByDay;
CString msNotSelected(L"");

CHolidaysByDay::CHolidaysByDay()
	: mbDefined(false)
{

}
bool CHolidaysByDay::LoadFromXml(class CXMLParseNode* pNode)
{

}
void ResetAllInputs()
{
	CHolidaysByDay::mComboHolidays.SetWindowTextW(L"Select set of Holidays");
}
