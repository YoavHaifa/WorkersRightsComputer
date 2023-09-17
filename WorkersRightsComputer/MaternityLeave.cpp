#include "stdafx.h"
#include "MaternityLeave.h"
#include "XMLDump.h"
#include "Right.h"
#include "WorkSpan.h"

CMaternityLeave::CMaternityLeave(CMyTime firstDay, CMyTime lastDay,
	int nPaidWeeks, bool bPaidWeeksDeservePension)
	: CVacationUsed(firstDay, lastDay)
	, mnPaidWeeks(nPaidWeeks)
	, mbPaidWeeksDeservePension(bPaidWeeksDeservePension)
{
	mnWeeks = firstDay.GetNWeeksUntil(lastDay);
	mbIsMaternityLeave = true;
	Compute();
}
void CMaternityLeave::Compute()
{
	mnPaidDays = min((int)(mnPaidWeeks * gWorkPeriod.mnWorkDaysPerWeek), mnWorkDays);
	mnUnPaidWorkDays = mnWorkDays - mnPaidDays;
	if (mnPaidDays < mnWorkDays)
	{
		FindUnpaidSpan();
		mPaidSpan.InitSpan(mFirstDay, mUnpaidSpan.mFirstDay.PrevDay());
	}
	else
		mPaidSpan.InitSpan(mFirstDay, mLastDay);
}
void CMaternityLeave::AddToWorkSpan(CWorkSpan& workSpan)
{
	CVacationUsed::AddToWorkSpan(workSpan);
	if (workSpan.Overlaps(mPaidSpan))
		workSpan.AddPaidMaternityLeave(mPaidSpan);
}
void CMaternityLeave::SaveToXml(CXMLDump& xmlDump)
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
	fprintf(pf, " Maternity Leave - %d / %.2f weeks paid, Pension: %s\n", 
		mnPaidWeeks, mnWeeks, mbPaidWeeksDeservePension ? "Yes" : "No");
	CVacationUsed::LongLog(pf);
	mPaidSpan.Log(pf, "Paid");
	fprintf(pf, "\n");
}