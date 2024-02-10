#include "stdafx.h"
#include "Comments.h"
#include "MyDialogEx.h"
#include "resource.h"
#include "XMLDump.h"
#include "XMLParse.h"
#include "CommentsDlg.h"
#include "WorkersRightsComputerDlg.h"

CComments gComments;

CComments::CComments()
	: mbDisplayOnLoad(true)
{
	maEditId[0] = IDC_EDIT_COMMENT1;
	maEditId[1] = IDC_EDIT_COMMENT2;
	maEditId[2] = IDC_EDIT_COMMENT3;
}
void CComments::Clear()
{
	while (!mList.IsEmpty())
	{
		delete mList.GetTail();
		mList.RemoveTail();
	}
	UpdateMainDialog();
}
void CComments::UpdateGUI(CMyDialogEx* pDlg)
{
	int i = 0;
	POSITION pos = mList.GetHeadPosition();
	while (pos)
	{
		CString* ps = mList.GetNext(pos);
		pDlg->SetText(maEditId[i], *ps);
		i++;
	}

	pDlg->SetCheck(IDC_CHECK_DISPLAY_COMMENTS, mbDisplayOnLoad);
}
void CComments::UpdateFromGUI(CMyDialogEx* pDlg)
{
	Clear();

	for (int i = 0; i < MAX_COMMENTS; i++)
	{
		CString s(pDlg->GetText(maEditId[i]));
		if (!s.IsEmpty())
			mList.AddTail(new CString(s));
	}

	mbDisplayOnLoad = pDlg->IsChecked(IDC_CHECK_DISPLAY_COMMENTS);
	UpdateMainDialog();
}
void CComments::SaveToXml(CXMLDump& xmlDump)
{
	if (mList.IsEmpty())
		return;

	CXMLDumpScope mainScope(L"Comments", xmlDump);

	POSITION pos = mList.GetHeadPosition();
	while (pos)
	{
		CString* ps = mList.GetNext(pos);
		xmlDump.Write(L"Comment", *ps);
	}

	xmlDump.Write(L"b_display_on_load", mbDisplayOnLoad);
}
void CComments::LoadFromXml(CXMLParseNode* pMain)
{
	Clear();

	CXMLParseNode* pCommentsNode = pMain->GetFirst(L"Comments");
	if (!pCommentsNode)
		return;

	CXMLParseNode* pComNode = pCommentsNode->GetFirst(L"Comment");
	while (pComNode)
	{
		CString s;
		pComNode->GetValue(s);
		if (!s.IsEmpty())
			mList.AddTail(new CString(s));
		pComNode = pCommentsNode->GetNext(L"Comment", pComNode);
	}

	pCommentsNode->GetValue(L"b_display_on_load", mbDisplayOnLoad);
}
void CComments::OnLoad()
{
	if (!mList.IsEmpty() && mbDisplayOnLoad)
	{
		CCommentsDlg dlg;
		dlg.DoModal();
	}
	UpdateMainDialog();
}
void CComments::UpdateMainDialog()
{
	if (!gpDlg)
		return;

	if (mList.IsEmpty())
		gpDlg->SetText(IDC_COMMENTS, L"Comments");
	else
	{
		int n = (int)mList.GetSize();
		if (n == 1)
			gpDlg->SetText(IDC_COMMENTS, L"1 Comment *");
		else if (n == 2)
			gpDlg->SetText(IDC_COMMENTS, L"2 Comments **");
		else
			gpDlg->SetText(IDC_COMMENTS, L"3 Comments ***");
	}
}
