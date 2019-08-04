#pragma once

class CPerson
{
public:
	CPerson(void);

	CString GetFirstName(void);
	CString GetLastName(void);
	CString GetPassport(void);
	CString GetTel(void);

	//void StartLetter(class CLogoWriter &lw);
	void StartLetter(class CHtmlWriter &html);
	void SetDlg(class CMyDialogEx *pDlg, int iFirstName, int iFamilyName, int iId, int iTel, int iAdd, int iEmail, int iRole = 0);
	int UpdateFromDlg();
	void UpdateDlg();

	CString msPrivateName;
	CString msFamilyName;
	CString msPassport;
	CString msTelephone;
	CString msAddress;
	CString msEmail;
	CString msRole;
	CString msComment;
	bool mbIsPassport; // Otherwise ID
	class CMyDialogEx *mpDlg;
	int miFirstName;
	int miFamilyName;
	int miId;
	int miAdd;
	int miTel;
	int miEmail;
	int miRole;
	bool mbEmployer;
	CString GetDescription();
	static void ClearContacts(void);
	static void SaveContactsToXml(class CXMLDump& xmlDump);
	void SaveToXml(class CXMLDump& xmlDump);
	static void LoadContactsFromXml(class CXMLParseNode* pRoot);
	void LoadFromXml(class CXMLParseNode* pRoot);
};

extern CPerson gWorker;
extern CList<CPerson*, CPerson*> gContacts;