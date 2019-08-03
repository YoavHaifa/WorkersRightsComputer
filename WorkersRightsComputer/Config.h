#pragma once

class CConfig
{
public:
	CConfig(void);
	void InitFromXml();
	CString msVersion;
	CString msSaveRoot;
};

extern CConfig gConfig;