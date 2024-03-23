#pragma once

class CStringsList
{
public:
	void AddTail(const CString& s);
	void AddTail(const char* zText);
	bool AddIfNew(const char* zText);
	void Clear();
	bool IsEmpty() { return mList.IsEmpty(); }
	POSITION GetHeadPosition() { return mList.GetHeadPosition(); }
	CString* GetNext(POSITION& pos) { return mList.GetNext(pos); }

	CList<CString*, CString*> mList;
};

