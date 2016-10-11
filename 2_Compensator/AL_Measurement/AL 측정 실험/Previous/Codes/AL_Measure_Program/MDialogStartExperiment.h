#if !defined(AFX_MDIALOGSTARTEXPERIMENT_H__DF1F6BF5_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
#define AFX_MDIALOGSTARTEXPERIMENT_H__DF1F6BF5_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MDialogStartExperiment.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MDialogStartExperiment dialog

class MDialogStartExperiment : public CDialog
{
// Construction
public:
	MDialogStartExperiment(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MDialogStartExperiment)
	enum { IDD = IDD_DIALOG_STARTEXPERIMENT };
	int		m_iExperimentNumber;
	CString	m_sSubjectCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MDialogStartExperiment)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MDialogStartExperiment)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDIALOGSTARTEXPERIMENT_H__DF1F6BF5_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
