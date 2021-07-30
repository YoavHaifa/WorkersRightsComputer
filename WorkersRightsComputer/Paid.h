#pragma once
#include "Right.h"

class CPaid : public CRight
{
public:
	CPaid(void);
	virtual ~CPaid(void);
	virtual	bool SetEditRef(class CEditRef* pRef) override;
	virtual bool Compute(void) override;
	//virtual CString GetDecriptionForLetter(void)override;
	//virtual CString GetDecriptionForLetterHebrew(void)override;
	virtual bool HasLegalValue();

private:

	CEdit* mpDescBox;
	CEdit* mpDueBox;
};

