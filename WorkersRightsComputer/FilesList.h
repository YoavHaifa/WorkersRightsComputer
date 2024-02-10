#pragma once
#include "afxtempl.h"

class CFilesList
{
public:
	CFilesList(const wchar_t *zfName = NULL);
	~CFilesList(void);

	void Add(const wchar_t *zfName);
	int AddNamesFromFile(const wchar_t *zfName);
	void AddWithFactor(const wchar_t *zfName, double factor);
	int N(void) const;
	int Invert(const CFilesList &other);
	bool Save(const wchar_t *zListName = NULL);
	bool Init(const wchar_t *zListName = NULL, int nVerify = 0);
	const wchar_t *Name(void){return (const wchar_t *)msListName;}

	POSITION GetHeadPosition(void)const {return mList.GetHeadPosition();}
	CString *GetNext(POSITION &pos)const {return mList.GetNext(pos);}
	CString GetHead(void)const;
	CString GetAt(int i)const;

	bool IsEmpty(void)const {return mList.IsEmpty() == TRUE;}
	POSITION GetFactorsHeadPosition(void){return mFactorsList.GetHeadPosition();}
	double GetNextFactor(POSITION &pos){return mFactorsList.GetNext(pos);}
	bool DeleteFileName(POSITION posToRemove);
	void PrintWithValues(FILE *pf, const wchar_t *zTitle = NULL, bool bIndex = true);
	void Print(FILE *pf, const wchar_t *zTitle = NULL, bool bIndex = true, bool bValues = false);
	bool Print(const wchar_t *zfName, const wchar_t *zTitle = NULL, bool bIndex = true);
	void ConvertToPrivateWithoutExtension(void);
	void ComputePrivatePrefixNumber(void);
	bool Includes(const wchar_t *zfName);
	void ClearAll(void);
	int PurgeNonDir(void);
	CString GetBiggest(void);

	static void SetDefaultSaveDir(const wchar_t *zDir);
	static void SetValuesPrintFormat(const wchar_t *zFormat);
	void DeleteAllFilesAndClearList(FILE *pfLog = NULL);

	bool Find(const wchar_t*zwText, CString &osfName);

private:
	CString msListName;
	static CString umsDefaultSaveDir;
    CList<CString*,CString*> mList;
    CList<double,double> mFactorsList;
	void InitOnConstruct(void);
	static CString umsValuesFormat;
};
