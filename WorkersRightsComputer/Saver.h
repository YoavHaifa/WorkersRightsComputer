#pragma once
class CSaver
{
public:
	CSaver();
	~CSaver();

	void Save(const wchar_t *zfName = NULL);
	void Restore(const wchar_t *zfName = NULL);

	void SaveToFile(void);
	void SaveEditBox(FILE *pfSave, class CEditRef *pRef);
	void SaveButton(FILE *pfSave, class CButtonRef *pRef);
	void WriteLetter(void);
	void LoadFromFile(void);

	FILE *mpfWrite;
	CString msfName;
	FILE *mpfRead;
};

