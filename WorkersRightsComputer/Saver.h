#pragma once
class CSaver
{
public:
	CSaver();
	~CSaver();

	static void ResetAllInputs(void);
	void Save(const wchar_t *zfName = NULL);
	void Restore(const wchar_t *zfName = NULL);

private:
	void SaveToXml(void);
	void SaveToTxtFile(void);
	void SaveEditBox(FILE *pfSave, class CEditRef *pRef);
	void SaveButton(FILE *pfSave, class CButtonRef *pRef);
	void WriteLetter(void);

	void LoadFromTxtFile(void);
	void LoadFromXmlFile(void);

	FILE *mpfWrite;
	CString msSaveId;
	CString msfName;
	FILE *mpfRead;
};

