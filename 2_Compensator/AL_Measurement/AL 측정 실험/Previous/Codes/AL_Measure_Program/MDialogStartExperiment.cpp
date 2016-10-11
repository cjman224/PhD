// MDialogStartExperiment.cpp : implementation file
//

#include "stdafx.h"
#include "Shaker Main Experiment.h"
#include "MDialogStartExperiment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MDialogStartExperiment dialog


MDialogStartExperiment::MDialogStartExperiment(CWnd* pParent /*=NULL*/)
	: CDialog(MDialogStartExperiment::IDD, pParent)
{
	//{{AFX_DATA_INIT(MDialogStartExperiment)
	m_iExperimentNumber = 1;
	m_sSubjectCode = _T("S3");
	//}}AFX_DATA_INIT
}


void MDialogStartExperiment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MDialogStartExperiment)
	DDX_Text(pDX, IDC_EDIT_EXPERIMENTNUMBER, m_iExperimentNumber);
	DDV_MinMaxInt(pDX, m_iExperimentNumber, 0, 10000);
	DDX_Text(pDX, IDC_EDIT_SUBJECTCODE, m_sSubjectCode);
	DDV_MaxChars(pDX, m_sSubjectCode, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MDialogStartExperiment, CDialog)
	//{{AFX_MSG_MAP(MDialogStartExperiment)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MDialogStartExperiment message handlers
