// Shaker Main ExperimentDoc.h : interface of the CShakerMainExperimentDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAKERMAINEXPERIMENTDOC_H__DF1F6BEA_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
#define AFX_SHAKERMAINEXPERIMENTDOC_H__DF1F6BEA_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "shaker.h"
//#include "Shaker Main ExperimentView.h"

class CShakerMainExperimentDoc : public CDocument
{
protected: // create from serialization only
	CShakerMainExperimentDoc();
	DECLARE_DYNCREATE(CShakerMainExperimentDoc)

// Attributes
public:
	CString sSubjectCode;	// subject code
	CString sSubjectFirstName;	// subject first name
	CString sSubjectLastName;	// subject last name
	int iSubjectAge;	// subject age
	CString sSubjectGender;		// subject gender	
	CString sSubjectDominantHand;	// subject dominant hand
	CString sFeedback;	// feedback 

	int iExperimentNumber;
	int iNumberOfFrequencies;
	int iNumberOfAmplitudeLevels;
	int *piFrequencies;		// Hz
	float *pfRoughThresholds;	// micrometer
	double *pDisplacement;
	int iInterStimulusInterval;		// ms
	int iStimulusDuration;		// ms
	int iNumberOfReversals;
	float *pfStepSizes;
	int *piOrderOfFrequencies;	// randomized frequency order for an experiment
	int iCurrentCondition;
	int iCurrentConditionIndex;	// index of the frequency for the current condition

	float fInitialSignalIntensity;	// dB above threshold
	// directory for experiment data
	char main_dir_path[100];

	// flag saying if the program is ready for starting an expereiment
	bool bIsDataReady;
	bool bIsExperimentStarted;

	// variables to run the 3-1 staircase adaptive technique
	double dCurrentStimulusIntensity;	// micrometer
	int iCurrentNumberOfReversals;
	int iCurrentStaircaseDirection;
	int iCurrentExperimentState;
	int iCurrentTrialNumber;
	int iRightAnswer;
	int iCurrentResponse;
	int iCurrentStimulusNumber;
	
	bool bWasAnswerRight;
	bool bHasResponseYet;
	bool bIsReversalOccurred; 


// output file
	FILE *pOutputFileID, *pMeasurementDataOutputFileID;
	FILE *facc;

	// calibration constants
	float *pfSlope, *pfOffset;

	
	// data for the sine wave
	struct sine_wave NullStimulus;
	struct sine_wave TestStimulus;
//	struct sine_wave PreviousTestStimulus;

	// task handles for the DAQ card
	TaskHandle  taskHandleInput, taskHandleOutput;	
	
	// for DAQ IO
	float64  *pdOutputData, *pdInputData;
	float64  *pdOutputBuffer, *pdOutputBufferForNullStimulus, *pdOutputBufferForTestStimulus, *pdInputBuffer;
	int32 ulCount;
	
public:	
	bool ReadExperimentData();
	bool ReadCalibrationFile();
	bool HarewareInitialize();
	bool SetIOModeOfDAQCard();
	bool PrepareOutputFile();
	void ClearUp();
	float64 ComputeVoltageFromDisplacement(double dDisplacement, int iFrequency);
	bool ComputeDAQOutputForNullStimulus();
	bool ComputeDAQOutputForTestStimulus();
	bool UpdateConfigurationFile();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShakerMainExperimentDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShakerMainExperimentDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CShakerMainExperimentDoc)
	afx_msg void OnExperimentStartexperiment();
	afx_msg void OnUpdateExperimentStartexperiment(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAKERMAINEXPERIMENTDOC_H__DF1F6BEA_F7D8_11D8_AAC5_00C04F6920E6__INCLUDED_)
