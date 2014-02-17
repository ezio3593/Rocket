// RocketDlg.h : header file
//
#include "Context.h"

#pragma once


// CRocketDlg dialog
class CRocketDlg : public CDialog
{
	DrawingContext *dContext;
	Context* context;

	std::vector<Rocket*> rockets;

	CClientDC* devc;

	int maxAngleValue;
	int minAngleValue;

	CSpinButtonCtrl* spin;
	CButton* startButton;
	CButton* stopButton;
	CButton* addButton;

// Construction
public:
	CRocketDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ROCKET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
public:
	afx_msg void OnBnClickedStop();
public:
	afx_msg void OnBnClickedAdd();

	~CRocketDlg();
};
