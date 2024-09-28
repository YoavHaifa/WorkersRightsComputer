#pragma once

class CHolidaysByDay
{
public:
	CHolidaysByDay();

	bool LoadFromXml(class CXMLParseNode* pNode);
	void ResetAllInputs();

private:
	bool mbDefined;
	CString msSelectedHolidays;
};

extern CHolidaysByDay gHolidaysByDay;

