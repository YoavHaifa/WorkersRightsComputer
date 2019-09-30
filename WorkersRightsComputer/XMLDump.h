#pragma once

class CXMLDump
{
public:
    CXMLDump(const wchar_t *zDir, const wchar_t *zName, const wchar_t *zRoot);
    CXMLDump(const wchar_t *zName, const wchar_t *zRoot);
    ~CXMLDump(void);
	void OnBuild(class CFileName &fName);
	void Close(void);
    CString msName;
    CString msRoot;
    FILE *mpf;
    int mnIdent;
    int mId;
    static int umSetId;
	static bool umbDebug;
	static bool umbReportError;
	static CString umsDefaultPath;

	CString GetName(void){return msName;}
    void Start(const wchar_t * zName);
    void Ident(void);
    void End(const wchar_t * zName);
    void Write(const wchar_t * zName, bool bValue);
    void Write(const wchar_t * zName, int value);
    void Write(const wchar_t * zName, ULONGLONG value);
    void Write(const wchar_t * zName, unsigned int value);
	void Write(const wchar_t * zName, float value);
	void Write(const wchar_t * zName, double value);
	void Write(const wchar_t * zName, const wchar_t * zValue);
	void Write(const wchar_t * zName, class CMyTime &time);
	template <typename T> void WriteArray (const wchar_t * zName, T *pArray, int n)
    {
        char zBuf[128];
        CXMLDumpScope scope(zName, *this);
        for (int i = 0; i < n; i++)
        {
            sprintf_s (zBuf, 128, "value_%d", i+1);
            Write (zBuf, pArray[i]);
        }   
    }
    void SaveColor(COLORREF color);
    operator bool(){return mpf != NULL;}
	static void SetDeafultPath(const wchar_t *zPath = NULL);
};

class CXMLDumpScope
{
public:
    CXMLDumpScope(const wchar_t *zName, CXMLDump &XMLDump);
    CXMLDumpScope(const wchar_t *zName, CXMLDump *pXMLDump);
    ~CXMLDumpScope(void);
private:
    CString msName;
    CXMLDump *mpXMLDump;
};
