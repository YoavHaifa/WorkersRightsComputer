#pragma once
class CSaver
{
public:
	CSaver();
	~CSaver();

	static void ResetAllInputs(bool bLoading);
	bool Save(const wchar_t *zfName = NULL);
	bool Restore(const wchar_t *zfName = NULL);

private:
	void SaveToXml(void);
	//void SaveToTxtFile(void);
	void SaveEditBox(FILE *pfSave, class CEditRef *pRef);
	void SaveButton(FILE *pfSave, class CButtonRef *pRef);
	void WriteLetter(void);

	bool LoadFromXmlFile(void);

	FILE *mpfWrite;
	CString msSaveId;
	CString msfName;
	FILE *mpfRead;
};

