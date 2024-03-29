#pragma once
#include "WagePeriod.h"

class CWage
{
public:
	CWage();

	bool IsValid();

	void SetMinWage(void);
	void SetMonthlyWage(double wage);
	void SetHourlyWage(double wagePerHour, double nHoursPerWeek);

	//void Restore(FILE* pfRead);
	bool IsSinglePeriod() { return mPeriods.GetSize() == 1; }
	bool IsSinglePeriod(EWageMode& oeMode);
	bool IsAllMin();
	int NPeriods() { return (int)mPeriods.GetSize(); }
	class CWagePeriod *GetLastPeriod() { return mPeriods.GetTail(); }

	bool AddPeriod(class CWagePeriod*);

	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pMain);

	double GetMonthlyWage();
	double GetHourlyWage();
	double GetHoursPerMonth();

	CString GetStateDesc();
	bool CheckList();

	CString GetShortText();

	POSITION GetHeadPosition() { return mPeriods.GetHeadPosition(); }
	class CWagePeriod* GetNext(POSITION &pos) { return mPeriods.GetNext(pos); }

	int mDebug;

private:
	class CWagePeriod* mpFirst;
	class CWagePeriod* mpLast;
	void SetSingle(EWageMode eMode, double wage = 0, double nHours = 0);
	CList<class CWagePeriod*, class CWagePeriod*> mPeriods;
	void Clear();
	void ClearOverlap(CWagePeriod& newPeriod);
	void Update();
	void UniteAdjacentFileds();
	bool UniteAdjacentPair();
public:
	bool VerifyWorkPeriod();
};

extern CWage gWage;

