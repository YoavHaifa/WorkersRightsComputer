#pragma once

class CMyRegistry
{
public:
    CMyRegistry(const wchar_t *zPath, REGSAM samDesired = KEY_READ, bool bMustRead = false);
    CMyRegistry(REGSAM samDesired = KEY_READ);
    ~CMyRegistry(void);
	void DoConsturct(bool bMustRead = false);
	bool Open(void);
    operator bool(){return mbOpen;}
    CString msPath;
	HKEY mhKey;
    bool mbOpen;
    REGSAM mSamDesired;

    bool Set(const wchar_t * zName, DWORD value);
    bool Set(const wchar_t * zName, const wchar_t *zValue);

    bool Get(const wchar_t * zName, int &value);
    bool Get(const wchar_t * zName, DWORD &value);
    bool Get(const wchar_t * zName, bool &value);
    bool Get(const wchar_t * zName, CString &sValue);

    template<typename T>
	static bool GetAppReg(const wchar_t * zName, T &value)
	{
		CMyRegistry myReg;
		if (!myReg)
			return false;
		return myReg.Get(zName, value);
	}

	class CXMLParseNode *GetXmlNode(const wchar_t *zName);

	static void SetLogFile(FILE *pfLog);

private:
	class CXMLParse *mpXMLParseFile;
	class CXMLParseNode *mpXMLParseRoot;
	static bool umbLookedForXml;
	static CString umsXmlFileName;
	bool LookForXml(void);
	bool VerifyXml(void);
	static FILE *umpfLog;
};
