#pragma once

class  CLogoWriter
{
public:
	CLogoWriter(const wchar_t *zfName);
	~CLogoWriter(void);

	void WriteLine(const wchar_t *zText);
	void Close(void);

private:
	bool ReadLinePattern(void);
	CString msLinePattern;
	bool CopyFile(const wchar_t *zfName);
	FILE *mpf;
	bool mbOpen;
};

