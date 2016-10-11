// Shaker Main ExperimentView.cpp : implementation of the CShakerMainExperimentView class
//

#include "stdafx.h"
#include "Shaker Main Experiment.h"

#include "Shaker Main ExperimentDoc.h"
#include "Shaker Main ExperimentView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentView

IMPLEMENT_DYNCREATE(CShakerMainExperimentView, CView)

BEGIN_MESSAGE_MAP(CShakerMainExperimentView, CView)
	//{{AFX_MSG_MAP(CShakerMainExperimentView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentView construction/destruction

CShakerMainExperimentView::CShakerMainExperimentView()
{
	// TODO: add construction code here
//	CShakerMainExperimentDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);
//	pDoc->pView=this;

}

CShakerMainExperimentView::~CShakerMainExperimentView()
{
}

BOOL CShakerMainExperimentView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentView drawing

void CShakerMainExperimentView::OnDraw(CDC* pDC)
{
	CShakerMainExperimentDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	// TODO: add draw code for native data here
	if (pDoc->bIsExperimentStarted == false)
		return;

	// make a font
	CFont font;
	font.CreateFont(
		150,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial");                 // lpszFacename

	CFont* def_font = pDC->SelectObject(&font);
	COLORREF def_color = pDC->SetTextColor(RGB(0,0,255));
	
	RECT clientWindowSize;
	GetClientRect(&clientWindowSize);

	CString sRightAnswer("The Answer Was ");
	CString sRightNumber;
	CString sTrial("Trial "), temp;
	temp.Format("%d", pDoc->iCurrentTrialNumber);
	sTrial += temp;

	pDC->DrawText(sTrial, &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_TOP );
	
	switch (pDoc->iCurrentStimulusNumber) {
		case 1:
			pDC->DrawText("1st Stimulus", &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
			break;
		case 2:
			pDC->DrawText("2nd Stimulus", &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
			break;
		case 3:
			pDC->DrawText("3rd Stimulus", &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
			break;
		case 4:
			pDC->DrawText("Your Answer?", &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
			break;
		case 5:		// when feedback is on
			if ( pDoc->bWasAnswerRight == true )
				pDC->DrawText("Correct !!", &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
			else {
				pDC->DrawText("Wrong !!", &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
				sRightNumber.Format("%d", pDoc->iRightAnswer);
				sRightAnswer = sRightAnswer + sRightNumber;
				pDC->DrawText(sRightAnswer, &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_BOTTOM );
			}
			break;
		case 6:
			pDC->DrawText("Done. Thanks!!", &clientWindowSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
			break;
		default:
			break;
	}

	// Simply for testing
//	CString sRightAnswer;
//	sRightAnswer.Format("%d", pDoc->iRightAnswer);
//	pDC->TextOut(0, 0, sRightAnswer);

	// Restore defaults DC properties
	pDC->SetTextColor(def_color);
	pDC->SelectObject(def_font);
	font.DeleteObject(); 	

}

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentView printing

BOOL CShakerMainExperimentView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CShakerMainExperimentView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CShakerMainExperimentView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentView diagnostics

#ifdef _DEBUG
void CShakerMainExperimentView::AssertValid() const
{
	CView::AssertValid();
}

void CShakerMainExperimentView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShakerMainExperimentDoc* CShakerMainExperimentView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShakerMainExperimentDoc)));
	return (CShakerMainExperimentDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentView message handlers
