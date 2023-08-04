#include "stdafx.h"
#include "MaternityLeave.h"
#include "XMLDump.h"
#include "Right.h"

CMaternityLeave::CMaternityLeave(CMyTime firstDay, CMyTime lastDay,
	double nPaidWeeks, bool bPaidWeeksDeservePension)
	: CVacationUsed(firstDay, lastDay)
	, mnPaidWeeks(nPaidWeeks)
	, mbPaidWeeksDeservePension(bPaidWeeksDeservePension)
{
	mbIsMaternityLeave = true;
}

void CMaternityLeave::SaveToXml(class CXMLDump& xmlDump)
{
	CVacationUsed::SaveToXml(xmlDump);

	xmlDump.Write(L"b_maternity", mbIsMaternityLeave);
	xmlDump.Write(L"b_maternity_paid_weeks", mnPaidWeeks);
	xmlDump.Write(L"b_maternity_pension", mbPaidWeeksDeservePension);
}

CString CMaternityLeave::GetText()
{
	CString s(CVacationUsed::GetText());
	s += "\r\nMaternity: Paid ";
	s += CRight::ToString(mnPaidWeeks);
	s += " weeks. ";
	if (mnPaidWeeks > 0)
	{
		if (mbPaidWeeksDeservePension)
			s += "Paid weeks desserve pension.";
		else
			s += "Paid weeks do not desserve pension.";
	}
	return s;
}
void CMaternityLeave::ShortLog(FILE* pf)
{
	CVacationUsed::ShortLog(pf, false);
	fprintf(pf, " Maternity Leave - %.2f weeks paid, Pension: %s\n",
		mnPaidWeeks, mbPaidWeeksDeservePension ? "Yes" : "No");
}
void CMaternityLeave::LongLog(FILE* pf)
{
	CVacationUsed::LongLog(pf);

}