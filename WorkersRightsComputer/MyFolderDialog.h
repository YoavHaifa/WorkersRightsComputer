#pragma once

class CMyFolderDialog 
	: public CFolderPickerDialog
{
public:
	CMyFolderDialog(const char* zTitle, const char* zPath = NULL, bool bPreferNewPath = false);
	virtual INT_PTR DoModal(); // Returns TRUE if Selection Done
	CString msFolderName;

private:
	CString msTitle;
	CString msInitialPath;
	bool mbPreferNewPath;
};

