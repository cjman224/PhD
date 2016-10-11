// Shaker Main ExperimentView.h : interface of the CShakerMainExperimentView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAKERMAINEXPERIMENTVIEW_H__DF1F6BEC_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
#define AFX_SHAKERMAINEXPERIMENTVIEW_H__DF1F6BEC_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CShakerMainExperimentView : public CView
{
protected: // create from serialization only
	CShakerMainExperimentView();
	DECLARE_DYNCREATE(CShakerMainExperimentView)

// Attributes
public:
	CShakerMainExperimentDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShakerMainExperimentView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShakerMainExperimentView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CShakerMainExperimentView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Shaker Main ExperimentView.cpp
inline CShakerMainExperimentDoc* CShakerMainExperimentView::GetDocument()
   { return (CShakerMainExperimentDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAKERMAINEXPERIMENTVIEW_H__DF1F6BEC_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
