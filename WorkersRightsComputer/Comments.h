#pragma once
#include "afxtempl.h"

class CComments
{
public:
	CComments();

	void UpdateGUI(class CMyDialogEx *pDlg);
	void UpdateFromGUI(CMyDialogEx *pDlg);
	void Clear();
	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);
	void OnLoad();
	void UpdateMainDialog();

private:
	static const int MAX_COMMENTS = 3;

	int maEditId[MAX_COMMENTS];

	CList<CString*, CString*> mList;
	bool mbDisplayOnLoad;
};

extern CComments gComments;