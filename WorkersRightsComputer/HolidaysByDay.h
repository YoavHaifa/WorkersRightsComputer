#pragma once

class CHolidaysByDay
{
public:
	CHolidaysByDay();

	//bool LoadFromXml(class CXMLParseNode* pNode);
	void SaveToXml(class CXMLDump& xmlDump);

	void SetSelectionFromSave(const CString& sText);
	void SetSelectionByUser(const CString& sText);
	void ResetAllInputs();
	CString GetHolidaysSet();
	bool UpdateDataFromDialog();
	bool IsHolidaysSetDefined();

	CString msSelectedHolidays;

private:
	//static DWORD WINAPI StaticThreadFunc(LPVOID);

	bool mbDefined;
};

extern CHolidaysByDay gHolidaysByDay;

