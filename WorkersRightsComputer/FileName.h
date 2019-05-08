#pragma once

class CFileName
{
public:
    CFileName(const wchar_t *zName);
    CFileName(const wchar_t *zPath, const wchar_t *zName);
    CFileName(const wchar_t *zPath, const wchar_t *zName, const wchar_t *zExt);
    CFileName(void);
    ~CFileName(void);

    const wchar_t * Name(void) const;
    const wchar_t * Type(void) const;
    const wchar_t * Private(void) const;
    CString PrivateWithoutType(void) const;
    CString PrivateBase(void); // private until first '.' or '_'
    CString NameWithoutType(void);
    CString NameWithoutTypeAndMetaType(void);
    void AddMetaTypeAndType(const wchar_t *zMetaType, const wchar_t *zType);

    CString Path(bool bCurrentDirIfNone = false);
    CString PathDir(void);
	static CString GetPath(const wchar_t *zfName);

    void Set(const wchar_t *zName);
    void AddType(const wchar_t *zNewType);
    void ChangeType(const wchar_t *zNewType);
    void ChangePath(const wchar_t *zNewPath);
    bool ChangeDrive(wchar_t drive);
	bool ChangeToVerifiedSubDir(const wchar_t *zSubDirName);
	bool ChangeToSubDirIfExists(const wchar_t *zSubDirName);
    static void MakeLegalName(wchar_t * zName);
    static void MakeLegalName(CString &sName);
    static void MakeLegalShortName(CString &sName);

    bool IsOfType(const wchar_t *zType);
    static bool StaticIsOfType(const wchar_t *zName, const wchar_t *zType);
    bool IsOfMetaType(const wchar_t *z);
    CString GetMetaType(void);
    bool HasExtension (void) {return miExtension != 0;}
    static bool HasExtension (const wchar_t *zfName);
    bool IsTyped (void) {return miExtension != 0 && isalpha(msName[miExtension+1]);}
    bool HasFullPath (void);


    FILE *Open (const wchar_t *zMode);
    FILE *OpenWithErrorBox (const wchar_t *zMode, const wchar_t *zDesc);
    static bool Exist (const wchar_t *zfName);
    static bool ExistNonEmpty (const wchar_t *zfName);
    static FILETIME GetModificationTime (const wchar_t *zfName);

    size_t Length (void) const;
    size_t TypeLength (void) const;
	ULONG64 FileLength(void);
    static ULONG64 GetFileLength(const wchar_t * zfName);

    bool IsImageFromHostArchives(void);
    CString msName;

    operator const wchar_t * (void) {return (const wchar_t *)msName;}
	bool operator == (const CFileName &other) {return msName == other.msName;}
	bool operator != (const CFileName &other) {return msName != other.msName;}

    int miExtension;

private:
    int miPrivate;

public:
	static void SetDataPath(const wchar_t *zPath);
	static void SetMduPath(const wchar_t *zPath);
    void PrefixPath(const wchar_t * zPrefix);
    CString FirstName(void);
    static CString GetPrivate(const wchar_t * zName);
    static CString GetPrivateWithoutType(const wchar_t * zName);
    static CString GetDirOrPath(const wchar_t * zName);
    static CString GetLastDirName(const wchar_t * zName);
    CString GetLastDirName(void);
    bool Exist(void);
	CString GetIndexedName(int index, bool bKeepSecondExtension = false);
	static CString GetBaseOfPattern(const wchar_t * zPattern);
	static bool CreateRelativePath(const wchar_t * zBase, const wchar_t * zSrcPath, CString &osPath);
	static CString GetLastInPath(const wchar_t *zPath);
	static CString BuildFileName(const wchar_t *zPath, const wchar_t *zName, const wchar_t *zExtension = NULL);
	static CString BuildDirNearFile(const wchar_t *zFileName, const wchar_t *zNewDirName);
	static CString ReplaceTopDirectoryName(const wchar_t * zFileName, const wchar_t * zNewDir);
	static bool IsSubDir(const wchar_t * zPath, const wchar_t * zRoot);
	static bool GetNewFileName(const wchar_t *zName, int nTrials, CString &sNewName);
	//static bool LookForVariantOf(CString &sioName);
	static CString GetVariantPattern(const wchar_t *zName);
	//static void TestGetVariantPattern(void);
	//static void TestGetVariantPattern1(const wchar_t *zfName, class CLogFile &log);
	static bool FindValidPath(CString &sioPath);
	static CString CreateVariantOfPrivateName(const wchar_t *zfName, const wchar_t *zVar);
	static CString CreateVariantOfFullName(const wchar_t *zfName, const wchar_t *zVar);
	static CString GetNameValue(const wchar_t *zProperty, int value);
	static CString AddIndexToPivateName(const wchar_t *zfName, int i);
};
