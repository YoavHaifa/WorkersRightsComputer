#pragma once

class CUsedVacations
{
public:
	CUsedVacations();
	~CUsedVacations();

	void AddVacation(class CMyTime &firstDay, CMyTime &lastDay, 
		bool bMaternity, int nMaternityPaidWeeks, bool bMaternityPension);
	//void Save(FILE *pfSave);
	void SaveToXml(class CXMLDump &xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

	//void Restore(FILE *pfRead);
	bool IsEmpty(void) { return mVacationsUsed.IsEmpty(); }

	CList<class CVacationUsed*, class CVacationUsed*> mVacationsUsed;
	void ClearLastVacation(void);
	void ClearAllVacations(void);
	CString GetVacationsShortText(void);
	CString GetVacationsFullText(void);
	void Compute();
	void Log();
	bool mbAdd14DaysUnpaidVacation4Severance;

	void UpdateNextYearStart(class CMyTime &yearStart, class CMyTime &nextYearStart);
	void WriteToLetter(class CHtmlWriter& html);
	void WriteToLetterExtraSeverance(class CHtmlWriter& html);

	int CountDaysOfUnpaidVacation(CMyTime& first, CMyTime& last);
	void AddAllVacationsToWorkSpan(class CWorkSpan &workSpan);
	double GetSeveranceYearsForUnpaidVacation();

	static bool umbPrintUnpaid;
};

extern CUsedVacations gUsedVacations;
