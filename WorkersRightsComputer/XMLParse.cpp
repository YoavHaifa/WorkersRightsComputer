#include "StdAfx.h"
#include "XmlParse.h"
#include "Utils.h"
#include "FileName.h"
#include "MyTime.h"
//#include "XMLDump.h"

FILE *CXMLParseNode::umpfDebug = NULL;
int CXMLParseNode::umnIndent = 0;

CXMLParseNode::CXMLParseNode(const wchar_t *zName, const wchar_t *zValue)
: msName(zName)
, msValue(zValue)
{
}

CXMLParseNode::CXMLParseNode(CXMLParse &file)
: msName(file.GetNextToken())
, msValue(L"")
{
    bool bEnd = false;
    while (!bEnd)
    {
        switch (file.ParseNextToken())
        {
        case CXMLParse::XML_START:
            mChildren.AddTail(new CXMLParseNode(file));
            break;
        case CXMLParse::XML_VALUE:
            msValue += file.GetNextToken();
            break;
        case CXMLParse::XML_END:
        case CXMLParse::XML_ERROR:
        default:
            bEnd = true;
            break;
        }
    }
}

CXMLParseNode::~CXMLParseNode(void)
{
    POSITION pos = mChildren.GetHeadPosition();
    while (pos)
    {
        CXMLParseNode *pNode = mChildren.GetNext(pos);
        delete pNode;
    }
}

void CXMLParseNode::SetDebugFile(FILE *pfDebug)
{
	umpfDebug = pfDebug;
}
POSITION CXMLParseNode::GetHeadPosition(void)
{
	return mChildren.GetHeadPosition();
}
CXMLParseNode *CXMLParseNode::GetNext(POSITION &pos)
{
	return mChildren.GetNext(pos);
}
CXMLParseNode * CXMLParseNode::GetFirst(const wchar_t * zName)
{
	if (mChildren.IsEmpty())
		return NULL;

    POSITION pos = mChildren.GetHeadPosition();
    while (pos)
    {
        CXMLParseNode *pNode = mChildren.GetNext(pos);
        if (!zName || (pNode->msName == zName))
            return pNode;
    }
    return NULL;
}
CXMLParseNode * CXMLParseNode::GetFirst(const wchar_t * zName1, const wchar_t * zName2)
{
    CXMLParseNode *pChild = GetFirst(zName1);
    if (!pChild)
        return false;
    return pChild->GetFirst(zName2);
}
CXMLParseNode * CXMLParseNode::GetFirstByNameAndValue(const wchar_t * zName, const wchar_t * zValue)
{
	if (mChildren.IsEmpty())
		return NULL;

    POSITION pos = mChildren.GetHeadPosition();
    while (pos)
    {
        CXMLParseNode *pNode = mChildren.GetNext(pos);
        if ((pNode->msName == zName) && (pNode->msValue == zValue))
            return pNode;
    }
    return NULL;
}
CXMLParseNode * CXMLParseNode::GetFirstDeep(const wchar_t * zName)
{
    if (zName)
    {
        if (msName == zName)
            return this;

        POSITION pos = mChildren.GetHeadPosition();
        while (pos)
        {
            CXMLParseNode *pNode = mChildren.GetNext(pos);
            if (!zName || (pNode->msName == zName))
                return pNode;
        }
    }

    if (mChildren.IsEmpty())
    {
        if (!zName)
            return this;
    }
    else
    {
        CXMLParseNode *pFirstNode = mChildren.GetHead();
        if (pFirstNode)
            return pFirstNode->GetFirstDeep(zName);
    }
    return NULL;
}
bool CXMLParseNode::IsCalled(const wchar_t * zObjectName)
{
	CXMLParseNode * pNameNode = GetFirstDeep(L"name");
	if (!pNameNode)
		return false;
	return pNameNode->msValue == zObjectName;
}
CXMLParseNode * CXMLParseNode::GetDFSByName(const wchar_t * zNodeName, const wchar_t * zObjectName)
{
	CXMLParseNode * pNode = GetDFS(zNodeName);
	while (pNode)
	{
		if (pNode->IsCalled(zObjectName))
				return pNode;

		pNode = GetNextDFS(zNodeName, &pNode);
	}
    return NULL;
}
CXMLParseNode * CXMLParseNode::GetDFS(const wchar_t * zName)
{
    if (msName == zName)
        return this;

    POSITION pos = mChildren.GetHeadPosition();
    while (pos)
    {
        CXMLParseNode *pNode = mChildren.GetNext(pos);
        CXMLParseNode *pResult = pNode->GetDFS(zName);
        if (pResult)
            return pResult;
    }
    return NULL;
}
CXMLParseNode * CXMLParseNode::GetNextDFS(const wchar_t * zName, CXMLParseNode  **ppAfter, bool bShallow)
{
	static int depth = 0;
	depth++;
	if (umpfDebug)
		fwprintf (umpfDebug, L"<GetNextDFS> %2d msName %s (Looking for %s) *ppAfter %p\n", 
											depth, (const wchar_t *)msName, zName, *ppAfter);

    if (msName == zName)
    {
        if (!*ppAfter)
		{
			if (umpfDebug)
				fwprintf (umpfDebug, L"<GetNextDFS> %d %s ============> Found\n\n", depth, (const wchar_t *)msName);
			depth--;
            return this;
		}
        if (this == *ppAfter)
		{
			if (umpfDebug)
				fwprintf (umpfDebug, L"<GetNextDFS> Arrived at <pAfter>\n");
            *ppAfter = NULL;
		}
		if (bShallow)
		{
			if (umpfDebug)
				fwprintf (umpfDebug, L"<GetNextDFS> Skip\n");
			depth--;
			return NULL;
		}
    }

	if (umpfDebug)
	{
		int nChildren = (int)mChildren.GetSize();
		if (nChildren > 0)
			fwprintf (umpfDebug, L"<GetNextDFS> nChildren %d\n", nChildren);
	}

	int iChild = 0;
    POSITION pos = mChildren.GetHeadPosition();
    while (pos)
    {
        CXMLParseNode *pChildNode = mChildren.GetNext(pos);
		if (umpfDebug)
			fwprintf (umpfDebug, L"<GetNextDFS> child %d %s\n", ++iChild, (const wchar_t *)pChildNode->msName);
        CXMLParseNode *pResult = pChildNode->GetNextDFS(zName, ppAfter, bShallow);
        if (pResult)
		{
			if (umpfDebug)
				fwprintf (umpfDebug, L"<GetNextDFS> %d %s ==> After Found\n", depth, (const wchar_t *)msName);
			depth--;
            return pResult;
		}
    }
	depth--;
	if (umpfDebug)
		fwprintf (umpfDebug, L"<GetNextDFS> %d msName %s Failed\n\n", depth, (const wchar_t *)msName);
    return NULL;
}
CString CXMLParseNode::GetFirstDeepString(const wchar_t *zName, const wchar_t *zDefault)
{
    if (msName == zName)
        return msValue;
    if (mChildren.IsEmpty())
        return CString(zDefault);

    CXMLParseNode *pNamedNode = GetFirst(zName);
    if (pNamedNode)
        return pNamedNode->msValue;

    return mChildren.GetHead()->GetFirstDeepString(zName, zDefault);
}
CXMLParseNode * CXMLParseNode::GetNext(const wchar_t * zName, CXMLParseNode  * pAfter)
{
    POSITION pos = mChildren.Find(pAfter);
    if (pos)
        mChildren.GetNext(pos);
    while (pos)
    {
        CXMLParseNode *pNode = mChildren.GetNext(pos);
        if (!zName || (pNode->msName == zName))
            return pNode;
    }
    return NULL;
}
CString CXMLParseNode::GetObjectName(void)
{
    CXMLParseNode *pNameNode = GetFirst(L"name");
    if (!pNameNode)
        pNameNode = GetFirst(L"Object", L"name");
    if (pNameNode)
        return pNameNode->msValue;
    return CString();
}
int CXMLParseNode::GetValues(const wchar_t * zName, CList<CString*, CString*> &valuesList)
{
	if (mChildren.IsEmpty())
		return 0;

	int n = 0;
    POSITION pos = mChildren.GetHeadPosition();
    while (pos)
    {
        CXMLParseNode *pNode = mChildren.GetNext(pos);
        if (!zName || (pNode->msName == zName))
		{
			valuesList.AddTail(new CString(pNode->msValue));
			n++;
		}
    }
	return n;
}

/*bool CXMLParseNode::GetValue(const wchar_t * zName, CString & sValue)
{
    CXMLParseNode * pNode = GetFirst(zName);
    if (!pNode)
        return false;
    return pNode->GetValue(sValue);
} */
/*bool CXMLParseNode::GetValue(wchar_t *zBuf)
{
    zBuf[0] = 0;
    if (msValue.IsEmpty())
        return false;
    strcpy (zBuf, msValue);
    return true;
} */
bool CXMLParseNode::GetValue(bool & bValue)
{
    if (msValue.IsEmpty())
        return false;
    if (msValue == L"false" || msValue == L"no" 
        || msValue == L"FALSE" || msValue == L"NO" 
        || msValue == L"0")
        bValue = false;
    else
        bValue = true;
    return true;
}
bool CXMLParseNode::GetValue(int & value)
{
    if (msValue.IsEmpty())
        return false;
    wchar_t first = msValue[0];
    if (isdigit(first) || first == '-')
    {
        value = _wtoi(msValue);
        return true;
    }
    return false;
}
bool CXMLParseNode::GetValue(short & value)
{
    if (msValue.IsEmpty())
        return false;
    wchar_t first = msValue[0];
    if (isdigit(first) || first == '-')
    {
        value = (short)_wtoi(msValue);
        return true;
    }
    return false;
}
bool CXMLParseNode::GetHexaValue(unsigned long & value)
{
    if (msValue.IsEmpty() || (msValue.GetLength() < 3))
        return false;

    const wchar_t *zValue = (const wchar_t *)msValue;
    if (zValue[0] == '0')
        zValue++;
    if (zValue[0] == 'x' || zValue[0] == 'X')
        zValue++;

    wchar_t first = zValue[0];
    if (isdigit(first) || (first >= 'a' && first <= 'f') || (first >= 'A' && first <= 'F'))
    {
        swscanf_s (zValue, L"%x", &value);
        return true;
    }
	return false;
}
bool CXMLParseNode::GetValue(unsigned long & value)
{
    if (msValue.IsEmpty())
        return false;
    wchar_t first = msValue[0];
	if (first == '0')
	{
		if (msValue.GetLength() > 2)
		{
			wchar_t second = msValue[1];
			if (second == 'x' || second == 'X')
				return GetHexaValue(value);
		}
	}
    if (isdigit(first) || first == '-')
    {
        value = _wtoi(msValue);
        return true;
    }
    return false;
}
bool CXMLParseNode::GetValue(long & value)
{
    if (msValue.IsEmpty())
        return false;
    wchar_t first = msValue[0];
    if (isdigit(first) || first == '-')
    {
        value = _wtoi(msValue);
        return true;
    }
    return false;
}
bool CXMLParseNode::GetValue(float& value)
{
	if (msValue.IsEmpty())
		return false;
	wchar_t first = msValue[0];
	if (isdigit(first) || first == '-')
	{
		value = (float)_wtof(msValue);
		return true;
	}
	return false;
}
bool CXMLParseNode::GetValue(double& value)
{
	if (msValue.IsEmpty())
		return false;
	wchar_t first = msValue[0];
	if (isdigit(first) || first == '-')
	{
		value = _wtof(msValue);
		return true;
	}
	return false;
}
bool CXMLParseNode::GetValue(CString & sValue)
{
    if (msValue.IsEmpty())
        return false;
    sValue = msValue;
    return true;
}
bool CXMLParseNode::GetValue(CMyTime& time)
{
	CString sTime;
	if (!GetValue(L"time", sTime))
	{
		time.Reset();
        return false;
	}

	if (sTime.IsEmpty())
	{
		time.Reset();
        return false;
	}

	__int64 t = _wtoi64(sTime);
	if (t == 0)
		time.Reset();
	else
		time.Set(t);
	return true;
}
bool CXMLParseNode::ValueIs(const wchar_t *zValue)
{
	return msValue == zValue;
}
bool CXMLParseNode::ValueStartsWith(const wchar_t *zPrefix)
{
	CString s(msValue.Left((int)wcslen(zPrefix)));
	return s == zPrefix;
}
bool CXMLParseNode::GetColor(COLORREF & color)
{
    CXMLParseNode * pNode = GetFirstDeep(L"color");
    if (!pNode)
        return false;

	unsigned long lValue;
	if (pNode->GetHexaValue(lValue))
	{
		color = lValue;
		return true;
	}

    return false;
}
bool CXMLParseNode::IsOfClass(const wchar_t *zClass)
{
    CXMLParseNode * pClassNode = GetFirstDeep(L"class");
    if (!pClassNode)
		return false;
	
	return pClassNode->ValueIs(zClass);
}
bool CXMLParseNode::IsNamed(const wchar_t *zName)
{
    CXMLParseNode * pNameNode = GetFirstDeep(L"name");
    if (!pNameNode)
		return false;
	
	return pNameNode->ValueIs(zName);
}
bool CXMLParseNode::NameIsPrefixedBy(const wchar_t *zNamePrefix)
{
    CXMLParseNode * pNameNode = GetFirstDeep(L"name");
    if (!pNameNode)
		return false;
	
	return pNameNode->ValueStartsWith(zNamePrefix);
}
void CXMLParseNode::Indent(FILE *pf)
{
	int count = umnIndent;
	while (count--)
		fprintf (pf, "   ");
}
void CXMLParseNode::Print(FILE *pf)
{
	Indent(pf);
	fwprintf (pf, L"<%s>", (const wchar_t *)msName);
	if (!msValue.IsEmpty())
		fwprintf (pf, L" %s", (const wchar_t *)msValue);

    POSITION pos = mChildren.GetHeadPosition();
	if (pos)
	{
		fwprintf (pf, L"\n");
		umnIndent++;
		while (pos)
		{
			CXMLParseNode *pNode = mChildren.GetNext(pos);
			pNode->Print(pf);
		}
		Indent(pf);
		umnIndent--;
	}
	fwprintf (pf, L" </%s>\n", (const wchar_t *)msName);
	if (!umnIndent)
		fwprintf (pf, L"\n");
}

//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

const CString CXMLParse::umsBlanks(L" \t\n");
const CString CXMLParse::umsNameEnd(L"<>=\"'.()[]-+\\");

CXMLParse::CXMLParse(void)
: msName(L"")
, mpf(NULL)
, mpMemoryToParse(NULL)
, mMemoryLen(0)
, mMemoryLenUsed(0)
, mpRoot(NULL)
, msNextToken(L"")
, mbNextCharReady(false)
, mNextChar (0)
, meNextType (XML_UNDEF)
{
}
CXMLParse::CXMLParse(const wchar_t *zDir, const wchar_t *zName, const wchar_t *zRoot)
: msName(zName)
, mpf(NULL)
, mpMemoryToParse(NULL)
, mMemoryLen(0)
, mMemoryLenUsed(0)
, mpRoot(NULL)
, msNextToken(L"")
, mbNextCharReady(false)
, mNextChar (0)
, meNextType (XML_UNDEF)
{
	CFileName fName(zName);
	fName.ChangePath(zDir);
	fName.AddMetaTypeAndType(zRoot, L"xml");
	OpenFile(fName);
}
CXMLParse::CXMLParse(const wchar_t* zfName, int offset)
	: msName(zfName)
	, mpf(NULL)
	, mpMemoryToParse(NULL)
	, mMemoryLen(0)
	, mMemoryLenUsed(0)
	, mpRoot(NULL)
	, msNextToken(L"")
	, mbNextCharReady(false)
	, mNextChar(0)
	, meNextType(XML_UNDEF)
{
	OpenFile(zfName, offset);
}
CXMLParse::CXMLParse(const wchar_t* zfName, bool bUnicode)
	: msName(zfName)
	, mpf(NULL)
	, mpMemoryToParse(NULL)
	, mMemoryLen(0)
	, mMemoryLenUsed(0)
	, mpRoot(NULL)
	, msNextToken(L"")
	, mbNextCharReady(false)
	, mNextChar(0)
	, meNextType(XML_UNDEF)
{
	OpenFile(zfName, 0, bUnicode);
}
CXMLParse::CXMLParse(const wchar_t *zName, const wchar_t *pMemory, int len)
: msName(zName)
, mpf(NULL)
, mpMemoryToParse(pMemory)
, mMemoryLen(len)
, mMemoryLenUsed(0)
, mpRoot(NULL)
, msNextToken(L"")
, mbNextCharReady(false)
, mNextChar (0)
, meNextType (XML_UNDEF)
{
	static int count = 0;
	count++;
    if (ParseNextToken() == XML_START)
    {
        mpRoot = new CXMLParseNode (*this);
    }
	//if (count == 1)
	//	DumpOnError();
}
CXMLParse::~CXMLParse(void)
{
    if (mpRoot)
        delete mpRoot;
}
bool CXMLParse::OpenFile(const wchar_t *zfName, int offset, bool bUnicode)
{
	msName = zfName;
    if (mpRoot)
        delete mpRoot;
	mpRoot = NULL;
	if (bUnicode)
		mpf = MyFOpenWithErrorBox(msName, L"r, ccs=UNICODE", L"for restoring");
	else
		mpf = MyFOpen (msName, L"r");
    if (!mpf)
        return false;

	if (offset)
		fseek(mpf, offset, SEEK_SET);

    if (ParseNextToken() == XML_START)
    {
        mpRoot = new CXMLParseNode (*this);
    }
    fclose (mpf);
    mpf = NULL;
	return mpRoot != NULL;
}
void CXMLParse::SetNew(const wchar_t *zfName)
{
	msName = zfName;
    if (mpRoot)
        delete mpRoot;
	mpRoot = new CXMLParseNode(L"root");
}
CXMLParse::EXmlElementType CXMLParse::ParseNextToken(void)
{
    if (!ReadNextNonBlankChar())
        return XML_ERROR;

    if (mNextChar == L'<') // Start Tag
    {
        mbNextCharReady = false;
        if (!ReadNextNonBlankChar())
            return XML_ERROR;

        if (mNextChar == '/')
        {
            meNextType = XML_END;
            mbNextCharReady = false;
        }
        else
            meNextType = XML_START;

        if (!ReadName())
            return XML_ERROR;

        while (ReadNextNonBlankChar())
        {
            mbNextCharReady = false;
            if (mNextChar == '>')
                break;
        }
    }
    else // Non Tag Value
    {
        msNextToken = mNextChar;
        mbNextCharReady = false;
        meNextType = XML_VALUE;

        while (ReadNextChar())
        {
            if (mNextChar == '<')
                break;

            msNextToken += mNextChar;
            mbNextCharReady = false;
        }
        // Delete Last Blanks
        msNextToken.TrimRight(umsBlanks);
    }
    return meNextType;
}

bool CXMLParse::IsEmpty(void)
{
    return (mpRoot == NULL);
}

CXMLParseNode * CXMLParse::GetRoot(void)
{
    return mpRoot;
}

bool CXMLParse::ReadNextChar(void)
{
    if (!mbNextCharReady)
    {
		if (mpMemoryToParse)
		{
			if (mMemoryLenUsed >= mMemoryLen)
				return false;
			mNextChar = mpMemoryToParse[mMemoryLenUsed++];
		}
		else
		{
			int ch = fgetwc(mpf);
			if (ch == WEOF)
				return false;
			mNextChar = (wchar_t)ch;
		}
        mbNextCharReady = true;
    }
    return true;
}
bool CXMLParse::ReadNextNonBlankChar(void)
{
    while (ReadNextChar())
    {
        if (umsBlanks.Find(mNextChar) < 0)
            return true;
        mbNextCharReady = false;
    }
    return false;
}

bool CXMLParse::ReadName(void)
{
    msNextToken = L"";
    if (!ReadNextNonBlankChar())
        return false;

    msNextToken = mNextChar;
    mbNextCharReady = false;
    while (ReadNextChar())
    {
        if (umsBlanks.Find(mNextChar) >= 0)
            return true;

        if (umsNameEnd.Find(mNextChar) >= 0)
            return true;

        msNextToken += mNextChar;
        mbNextCharReady = false;
    }

    return true;
}
/*
void CXMLParse::Dump(void)
{
    CXMLDump xmlDump(msName, L"");
    if (mpRoot)
        mpRoot->DumpOnError(xmlDump);
}
void CXMLParse::DumpOnError(void)
{
    CFileName name(msName);
    CString sDumpName (CMyWindows::GetLogDir() += "\\");
    sDumpName += name.Private();
    if (sDumpName.Right(4) == ".xml")
        sDumpName = sDumpName.Left(sDumpName.GetLength() - 4);

    CXMLDump xmlDump(sDumpName, "DumpOnError");
    if (mpRoot)
        mpRoot->DumpOnError(xmlDump);
}
void CXMLParseNode::DumpOnError(CXMLDump &xmlDump)
{
    CXMLDumpScope scope (msName, xmlDump);
    if (!msValue.IsEmpty())
        fprintf (xmlDump.mpf, (const wchar_t *)msValue);

    POSITION pos = mChildren.GetHeadPosition();
    while (pos)
    {
        mChildren.GetNext(pos)->DumpOnError (xmlDump);
    }
} */
bool CXMLParse::GetValueByKey(const wchar_t *zfName, int key, float &oValue)
{
	CXMLParse xmlParse(zfName);
	CXMLParseNode *pRoot = xmlParse.GetRoot();
	if (!pRoot)
		return false;

	return pRoot->GetValueByKey(key, oValue);
}
bool CXMLParseNode::GetValueByKey(int key, float &oValue)
{
	wchar_t zBuf[128];
	swprintf_s(zBuf, sizeof(zBuf)/sizeof(zBuf[0]),L"%d",key);
	CXMLParseNode *pKeyNode = GetFirst(zBuf);
	if (!pKeyNode)
	{
		pKeyNode = GetFirst(L"default");
		if (!pKeyNode)
			return false;
	}

	return pKeyNode->GetValue(oValue);
}
void CXMLParseNode::Add(CXMLParseNode *pNode)
{
	mChildren.AddTail(pNode);
}
CXMLParseNode * CXMLParseNode::Set(const wchar_t *zName, const wchar_t *zValue)
{
	CXMLParseNode *pNode = GetFirst(zName);
	if (pNode)
	{
		pNode->msValue = zValue;
	}
	else
	{
		pNode = new CXMLParseNode(zName, zValue);
		mChildren.AddTail(pNode);
	}
	return pNode;
}
CXMLParseNode * CXMLParseNode::Add(const wchar_t *zName, const wchar_t *zValue)
{
	CXMLParseNode *pNode = new CXMLParseNode(zName, zValue);
	mChildren.AddTail(pNode);
	return pNode;
}
