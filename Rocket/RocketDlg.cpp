// RocketDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Rocket.h"
#include "RocketDlg.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRocketDlg dialog


CRocketDlg::CRocketDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRocketDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	maxAngleValue = 90;
	minAngleValue = -90;

	context = new Context();
	dContext = new DrawingContext(context->getObjCriticalSection());

	rockets = new std::vector<Rocket*>();
}

void CRocketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRocketDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START, &CRocketDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CRocketDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_ADD, &CRocketDlg::OnBnClickedAdd)
END_MESSAGE_MAP()


// CRocketDlg message handlers

BOOL CRocketDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_MINIMIZE);

	// TODO: Add extra initialization here
	
	spin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_ANGLE);
	spin->SetRange(minAngleValue, maxAngleValue);
	
	startButton = (CButton*)GetDlgItem(IDC_START);
	stopButton  = (CButton*)GetDlgItem(IDC_STOP);
	addButton  = (CButton*)GetDlgItem(IDC_ADD);

	CStatic *pc = (CStatic*)GetDlgItem(IDC_OPENGL);
	

	int res = dContext->init(pc);
	if (res) 
	{
		MessageBox(_T("Cannot init OpenGL context"), _T("Error"), MB_ICONERROR | MB_OK);
		EndDialog(res);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRocketDlg::OnPaint()
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
		CDialog::OnPaint();
	}
	
	stopButton->EnableWindow(FALSE);

	dContext->redrawScene();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRocketDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRocketDlg::OnBnClickedStart()
{
	try
	{
		context->start();
		dContext->unmakeCurrent();
		dContext->startRedrawingThread();
	} catch (const std::exception& exc)
	{
		MessageBoxA(::GetActiveWindow(), exc.what(), "Error", MB_ICONERROR | MB_OK);
		EndDialog(-1);
	}

	for (int i = 0; i < rockets->size(); ++i)
	{
		rockets->at(i)->start();
	}

	startButton->EnableWindow(FALSE);
	stopButton->EnableWindow(TRUE);
}

void CRocketDlg::OnBnClickedStop()
{
	context->stop();
	dContext->stopRedrawingThread();

	startButton->EnableWindow(TRUE);
	stopButton->EnableWindow(FALSE);

}

void CRocketDlg::OnBnClickedAdd()
{
	Rocket* r = new Rocket();
	rockets->push_back(r);

	dContext->addObject(r);
	r->setLimitCoords(0, 0, dContext->getWidth(), dContext->getHeight());
	int res = context->addObject(r);

	if (res)
	{
		MessageBox(_T("Cannot add object to context"), _T("Error"), MB_ICONERROR | MB_OK);
		EndDialog(res);
	}
	
	r->setAngle((INT16)spin->GetPos());
	
	if (context->isStarted()) 
		r->start();
	else dContext->redrawScene();
}

CRocketDlg::~CRocketDlg()
{
	delete context;
	delete dContext;

	for (int i = 0; i < rockets->size(); ++i)
	{
		delete rockets->at(i);
	}

	delete rockets;
}
