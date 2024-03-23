#include "stdafx.h"
#include "StringsList.h"

void CStringsList::AddTail(const CString& s)
{
	mList.AddTail(new CString(s));
}
void CStringsList::AddTail(const char* zText)
{
	mList.AddTail(new CString(zText));
}
bool CStringsList::AddIfNew(const char* zText)
{
	if (!mList.IsEmpty())
	{
		CString* ps = mList.GetTail();
		if (*ps == zText)
			return false;
	}

	AddTail(zText);
	return true;
}
void CStringsList::Clear()
{
	while (!mList.IsEmpty())
	{
		delete mList.GetTail();
		mList.RemoveTail();
	}
}
