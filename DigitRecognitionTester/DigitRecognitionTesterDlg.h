
// DigitRecognitionTesterDlg.h : header file
//

#pragma once
#include "matrix.h"
#include <map>
#include <vector>
#include "Thread.h"
#include "GraphObject.h"

#define BLOCK_X 8
#define BLOCK_Y 12
#define LBLOCK_X BLOCK_X * 2
#define LBLOCK_Y BLOCK_Y * 2

#define INPUT_NODE 96
#define HIDDEN_NODE 48
#define OUTPUT_NODE 10

#if _MSC_VER > 1020 // if VC++ version is > 4.2
using namespace std; // std c++ libs implemented in std
#endif

typedef vector<int> DIGITPATTERN;
typedef vector<double> NNNODES;
typedef vector<vector<double>> NNLINKS;

typedef struct {
	CString szNeuralNetwork;	// Loaded neural network data file
	NNLINKS dWeight_btwnInputHidden; // weight in between input and hidden layer
	NNLINKS dWeight_btwnHiddenOutput; // weight in between hidden and output layer
	NNNODES dHiddenLayerNodes;	// hidden layer nodes
	NNNODES dOutputLayerNodes;	// output layer nodes
	int nCorrect;	// # of correct recognition on digits
	int nIncorrect;	// # of incorrect recognition on digits
}NN;

typedef struct {
	double NNOutput;
	int Digit;
}DIGITWEIGHT;

// CDigitRecognitionTesterDlg dialog
class CDigitRecognitionTesterDlg : public CDialogEx
{
// Construction
public:
	CDigitRecognitionTesterDlg(CWnd* pParent = nullptr);	// standard constructor
	void DrawBlock();
	void RedrawGridBlocks();
	void ResetGridBlocks();
	void NNForwardComputation(const DIGITPATTERN& inputPattern, 
		NN& NeuralNetwork, vector<double>& dOutputValues);
	void NNPreProcessingEachPattern(const DIGITPATTERN& inPattern, 
		DIGITPATTERN& outPattern);
	void LoadTestingDataFile(CString& szfilePathName);

	BOOL LoadNeuralNetwork(CString& szNeuralNetworkPathName, CString& szNeuralNetworkFileName);
	void SaveRecognitionResult(CString& szfilePathName);
	void CreatePatternResultsGraphs(CString& szNeuralNetwork);
	
	static UINT RunTestingThread(LPVOID pParam);

	double	m_dGridWidth;
	double	m_dGridHeight;
	Matrix	m_mtrxGridBlocks;
	BOOL	m_bDataLoaded;
	BOOL	m_bDrawingActivated;
	int		m_nRecognitionWaitingTerm;

	CThread* m_pRunTestingThread;

	map<CString, CGraphObject*> m_mapGraphObjects;

	multimap<int/*digit*/, DIGITPATTERN> m_mtmpTestdataPatterns;
	multimap<int/*digit*/, DIGITPATTERN> m_mtmPreprocessedTestdataPatterns;
	typedef multimap<int, DIGITPATTERN>::value_type m_PatternsT;

	// Neural networks
	vector<NN> m_vecNeuralNetworks;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIGITRECOGNITION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnClickedRunRecognition();
	afx_msg void OnHelpAbout();
	int m_nNumInputNode;
	int m_nNumHiddenNode;
	int m_nNumOutputNode;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDropdownRunRecognition(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnScreenClear();
	CString m_szTestingOutput;
	afx_msg void OnFileExit();
	afx_msg void OnClose();
	CListCtrl m_cListPatterns;
	afx_msg void OnBnClickedButtonLoadneuralnetwork();
	afx_msg void OnFileLoadtestdataset();
	afx_msg void OnClickedButtonLoadtestingdata();
	afx_msg void OnBnClickedButtonRuntesting();
	CString m_szTestingDataset;
	afx_msg void OnBnClickedButtonSaverecognitionresult();
	CProgressCtrl m_cRunRecognition;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnKeydownListPatternsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnBnClickedButton1();
};
