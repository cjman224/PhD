// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DF1F6BE6_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
#define AFX_STDAFX_H__DF1F6BE6_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



// My definitions


// staircase direction
#define GOING_UP 1
#define GOING_DOWN (-1)

// state definition for 3 up 1 down 
#define NUM_CORRECT_0 0
#define NUM_CORRECT_1 1 
#define NUM_CORRECT_2 2 

// user response
#define RIGHT_ANSWER 0
#define WRONG_ANSWER 1

#define HIGH 0
#define LOW 1


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DF1F6BE6_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
