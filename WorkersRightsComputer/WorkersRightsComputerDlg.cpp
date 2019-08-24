
// WorkersRightsComputerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "WorkersRightsComputerDlg.h"
#include "afxdialogex.h"
#include "WorkPeriodDlg.h"
#include "AllRights.h"
#include "Utils.h"
#include "FilesList.h"
#include "MyFileDialog.h"
#include "Verify.h"
#include "Hebrew.h"
#include "HtmlWriter.h"
#include "EmployerDlg.h"
#include "Person.h"
#include "Saver.h"
#include "MyAskDlg.h"
#include "XmlDump.h"
#include "XmlParse.h"
#include "HtmlWriter.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWorkersRightsComputerDlg *gpDlg = NULL;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWorkersRightsComputerDlg dialog



CWorkersRightsComputerDlg::CWorkersRightsComputerDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_WORKERSRIGHTSCOMPUTER_DIALOG, pParent)
	, mbInitialized(false)
	, mbDisableComputations(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Worker's personal details
	gWorker.SetDlg(this, IDC_EDIT_FIRST_NAME, IDC_EDIT_FAMILY_NAME, IDC_EDIT_ID, IDC_EDIT_TEL, IDC_EDIT_ADDRESS, IDC_EDIT_EMAIL);
	mEditBoxes.AddTail(new CEditRef(L"FirstName", mEditFirstName, L"textBox1", false));
	mEditBoxes.AddTail(new CEditRef(L"FamilyName", mEditFamilyName, L"textBox2", false));
	mEditBoxes.AddTail(new CEditRef(L"ID", mEditID, L"textBox12", false));
	mEditBoxes.AddTail(new CEditRef(L"Tel", mEditTel, L"textBox16", false));
	mEditBoxes.AddTail(new CEditRef(L"Address", mAddress, NULL, false));
	mEditBoxes.AddTail(new CEditRef(L"Email", mEmail, NULL, false));
	mEditBoxes.AddTail(new CEditRef(L"FilledBy", mFilledBy, NULL, false));

	// Employment details
	mEditBoxes.AddTail(new CEditRef(L"LastYearWork", mEditLastYearWork, L"textBox9"));
	mEditBoxes.AddTail(new CEditRef(L"LastYearPaid", mEditLastYearPaid, L"textBox3"));
	mEditBoxes.AddTail(new CEditRef(L"LastYearFrom", mEditLastYearFrom, L"textBox4"));
	mEditBoxes.AddTail(new CEditRef(L"PrevYearWork", mEditPrevYearWork, L"textBox10"));
	mEditBoxes.AddTail(new CEditRef(L"PrevYearPaid", mEditPrevYearPaid, L"textBox5"));
	mEditBoxes.AddTail(new CEditRef(L"PrevYearFrom", mEditPrevYearFrom, L"textBox6"));
	mEditBoxes.AddTail(new CEditRef(L"PrevNYear", mEditPrevNYears, L"textBox7"));
	mEditBoxes.AddTail(new CEditRef(L"AdditionalDesc", mEditAdditionalDesc, L"textBox14"));
	mEditBoxes.AddTail(new CEditRef(L"AdditionalSum", mEditAdditionalSum, L"textBox15"));
	mEditBoxes.AddTail(new CEditRef(L"VacationPrevYears", mEditVacationPrevYears, L"textBox11"));
	mEditBoxes.AddTail(new CEditRef(L"RecuperationPrevYears", mEditRecuperationPrevYears, L"textBox13"));

	mEditBoxes.AddTail(new CEditRef(L"PayRatePerHoliday", mEditPayPerEachHolyDay));


	mButtons.AddTail(new CButtonRef(L"AllowSevLess", mAllowSevLess, L"checkBox1"));
	mButtons.AddTail(new CButtonRef(L"DemandVac4Prev", mDemandVac4Prev, L"checkBox2"));
	mButtons.AddTail(new CButtonRef(L"DemandRec4Prev", mDemandRec4Prev, L"checkBox3"));
	mButtons.AddTail(new CButtonRef(L"EntitledOnly2Sev", mEntitledOnly2Sev, L"checkBox11"));
	mButtons.AddTail(new CButtonRef(L"HadPensionBefore", mHadPensionBefore, L"checkBox12"));

	mButtons.AddTail(new CButtonRef(L"RadioID", mRadiID, NULL, false));
	mButtons.AddTail(new CButtonRef(L"RadioPass", mRadioPassport, NULL, false));
}

void CWorkersRightsComputerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_LASTY_WORK, mEditLastYearWork);
	DDX_Control(pDX, IDC_EDIT_FIRST_NAME, mEditFirstName);
	DDX_Control(pDX, IDC_EDIT_FAMILY_NAME, mEditFamilyName);
	DDX_Control(pDX, IDC_EDIT_ID, mEditID);
	DDX_Control(pDX, IDC_EDIT_TEL, mEditTel);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_LASTY_PAID, mEditLastYearPaid);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_LASTY_FROM, mEditLastYearFrom);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_WORK, mEditPrevYearWork);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_PAID, mEditPrevYearPaid);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_FROM, mEditPrevYearFrom);
	DDX_Control(pDX, IDC_EDIT_ADDITIONAL_DESC, mEditAdditionalDesc);
	DDX_Control(pDX, IDC_EDIT_ADDITIONAL_SUM, mEditAdditionalSum);
	DDX_Control(pDX, IDC_EDIT_VACATION_YEARS, mEditVacationPrevYears);
	DDX_Control(pDX, IDC_EDIT_RECUPERATION_YEARS, mEditRecuperationPrevYears);
	DDX_Control(pDX, IDC_COMBO_HOLIDAYS, mComboHolidays);
	DDX_Control(pDX, IDC_CHECK_SEVERANCE_LESS_THAN_YEAR, mAllowSevLess);
	DDX_Control(pDX, IDC_CHECK_VACATION_YEARS, mDemandVac4Prev);
	DDX_Control(pDX, IDC_CHECK_RECUPERATION_YEARS, mDemandRec4Prev);
	DDX_Control(pDX, IDC_CHECK_ONLY_SEVERANCE, mEntitledOnly2Sev);
	DDX_Control(pDX, IDC_CHECK_ACTIVE_PENSION, mHadPensionBefore);
	DDX_Control(pDX, IDC_EDIT_SUMMARY, mSummary);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_N_YEARS, mEditPrevNYears);
	DDX_Control(pDX, IDC_RADIO_ID, mRadiID);
	DDX_Control(pDX, IDC_RADIO_PASSPORT, mRadioPassport);
	DDX_Control(pDX, IDC_EDIT_FILLED_BY, mFilledBy);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, mAddress);
	DDX_Control(pDX, IDC_EDIT_EMAIL, mEmail);
	DDX_Control(pDX, IDC_EDIT_PAY_PER_HOLY_DAY, mEditPayPerEachHolyDay);
}

BEGIN_MESSAGE_MAP(CWorkersRightsComputerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_EXIT, &CWorkersRightsComputerDlg::OnFileExit)
	ON_BN_CLICKED(IDC_WORK_PERIOD, &CWorkersRightsComputerDlg::OnBnClickedWorkPeriod)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CWorkersRightsComputerDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CWorkersRightsComputerDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CWorkersRightsComputerDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_CHECK_SEVERANCE_LESS_THAN_YEAR, &CWorkersRightsComputerDlg::OnBnClickedCheckSeveranceLessThanYear)
	ON_BN_CLICKED(IDC_CHECK_VACATION_YEARS, &CWorkersRightsComputerDlg::OnBnClickedCheckVacationYears)
	ON_BN_CLICKED(IDC_CHECK_RECUPERATION_YEARS, &CWorkersRightsComputerDlg::OnBnClickedCheckRecuperationYears)
	ON_BN_CLICKED(IDC_CHECK_ONLY_SEVERANCE, &CWorkersRightsComputerDlg::OnBnClickedCheckOnlySeverance)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE_PENSION, &CWorkersRightsComputerDlg::OnBnClickedCheckActivePension)
	ON_CBN_SELCHANGE(IDC_COMBO_HOLIDAYS, &CWorkersRightsComputerDlg::OnCbnSelchangeComboHolidays)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_LASTY_WORK, &CWorkersRightsComputerDlg::OnEnChangeEditHolidaysLastyWork)
	ON_COMMAND(ID_COMPUTE_ALL, &CWorkersRightsComputerDlg::OnComputeAll)
	ON_COMMAND(ID_FILE_SAVEAS, &CWorkersRightsComputerDlg::OnFileSaveas)
	ON_COMMAND(ID_FILE_LOAD, &CWorkersRightsComputerDlg::OnFileLoad)
	ON_COMMAND(ID_TEST_CREATEDIR, &CWorkersRightsComputerDlg::OnTestCreatedir)
	ON_COMMAND(ID_FILE_LOADOLDCASE, &CWorkersRightsComputerDlg::OnFileLoadoldcase)
	ON_COMMAND(ID_TEST_VERIFYBATCH, &CWorkersRightsComputerDlg::OnTestVerifybatch)
	ON_COMMAND(ID_TEST_WRITEHTML, &CWorkersRightsComputerDlg::OnTestWritehtml)
	ON_BN_CLICKED(IDC_EMPLOYER, &CWorkersRightsComputerDlg::OnBnClickedEmployer)
	ON_BN_CLICKED(IDC_COMMENTS, &CWorkersRightsComputerDlg::OnBnClickedComments)
	ON_COMMAND(ID_TEST_LOADXML, &CWorkersRightsComputerDlg::OnTestLoadxml)
	ON_COMMAND(ID_TEST_LOADTXT, &CWorkersRightsComputerDlg::OnTestLoadtxt)
END_MESSAGE_MAP()


// CWorkersRightsComputerDlg message handlers

BOOL CWorkersRightsComputerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	mbDisableComputations = true;
	OnBnClickedButtonReset();
	InitHolidaysCombo();
	InitializeAllRights();
	mRadioPassport.SetCheck(1);
	gConfig.InitFromXml();
	CString sTitle(L"Workers Rights Computer - Experimental Beta Version ");
	SetTitle(sTitle + gConfig.msVersion);

	CUtils::CreateThread(&StaticThreadFunc, NULL);

	gpDlg = this;
	mbInitialized = true;
	mbDisableComputations = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWorkersRightsComputerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWorkersRightsComputerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWorkersRightsComputerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWorkersRightsComputerDlg::OnOK()
{
	CMyAskDlg dlg(L"Verify OK", L"Do you want save and to exit?");
	if (dlg.Ask())
	{
		OnBnClickedButtonSave();
		CDialogEx::OnOK();
	}
}

void CWorkersRightsComputerDlg::OnCancel()
{
	CMyAskDlg dlg(L"Verify Cancel", L"Do you want to exit without saving?");
	if (dlg.Ask())
		CDialogEx::OnCancel();
}

void CWorkersRightsComputerDlg::OnFileExit()
{
	exit(0);
}


void CWorkersRightsComputerDlg::OnBnClickedWorkPeriod()
{
	CWorkPeriodDlg dlg;
	if (dlg.DoModal() == IDOK)
		OnInputChange();
}


void CWorkersRightsComputerDlg::OnBnClickedButtonSave()
{
	CSaver saver;
	saver.Save();
}
void CWorkersRightsComputerDlg::OnBnClickedButtonLoad()
{
	CSaver saver;
	saver.Restore();
}
void CWorkersRightsComputerDlg::OnBnClickedButtonReset()
{
	CSaver::ResetAllInputs();
	OnInputChange();
}
void CWorkersRightsComputerDlg::ResetAllInputs(void)
{
	POSITION pos = mEditBoxes.GetHeadPosition();
	while (pos)
	{
		CEditRef* pEdit = mEditBoxes.GetNext(pos);
		pEdit->mEdit.SetWindowTextW(L"0");
	}

	pos = mButtons.GetHeadPosition();
	while (pos)
	{
		CButtonRef* pButton = mButtons.GetNext(pos);
		pButton->mButton.SetCheck(BST_UNCHECKED);
	}
	mComboHolidays.SetWindowTextW(L"Select set of Holidays");

}
void CWorkersRightsComputerDlg::InitHolidaysCombo()
{
	CString msDir(CUtils::GetBaseDir() + L"input\\holidays");
	CFilesList list;
	CUtils::ListFilesInDir(msDir, L"txt", list);
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CString *psName = list.GetNext(pos);
		CFileName fName(*psName);
		CString sPrivate = fName.PrivateWithoutType();
		if (sPrivate.Right(8) == "Holidays")
			sPrivate = sPrivate.Left(sPrivate.GetLength() - 8);
		mComboHolidays.AddString(sPrivate);
	}
}
void CWorkersRightsComputerDlg::OnBnClickedCheckSeveranceLessThanYear()
{
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnBnClickedCheckVacationYears()
{
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnBnClickedCheckRecuperationYears()
{
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnBnClickedCheckOnlySeverance()
{
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnBnClickedCheckActivePension()
{
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnCbnSelchangeComboHolidays()
{
	CString s;
	mComboHolidays.GetWindowTextW(s);
	CString s1 = GetText(IDC_COMBO_HOLIDAYS);
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnEnChangeEditHolidaysLastyWork()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	OnInputChange();
}


void CWorkersRightsComputerDlg::InitializeAllRights()
{
	gAllRights.Init();

	POSITION pos = mEditBoxes.GetHeadPosition();
	while (pos)
	{
		CEditRef *pRef = mEditBoxes.GetNext(pos);
		if (pRef->mbConnect)
			gAllRights.SetEditRef(pRef);
	}

	pos = mButtons.GetHeadPosition();
	while (pos)
	{
		CButtonRef *pRef = mButtons.GetNext(pos);
		if (pRef->mbConnect)
			gAllRights.SetCheckRef(pRef);
	}
}
CString CWorkersRightsComputerDlg::GetHolidaysSet()
{
	CString sSelection;
	mComboHolidays.GetWindowTextW(sSelection);
	return sSelection;
}
void CWorkersRightsComputerDlg::DisplaySummary(const wchar_t *zText)
{
	if (mbInitialized)
		mSummary.SetWindowTextW(zText);
}
void CWorkersRightsComputerDlg::OnComputeAll()
{
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnFileSaveas()
{
	CString sDir(L"C:\\WorkersRights\\Save");
	CMyFileDialog dlg(CMyFileDialog::FD_SAVE, L"save file", sDir);
	dlg.SetDefaultExtention(L"txt");
	if (dlg.DoModal())
	{
		CString sfName(dlg.mSelectedFileName);
		CSaver saver;
		saver.Save(sfName);
	}
}
void CWorkersRightsComputerDlg::OnFileLoad()
{
	CString sDir(L"C:\\WorkersRights\\Save");
	CMyFileDialog dlg(CMyFileDialog::FD_OPEN, L"saved file", sDir);
	dlg.SetDefaultExtention(L"txt");
	if (dlg.DoModal())
	{
		CString sfName(dlg.mSelectedFileName);
		CSaver saver;
		saver.Restore(sfName);
	}
}
void CWorkersRightsComputerDlg::OnTestCreatedir()
{
	CUtils::VerifyDirectory(L"c:\\tmp\\new_dir");
}
void CWorkersRightsComputerDlg::OnFileLoadoldcase()
{
	CString sDir(L"C:\\WorkersRights\\New_examples\\Ioana_Raducanu_055746047");
	CMyFileDialog dlg(CMyFileDialog::FD_OPEN, L"saved file", sDir);
	dlg.SetDefaultExtention(L"txt");
	if (dlg.DoModal())
	{
		CString sfName(dlg.mSelectedFileName);
		CVerify verify(sfName);
		verify.Verify();
	}
}
void CWorkersRightsComputerDlg::OnInputChange()
{
	if (mbDisableComputations)
		return;

	gAllRights.Compute();
}
DWORD WINAPI CWorkersRightsComputerDlg::StaticThreadFunc(LPVOID)
{
	static int count = 0;
	count++;
	while (1)
	{
		Sleep(100);
		CString s = gpDlg->GetHolidaysSet();
		if (!s.IsEmpty() && s != "Select set of Holidays")
			if (s != gAllRights.GetHolidaysSelection())
				gpDlg->OnInputChange();
	}
	return 0;
}
void CWorkersRightsComputerDlg::OnTestVerifybatch()
{
	CString sDir(L"C:\\WorkersRights\\New_examples\\Beena MOl_Thomas_M7281984");
	CMyFileDialog dlg(CMyFileDialog::FD_OPEN, L"saved file", sDir);
	dlg.SetDefaultExtention(L"txt");
	if (dlg.DoModal())
	{
		CString sfName(dlg.mSelectedFileName);
		CVerify::VerifyBatch(sfName);
	}
}
void CWorkersRightsComputerDlg::OnTestWritehtml()
{
	CMyFileDialog dlg(CMyFileDialog::FD_SAVE, L"Save Letter");
	if (dlg.DoModal())
	{
		CHtmlWriter writer;
		writer.WriteLetterFromTemplate(dlg.GetPathName());
	}
}
void CWorkersRightsComputerDlg::OnBnClickedEmployer()
{
	CEmployerDlg dlg;
	dlg.DoModal();
}
void CWorkersRightsComputerDlg::OnBnClickedComments()
{
	// TODO: Add your control notification handler code here
}
void CWorkersRightsComputerDlg::SaveToXml(CXMLDump &xmlDump)
{
	CXMLDumpScope mainScope(L"MainDialog", xmlDump);

	{
		CXMLDumpScope scope(L"EditBoxes", xmlDump);
		POSITION pos = mEditBoxes.GetHeadPosition();
		while (pos)
		{
			CString sText;
			CEditRef *pRef = mEditBoxes.GetNext(pos);
			pRef->mEdit.GetWindowText(sText);
			if (sText.IsEmpty())
				sText = L"0";
			xmlDump.Write((const wchar_t *)pRef->msName, (const wchar_t *)sText);
		}
	}
	// Holidays
	CString sText;
	mComboHolidays.GetWindowText(sText);
	if (sText.IsEmpty())
		sText = L"-";
	xmlDump.Write(L"Holidays", (const wchar_t *)sText);

	{
		CXMLDumpScope scope(L"Buttons", xmlDump);
		POSITION pos = mButtons.GetHeadPosition();
		while (pos)
		{
			CButtonRef *pRef = mButtons.GetNext(pos);
			if (pRef->mButton.GetCheck() == BST_CHECKED)
				xmlDump.Write((const wchar_t *)pRef->msName, L"checked");
			else
				xmlDump.Write((const wchar_t *)pRef->msName, L"not_checked");
		}
	}
}
void CWorkersRightsComputerDlg::LoadFromXml(CXMLParseNode* pRoot)
{
	CXMLParseNode *pMain = pRoot->GetFirst(L"MainDialog");
	if (!pMain)
		return;

	CXMLParseNode *pEdit = pMain->GetFirst(L"EditBoxes");
	if (!pEdit)
		return;
	CString sText;
	POSITION pos = mEditBoxes.GetHeadPosition();
	while (pos)
	{
		CEditRef* pRef = mEditBoxes.GetNext(pos);
		if (pEdit->GetValue((const wchar_t*)pRef->msName, sText))
			pRef->mEdit.SetWindowText(sText);
	}

	if (pMain->GetValue(L"Holidays", sText))
		mComboHolidays.SetWindowTextW(sText);

	CXMLParseNode* pButtons = pMain->GetFirst(L"Buttons");
	if (!pButtons)
		return;
	pos = mButtons.GetHeadPosition();
	while (pos)
	{
		CButtonRef* pRef = mButtons.GetNext(pos);
		if (pButtons->GetValue((const wchar_t*)pRef->msName, sText))
			pRef->mButton.SetCheck(sText == L"checked" ? BST_CHECKED : 0);
	}
}
void CWorkersRightsComputerDlg::OnTestLoadxml()
{
	CMyFileDialog dlg(CMyFileDialog::FD_OPEN, L"Select XML File");
	if (dlg.DoModal())
	{
		CSaver saver;
		saver.Restore(dlg.GetPathName());
	}
}
void CWorkersRightsComputerDlg::OnTestLoadtxt()
{
	CMyFileDialog dlg(CMyFileDialog::FD_OPEN, L"Select TXT File");
	if (dlg.DoModal())
	{
		CSaver saver;
		saver.Restore(dlg.GetPathName());
	}
}
void CWorkersRightsComputerDlg::WriteEditorToLetter(CHtmlWriter& html)
{
	CString sEditor(GetText(IDC_EDIT_FILLED_BY));
	html.WriteLineEH(L"Prepared by: ", L"הוכן על ידי: ", sEditor);
}


