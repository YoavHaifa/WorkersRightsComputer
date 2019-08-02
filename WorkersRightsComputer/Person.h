#pragma once

class CPerson
{
public:
	CPerson(void);

	CString GetFirstName(void);
	CString GetLastName(void);
	CString GetPassport(void);
	CString GetTel(void);

	void StartLetter(class CLogoWriter &lw);
	void StartLetter(class CHtmlWriter &html);
	void SetDlg(class CMyDialogEx *pDlg, int iFirstName, int iFamilyName, int iId, int iTel, int iAdd, int iEmail, int iRole = 0);
	int UpdateFromDlg();
	void UpdateDlg();
	void SaveToTxtFile();
	void LoadFromTxtFile();

	CString msPrivateName;
	CString msFamilyName;
	CString msPassport;
	CString msTelephone;
	CString msAddress;
	CString msEmail;
	CString msRole;
	bool mbIsPassport; // Otherwise ID
	class CMyDialogEx *mpDlg;
	int miFirstName;
	int miFamilyName;
	int miId;
	int miAdd;
	int miTel;
	int miEmail;
	int miRole;
};

extern CPerson gWorker;
extern CPerson gEmployer;
extern CPerson gEmployer2;
