﻿
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
#include "WorkYears.h"
#include "OnOpenDlg.h"
#include "PrevYearsHolidaysDlg.h"
#include "Holidays.h"
#include "HolidaysDue.h"
#include "CommentsDlg.h"
#include "PrevYearsVacationsDlg.h"

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
public:
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
	, mbSaveInProcess(false)
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
	mEditBoxes.AddTail(new CEditRef(L"FilledByHebrew", mFilledByHebrew, NULL, false));

	// Employment details
	mEditBoxes.AddTail(new CEditRef(L"LastYearWork", mEditLastYearWork, L"textBox9"));
	mEditBoxes.AddTail(new CEditRef(L"LastYearPaid", mEditLastYearPaid, L"textBox3"));
	mEditBoxes.AddTail(new CEditRef(L"LastYearFrom", mEditLastYearFrom, L"textBox4"));
	mEditBoxes.AddTail(new CEditRef(L"AdditionalDesc", mEditAdditionalDesc, L"textBox14"));
	mEditBoxes.AddTail(new CEditRef(L"AdditionalSum", mEditAdditionalSum, L"textBox15"));
	mEditBoxes.AddTail(new CEditRef(L"PaidDesc", mEditPaidDesc));
	mEditBoxes.AddTail(new CEditRef(L"PaidSum", mEditPaidSum));

	mEditBoxes.AddTail(new CEditRef(L"VacationPrevYears", mEditVacationPrevYears, L"textBox11"));
	mEditBoxes.AddTail(new CEditRef(L"RecuperationPrevYears", mEditRecuperationPrevYears, L"textBox13"));
	mEditBoxes.AddTail(new CEditRef(L"VacationDaysPaidLastYear", mEditNDaysPaidLastYear));

	mButtons.AddTail(new CButtonRef(L"AllowSevLess", mAllowSevLess, L"checkBox1"));
	mButtons.AddTail(new CButtonRef(L"DemandVac4Prev", mDemandVac4Prev, L"checkBox2"));
	mButtons.AddTail(new CButtonRef(L"DemandRec4Prev", mDemandRec4Prev, L"checkBox3"));
	mButtons.AddTail(new CButtonRef(L"EntitledOnly2Sev", mEntitledOnly2Sev, L"checkBox11"));
	mButtons.AddTail(new CButtonRef(L"HadPensionBefore", mHadPensionBefore, L"checkBox12"));

	mButtons.AddTail(new CButtonRef(L"RadioID", mRadiID, NULL, false));
	mButtons.AddTail(new CButtonRef(L"RadioPass", mRadioPassport, NULL, false));

	// Notice that the radio button "PaidLastYVacation" replaces the old checkbox
	// Its name stays the same for backward compatibility - April 15, 2023
	mButtons.AddTail(new CButtonRef(L"PaidLastYVacation", mVacationPaidAll4LastYear, NULL));
	mButtons.AddTail(new CButtonRef(L"PaidDaysLastYVacation", mVacationPaidDays4LastYear, NULL));
	mButtons.AddTail(new CButtonRef(L"PaidNoneLastYVacation", mVacationPaidNone4LastYear, NULL, false));
	mButtons.AddTail(new CButtonRef(L"PaidLastYRecuperation", mRecuperationPaid4LastYear, NULL));
}

void CWorkersRightsComputerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_WORK, mEditLastYearWork);
	DDX_Control(pDX, IDC_EDIT_FIRST_NAME, mEditFirstName);
	DDX_Control(pDX, IDC_EDIT_FAMILY_NAME, mEditFamilyName);
	DDX_Control(pDX, IDC_EDIT_ID, mEditID);
	DDX_Control(pDX, IDC_EDIT_TEL, mEditTel);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_PAID, mEditLastYearPaid);
	DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_FROM, mEditLastYearFrom);
	//DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_WORK, mEditPrevYearWork);
	//DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_PAID, mEditPrevYearPaid);
	//DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_FROM, mEditPrevYearFrom);
	DDX_Control(pDX, IDC_EDIT_ADDITIONAL_DESC, mEditAdditionalDesc);
	DDX_Control(pDX, IDC_EDIT_ADDITIONAL_SUM, mEditAdditionalSum);
	DDX_Control(pDX, IDC_EDIT_PAID_DESC, mEditPaidDesc);
	DDX_Control(pDX, IDC_EDIT_PAID_SUM, mEditPaidSum);
	DDX_Control(pDX, IDC_EDIT_VACATION_YEARS, mEditVacationPrevYears);
	DDX_Control(pDX, IDC_EDIT_RECUPERATION_YEARS, mEditRecuperationPrevYears);
	DDX_Control(pDX, IDC_EDIT_LAST_YEAR_VACATION_DAYS, mEditNDaysPaidLastYear);
	DDX_Control(pDX, IDC_COMBO_HOLIDAYS, mComboHolidays);
	DDX_Control(pDX, IDC_CHECK_SEVERANCE_LESS_THAN_YEAR, mAllowSevLess);
	DDX_Control(pDX, IDC_CHECK_VACATION_YEARS, mDemandVac4Prev);
	DDX_Control(pDX, IDC_CHECK_RECUPERATION_YEARS, mDemandRec4Prev);
	DDX_Control(pDX, IDC_CHECK_ONLY_SEVERANCE, mEntitledOnly2Sev);
	DDX_Control(pDX, IDC_CHECK_ACTIVE_PENSION, mHadPensionBefore);
	DDX_Control(pDX, IDC_EDIT_SUMMARY, mSummary);
	//DDX_Control(pDX, IDC_EDIT_HOLIDAYS_PREVY_N_YEARS, mEditPrevNYears);
	DDX_Control(pDX, IDC_RADIO_ID, mRadiID);
	DDX_Control(pDX, IDC_RADIO_PASSPORT, mRadioPassport);
	DDX_Control(pDX, IDC_EDIT_FILLED_BY, mFilledBy);
	DDX_Control(pDX, IDC_EDIT_FILLED_BY_HEBREW, mFilledByHebrew);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, mAddress);
	DDX_Control(pDX, IDC_EDIT_EMAIL, mEmail);
	// DDX_Control(pDX, IDC_EDIT_PAY_PER_HOLY_DAY, mEditPayPerEachHolyDay);
	DDX_Control(pDX, IDC_RADIO_ALL_LAST_YEAR_VACATION, mVacationPaidAll4LastYear);
	DDX_Control(pDX, IDC_RADIO_LAST_YEAR_VACATION_DAYS, mVacationPaidDays4LastYear);
	DDX_Control(pDX, IDC_RADIO_LAST_YEAR_VACATION_NONE, mVacationPaidNone4LastYear);
	DDX_Control(pDX, IDC_CHECK_PAID_RECUP, mRecuperationPaid4LastYear);
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
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK, &CWorkersRightsComputerDlg::OnEnChangeEditHolidaysLastyWork)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID, &CWorkersRightsComputerDlg::OnEnChangeEditHolidaysLastyWork)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM, &CWorkersRightsComputerDlg::OnEnChangeEditHolidaysLastyWork)
	ON_EN_CHANGE(IDC_EDIT_ADDITIONAL_SUM, &CWorkersRightsComputerDlg::OnInputChange)
	ON_EN_CHANGE(IDC_EDIT_PAID_SUM, &CWorkersRightsComputerDlg::OnInputChange)
	ON_COMMAND(ID_COMPUTE_ALL, &CWorkersRightsComputerDlg::OnComputeAll)
	ON_COMMAND(ID_FILE_SAVEAS, &CWorkersRightsComputerDlg::OnFileSaveas)
	ON_COMMAND(ID_FILE_LOAD, &CWorkersRightsComputerDlg::OnFileLoad)
	ON_COMMAND(ID_TEST_CREATEDIR, &CWorkersRightsComputerDlg::OnTestCreatedir)
	//ON_COMMAND(ID_FILE_LOADOLDCASE, &CWorkersRightsComputerDlg::OnFileLoadoldcase)
	ON_COMMAND(ID_TEST_VERIFYBATCH, &CWorkersRightsComputerDlg::OnTestVerifybatch)
	ON_COMMAND(ID_TEST_WRITEHTML, &CWorkersRightsComputerDlg::OnTestWritehtml)
	ON_BN_CLICKED(IDC_EMPLOYER, &CWorkersRightsComputerDlg::OnBnClickedEmployer)
	ON_BN_CLICKED(IDC_COMMENTS, &CWorkersRightsComputerDlg::OnBnClickedComments)
	ON_COMMAND(ID_TEST_LOADXML, &CWorkersRightsComputerDlg::OnTestLoadxml)
	ON_COMMAND(ID_TEST_LOADTXT, &CWorkersRightsComputerDlg::OnTestLoadtxt)
	//ON_BN_CLICKED(IDC_CHECK_PAID_VACATION, &CWorkersRightsComputerDlg::OnBnClickedCheckPaidVacation)
	ON_BN_CLICKED(IDC_RADIO_ALL_LAST_YEAR_VACATION, &CWorkersRightsComputerDlg::OnRadioPaidVacationForLastYearChange)
	ON_BN_CLICKED(IDC_RADIO_LAST_YEAR_VACATION_DAYS, &CWorkersRightsComputerDlg::OnRadioPaidVacationForLastYearChange)
	ON_BN_CLICKED(IDC_RADIO_LAST_YEAR_VACATION_NONE, &CWorkersRightsComputerDlg::OnRadioPaidVacationForLastYearChange)
	ON_EN_CHANGE(IDC_EDIT_LAST_YEAR_VACATION_DAYS, &CWorkersRightsComputerDlg::OnInputChange)
	ON_BN_CLICKED(IDC_CHECK_PAID_RECUP, &CWorkersRightsComputerDlg::OnInputChange)
	ON_BN_CLICKED(IDC_CHECK_LIVE_IN, &CWorkersRightsComputerDlg::OnBnClickedCheckLiveIn)
	ON_BN_CLICKED(IDC_BUTTON_PREV_YEARS_HOLIDAYS, &CWorkersRightsComputerDlg::OnBnClickedButtonPrevYearsHolidays)
	ON_BN_CLICKED(IDC_CHECK_CAREGIVER, &CWorkersRightsComputerDlg::OnBnClickedCheckCaregiver)
	ON_BN_CLICKED(IDC_BUTTON_VACATIONS_PREV_YEARS, &CWorkersRightsComputerDlg::OnBnClickedButtonVacationsPrevYears)
	ON_EN_CHANGE(IDC_EDIT_VACATION_YEARS, &CWorkersRightsComputerDlg::OnEnChangeEditVacationYears)
	ON_EN_CHANGE(IDC_EDIT_RECUPERATION_YEARS, &CWorkersRightsComputerDlg::OnEnChangeEditRecuperationYears)
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
	mVacationPaidNone4LastYear.SetCheck(1);
	gConfig.InitFromXml();
	CString sTitle(L"Workers Rights Computer - Experimental Beta Version ");
	SetTitle(sTitle + gConfig.msVersion);

	CUtils::CreateThread(&StaticThreadFunc, NULL);
	OnLoad();

	gpDlg = this;
	mbInitialized = true;
	mbDisableComputations = false;

	COnOpenDlg initDlg;
	initDlg.DoModal();

	SetText(IDC_EDIT_FILLED_BY, gConfig.msFilledBy);
	SetText(IDC_EDIT_FILLED_BY_HEBREW, gConfig.msFilledByHebrew);

	gHolidaysDue.UpdateMainDialog();

	if (!msStartFile.IsEmpty())
	{
		CSaver saver;
		saver.Restore(msStartFile);
	}

	GotoDlgCtrl(GetDlgItem(IDC_EDIT_FIRST_NAME));
	SetCheck(IDC_CHECK_CAREGIVER, true);
	return FALSE;  // return TRUE  unless you set the focus to a control
}
void CWorkersRightsComputerDlg::OnLoad()
{
	SetCheck(IDC_CHECK_LIVE_IN, gWorkPeriod.mbLiveIn);
	SetCheck(IDC_CHECK_CAREGIVER, gWorkPeriod.mbCaregiver);
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
	if (!VerifyThatNotInSave())
		return;
		
	CMyAskDlg dlg(L"Verify OK", L"Do you want to save and to exit?");
	if (dlg.Ask())
	{
		mbSaveFailed = false;
		OnBnClickedButtonSave();

		if (!mbSaveFailed) // On save failure - stay in application
			CDialogEx::OnOK();
	}
}

void CWorkersRightsComputerDlg::OnCancel()
{
	if (!VerifyThatNotInSave())
		return;

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
	{
		if (!gWorkYears.mbAllowPartYearSeverance)
		{
			SetCheck(IDC_CHECK_SEVERANCE_LESS_THAN_YEAR, false);
		}
		if (gWorkYears.mYearsForSeverance < 1)
		{
			if (!IsChecked(IDC_CHECK_SEVERANCE_LESS_THAN_YEAR))
				SetCheck(IDC_CHECK_ONLY_SEVERANCE, true);
		}
		OnInputChange();
	}
	gHolidaysDue.VerifyWorkPeriod(this);
}
void CWorkersRightsComputerDlg::OnBnClickedButtonSave()
{
	if (VerifyThatNotInSave())
	{
		OnInputChange();
		mbSaveInProcess = true;
		CSaver saver;
		mbSaveFailed = !saver.Save();
		mbSaveInProcess = false;
	}
}
bool CWorkersRightsComputerDlg::VerifyThatNotInSave()
{
	if (mbSaveInProcess)
	{
		CUtils::MessBox(L"Please OK previous save", L"Warning");
		return false;
	}
	return true;
}
void CWorkersRightsComputerDlg::OnBnClickedButtonLoad()
{
	CSaver saver;
	saver.Restore();
}
void CWorkersRightsComputerDlg::OnBnClickedButtonReset()
{
	CSaver::ResetAllInputs(false /*!bLoading*/);
	OnInputChange();
	gHolidaysDue.UpdateMainDialog();
}
void CWorkersRightsComputerDlg::ResetAllInputs(bool bLoading)
{
	POSITION pos = mEditBoxes.GetHeadPosition();
	while (pos)
	{
		CEditRef* pEdit = mEditBoxes.GetNext(pos);
		pEdit->mEdit.SetWindowTextW(L"0");
	}

	static int i = 0;
	pos = mButtons.GetHeadPosition();
	while (pos)
	{
		CButtonRef* pButton = mButtons.GetNext(pos);
		if (pButton->msName == "PaidNoneLastYVacation")
			pButton->mButton.SetCheck(BST_CHECKED);
		else
			pButton->mButton.SetCheck(BST_UNCHECKED);
		i++;
	}
	mComboHolidays.SetWindowTextW(L"Select set of Holidays");
	if (!bLoading)
	{
		mFilledBy.SetWindowTextW(gConfig.msFilledBy);
		mFilledByHebrew.SetWindowTextW(gConfig.msFilledByHebrew);

	}
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
	if (!gWorkPeriod.IsValid())
	{
		SetCheck(IDC_CHECK_SEVERANCE_LESS_THAN_YEAR, false);
		CUtils::MessBox(L"Please define working period first", L"Notice");
		return;
	}
	if (IsChecked(IDC_CHECK_SEVERANCE_LESS_THAN_YEAR))
	{
		if (!gWorkYears.mbAllowPartYearSeverance)
		{
			SetCheck(IDC_CHECK_SEVERANCE_LESS_THAN_YEAR, false);
			CUtils::MessBox(L"Period too short for full severance on less than a year", L"Notice");
		}
	}
	if (IsChecked(IDC_CHECK_SEVERANCE_LESS_THAN_YEAR))
		SetCheck(IDC_CHECK_ONLY_SEVERANCE, false);
	else
	{
		if (gWorkYears.mYearsForSeverance < 1)
			SetCheck(IDC_CHECK_ONLY_SEVERANCE, true);
	}
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
	SetCheck(IDC_CHECK_SEVERANCE_LESS_THAN_YEAR, false);
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
	CString sDir(L"F:\\WorkersRights\\Save");
	CMyFileDialog dlg(CMyFileDialog::FD_SAVE, L"save file", sDir);
	dlg.SetDefaultExtention(L"xml");
	if (dlg.DoModal())
	{
		CString sfName(dlg.mSelectedFileName);
		CSaver saver;
		saver.Save(sfName);
	}
}
void CWorkersRightsComputerDlg::OnFileLoad()
{
	CString sDir(L"F:\\WorkersRights\\Save");
	CMyFileDialog dlg(CMyFileDialog::FD_OPEN, L"saved file", sDir);
	dlg.SetDefaultExtention(L"xml");
	if (dlg.DoModal())
	{
		CString sfName(dlg.mSelectedFileName);
		CSaver saver;
		saver.Restore(sfName);
	}
}
void CWorkersRightsComputerDlg::OnTestCreatedir()
{
	CUtils::VerifyDirectory(L"F:\\tmp\\new_dir");
}
void CWorkersRightsComputerDlg::OnInputChange()
{
	OnInputChange(false);
}
void CWorkersRightsComputerDlg::OnInputChange(bool bJustLoaded)
{
	if (mbDisableComputations)
		return;

	static bool ubInChange = false;
	if (ubInChange)
		return;
	ubInChange = true;

	gHolidaysDue.VerifyWorkPeriod(this);
	if (!bJustLoaded)
		gHolidaysDue.OnMainDialogChange(this);

	gAllRights.Compute();
	ubInChange = false;
}
void CWorkersRightsComputerDlg::OnRadioPaidVacationForLastYearChange()
{
	if (!IsChecked(IDC_RADIO_LAST_YEAR_VACATION_DAYS))
		Clear(IDC_EDIT_LAST_YEAR_VACATION_DAYS);
	OnInputChange();
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
	CString sDir(L"F:\\WorkersRights\\Verify");
	CMyFileDialog dlg(CMyFileDialog::FD_OPEN, L"saved file", sDir);
	dlg.SetDefaultExtention(L"xml");
	if (dlg.DoModal())
	{
		CString sfName(dlg.mSelectedFileName);
		CVerify::StartVerifyBatch(sfName);
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
	CCommentsDlg dlg;
	dlg.DoModal();
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
bool CWorkersRightsComputerDlg::LoadFromXml(CXMLParseNode* pRoot)
{
	CXMLParseNode *pMain = pRoot->GetFirst(L"MainDialog");
	if (!pMain)
		return false;

	CXMLParseNode *pEdit = pMain->GetFirst(L"EditBoxes");
	if (!pEdit)
		return false;
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
		return false;
	pos = mButtons.GetHeadPosition();
	while (pos)
	{
		CButtonRef* pRef = mButtons.GetNext(pos);
		if (pButtons->GetValue((const wchar_t*)pRef->msName, sText))
			pRef->mButton.SetCheck(sText == L"checked" ? BST_CHECKED : 0);
	}
	return true;
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
	CString sEditorH(GetText(IDC_EDIT_FILLED_BY_HEBREW));
	CString sPrepared(L"Prepared by: ");
	sPrepared += sEditor;
	CString sPreparedH(L"הוכן על ידי: ");
	sPreparedH += sEditorH;

	html.WriteLineEH(sPrepared, sPreparedH);
}
void CWorkersRightsComputerDlg::OnBnClickedCheckLiveIn()
{
	gWorkPeriod.mbLiveIn = IsChecked(IDC_CHECK_LIVE_IN);
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnBnClickedCheckCaregiver()
{
	gWorkPeriod.mbCaregiver = IsChecked(IDC_CHECK_CAREGIVER);
}
void CWorkersRightsComputerDlg::OnBnClickedButtonPrevYearsHolidays()
{
	// Return "holidays" only if it is valid!
	CHolidays* pHolidays = gAllRights.GetHolidays();
	if (!pHolidays)
		return;

	gHolidaysDue.VerifyWorkPeriod(this);
	CPrevYearsHolidaysDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
	}
	gHolidaysDue.UpdateMainDialog();
}
void CWorkersRightsComputerDlg::OnBnClickedButtonVacationsPrevYears()
{
	CPrevYearsVacationsDlg dlg;
	dlg.DoModal();
}
void CWorkersRightsComputerDlg::OnEnChangeEditVacationYears()
{
	OnInputChange();
}
void CWorkersRightsComputerDlg::OnEnChangeEditRecuperationYears()
{
	OnInputChange();
}
