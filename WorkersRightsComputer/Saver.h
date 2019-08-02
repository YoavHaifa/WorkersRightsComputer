#pragma once
class CSaver
{
public:
	CSaver();
	~CSaver();

	void Save(const wchar_t *zfName = NULL);
	void Restore(const wchar_t *zfName = NULL);

	void SaveToXml(void);
	void SaveToTxtFile(void);
	void SaveEditBox(FILE *pfSave, class CEditRef *pRef);
	void SaveButton(FILE *pfSave, class CButtonRef *pRef);
	void WriteLetter(void);
	void LoadFromTxtFile(void);
	void LoadFromXmlFile(void);

	FILE *mpfWrite;
	CString msfName;
	FILE *mpfRead;
};

