#pragma once

class CRightResult
{
public:
	CRightResult(FILE *pf, const wchar_t *zName);
	CString msName;
	double mDue;
};

class CHolidaysDef
{
public:
	CHolidaysDef(const wchar_t *zfName);
	CString msfName;
	CList<CString *, CString *> mNames;
	void Log(FILE *pfLog);
	bool Includes(CList<CString *, CString *> &usedHolidays, FILE *pfLog = NULL);
};

class CVerify
{
public:
	CVerify(const wchar_t *zfName, bool bSilent = false);
	~CVerify();

	static CString umsfName;
	static bool umbBreakonDiff;
	static FILE* umpfReport;
	static int umiBatch;
	static bool umbDisplayDiff;
	static bool umbOld;

	static DWORD WINAPI StaticVerifyBatchOld(LPVOID);
	static DWORD WINAPI StaticVerifyBatch(LPVOID);
	static void VerifyBatch(const wchar_t *zfName, bool bOld = false);

	CString msfName;
	bool Verify();
	bool VerifyOld();
	bool ReadSavedXml();
	bool ReadOldFile();
	void ReadTime(FILE *pfRead, int i);
	CList<CRightResult *, CRightResult *> mResults;
	CList<CHolidaysDef *, CHolidaysDef *> mHolidaysDefs;
	CList<CString *, CString *> mUsedHolidays;

	int mnTimesDefined;
	FILE *mpfLog;
	int ReadResults(FILE *pfRead);
	void Clear();
	bool VerifyResults();
	bool VerifyResultsOld();
	void ReadHolidaysDefinitions();
	CString SelectHolidays();
	CString GetLogFileNameFor(const wchar_t *zRight);
	bool ReadCurrentHolidaysNames(void);
	CString msFirstName;
	CString msFamilyName;
	int mDebug;
	void WriteSummary(FILE *pf);
	bool mbAllSame;
	int mnCompared;
	int mnSame;
	int mnDiff;
	int mnMissing;
	bool mbSilentMode;
	void CheckIfNoticeSet();

	class CXMLParse* mpSavedXml;
	class CXMLParseNode* mpSavedXmlRoot;
	class CXMLParseNode* mpSavedResults;
};

