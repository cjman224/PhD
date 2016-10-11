// Shaker Main Experiment.h : main header file for the SHAKER MAIN EXPERIMENT application
//

#if !defined(AFX_SHAKERMAINEXPERIMENT_H__DF1F6BE4_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
#define AFX_SHAKERMAINEXPERIMENT_H__DF1F6BE4_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentApp:
// See Shaker Main Experiment.cpp for the implementation of this class
//

class CShakerMainExperimentApp : public CWinApp
{
public:
	CShakerMainExperimentApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShakerMainExperimentApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CShakerMainExperimentApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAKERMAINEXPERIMENT_H__DF1F6BE4_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
