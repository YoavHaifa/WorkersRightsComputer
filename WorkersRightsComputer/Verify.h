#pragma once


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

	bool Verify();
	static void StartVerifyBatch(const wchar_t *zfName);

protected:
	static CString umsfName;
	static bool umbBreakonDiff;
	static FILE* umpfReport;
	static int umiBatch;
	static bool umbDisplayDiff;

	static DWORD WINAPI StaticVerifyBatch(LPVOID);
	static void OpenBatchReport(const CString& sPath);

	CString msfName;
	void IdentifyLegacyVersion(class CXMLParseNode* pRoot);
	bool ReadSavedFile();
	void ReadTime(FILE *pfRead, int i);
	CList<class CRightResult *, class CRightResult *> mResults;
	CList<CHolidaysDef *, CHolidaysDef *> mHolidaysDefs;
	CList<CString *, CString *> mUsedHolidays;

	int mnTimesDefined;
	FILE *mpfLog;
	//int ReadResults(FILE *pfRead);
	void Clear();
	bool VerifyResults();
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
};
