#pragma once
#include "right.h"

class CAdditional : public CRight
{
public:
	CAdditional(void);
	virtual ~CAdditional(void);
	virtual	bool SetEditRef(class CEditRef *pRef) override;
	virtual bool Compute(void) override;
	virtual CString GetRightNameForLetter(void)override;

private:

	CEdit *mpDescBox;
	CEdit *mpDueBox;

};

