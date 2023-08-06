#include "stdafx.h"
#include "MaternityLeave.h"
#include "XMLDump.h"
#include "Right.h"

CMaternityLeave::CMaternityLeave(CMyTime firstDay, CMyTime lastDay,
	int nPaidWeeks, bool bPaidWeeksDeservePension)
	: CVacationUsed(firstDay, lastDay)
	, mnPaidWeeks(nPaidWeeks)
	, mbPaidWeeksDeservePension(bPaidWeeksDeservePension)
{
	mnWeeks = firstDay.GetNWeeksUntil(lastDay);
	mbIsMaternityLeave = true;
}

void CMaternityLeave::SaveToXml(class CXMLDump& xmlDump)
{
	CVacationUsed::SaveToXml(xmlDump);

	xmlDump.Write(L"b_maternity", mbIsMaternityLeave);
	xmlDump.Write(L"n_maternity_paid_weeks", mnPaidWeeks);
	xmlDump.Write(L"b_maternity_pension", mbPaidWeeksDeservePension);
}

CString CMaternityLeave::GetText()
{
	CString s(CVacationUsed::GetText());
	s += "Maternity: Paid ";
	s += CRight::ToString(mnPaidWeeks);
	s += " / ";
	s += CRight::ToString(mnWeeks);
	s += " weeks. ";
	if (mnPaidWeeks > 0)
	{
		if (mbPaidWeeksDeservePension)
			s += " + pension";
		else
			s += " no pension";
	}
	return s;
}
void CMaternityLeave::ShortLog(FILE* pf)
{
	CVacationUsed::ShortLog(pf, false);
	fprintf(pf, " Maternity Leave - %d / %.2f weeks paid, Pension: %s\n",
		mnPaidWeeks, mnWeeks, mbPaidWeeksDeservePension ? "Yes" : "No");
}
void CMaternityLeave::LongLog(FILE* pf)
{
	CVacationUsed::LongLog(pf);

}