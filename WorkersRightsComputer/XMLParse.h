#pragma once
#include "afxtempl.h"

class CXMLParseNode
{
public:
    CXMLParseNode(const wchar_t *zName, const wchar_t *zValue = NULL);
    CXMLParseNode(class CXMLParse &file);
    ~CXMLParseNode(void);

	static int umnIndent;
    CString msName;
    CString msValue;

    CXMLParseNode * GetFirst(const wchar_t * zName);
    CXMLParseNode * GetFirst(const wchar_t * zName1, const wchar_t * zName2);
    CXMLParseNode * GetFirstByNameAndValue(const wchar_t * zName, const wchar_t * zValue);

    CXMLParseNode * GetFirstDeep(const wchar_t * zName);

	bool IsCalled(const wchar_t * zObjectName);
    CXMLParseNode * GetDFSByName(const wchar_t * zNodeName, const wchar_t * zObjectName);
    CXMLParseNode * GetDFS(const wchar_t * zName);
    CXMLParseNode * GetNextDFS(const wchar_t * zName, CXMLParseNode **ppAfter, bool bShallow = false);

    CXMLParseNode * GetNext(const wchar_t * zName, CXMLParseNode  * pAfter);
    template <typename T> bool GetValue(const wchar_t * zName, T & value)
    {
        CXMLParseNode * pNode = GetFirst(zName);
        if (!pNode)
            return false;
        return pNode->GetValue(value);
    }
    //bool GetValue(wchar_t *zBuf);
    bool GetValue(bool & bValue);
    bool GetValue(int & value);
    bool GetValue(short & value);
    bool GetHexaValue(unsigned long & value);
    bool GetValue(unsigned long & value);
    bool GetValue(long & value);
    bool GetValue(float & value);
    bool GetValue(CString & sValue);

    int GetValues(const wchar_t * zName, CList<CString*, CString*> &valuesList);
    bool ValueIs(const wchar_t *zValue);
	bool ValueStartsWith(const wchar_t *zPrefix);

    bool GetColor(COLORREF & color);
	bool IsOfClass(const wchar_t *zClass);
	bool IsNamed(const wchar_t *zName);
	bool NameIsPrefixedBy(const wchar_t *zNamePrefix);

    CString GetFirstDeepString(const wchar_t * zName, const wchar_t *zDefault);
	POSITION GetHeadPosition(void);
	class CXMLParseNode *GetNext(POSITION &pos);

protected:
    CList<class CXMLParseNode*,class CXMLParseNode*> mChildren;
	static FILE *umpfDebug;

public:
    CString GetObjectName(void);
    void DumpOnError(class CXMLDump &xmlDump);
	static void SetDebugFile(FILE *pfDebug);
	void Print(FILE *pf);
	void Indent(FILE *pf);
	bool GetValueByKey(int key, float &oValue);
	void Add(CXMLParseNode *pNode);
	CXMLParseNode* Set(const wchar_t *zName, const wchar_t *zValue);
	CXMLParseNode* Add(const wchar_t *zName, const wchar_t *zValue);
};

class CXMLParse
{
public:
    typedef enum EXmlElementType
    {
        XML_UNDEF = 0,
        XML_START = 1,
        XML_END = 2,
        XML_VALUE = 3,
        XML_ATTRIBUTE = 4, // Not Supported Yet
        XML_ERROR = 5,
        XML_LAST = 6
    } EXmlElementType;

    CXMLParse(void);
    CXMLParse(const wchar_t *zDir, const wchar_t *zName, const wchar_t *zRoot);
	CXMLParse(const wchar_t* zfName, int offset = 0);
	CXMLParse(const wchar_t* zfName, bool bUnicode);
	CXMLParse(const wchar_t *zName, const wchar_t *pMemory, int len);
    ~CXMLParse(void);

	bool OpenFile(const wchar_t *zfName, int offset = 0, bool bUnicode = false);
	void SetNew(const wchar_t *zfName);

protected:
    CList<CString*, CString*> mPath;
    FILE *mpf;
    const wchar_t *mpMemoryToParse;
	int mMemoryLen;
	int mMemoryLenUsed;
    CString msName;
    CString msNextToken;
    EXmlElementType meNextType;
    CXMLParseNode *mpRoot;
    wchar_t mNextChar;
    bool mbNextCharReady;

    bool ReadNextChar(void);
    bool ReadNextNonBlankChar(void);
    bool ReadName(void);

    static const CString umsBlanks;
    static const CString umsNameEnd;

public:
    EXmlElementType ParseNextToken(void);
    CString & GetNextToken(){return msNextToken;}
    bool IsEmpty(void);
    CXMLParseNode * GetRoot(void);
    void DumpOnError(void);
    void Dump(void);
	CString GetName(void){return msName;}
	static bool GetValueByKey(const wchar_t *zfName, int key, float &oValue);
};
