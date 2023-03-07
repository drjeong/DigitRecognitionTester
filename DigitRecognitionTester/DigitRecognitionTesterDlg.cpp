
// DigitRecognitionTesterDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "DigitRecognitionTester.h"
#include "DigitRecognitionTesterDlg.h"
#include "afxdialogex.h"
#include "StringWrapper.h"
#include "resource.h"
#include <algorithm>
#include <CSVParser.h>
#include <cvt/wstring>
typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_t;

#define ID_OBJECT_GRAPH_5			40004

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define DEBUG_SCREEN

#define NNDATA_NAME 0
#define NNDATA_CORRECT 1
#define NNDATA_INCORRECT 2
#define NNDATA_ACCURACY 3

#define sigmoid(x)  (1./  (1 + exp(-x)) )
#define ReLU(x) (x * (x > 0))

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnHelpAbout();
//	afx_msg void OnFileLoad();
//	afx_msg void OnClose();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_COMMAND(ID_HELP_ABOUT, &CAboutDlg::OnHelpAbout)
//ON_COMMAND(ID_FILE_LOAD, &CAboutDlg::OnFileLoad)
//ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDigitRecognitionTesterDlg dialog

CDigitRecognitionTesterDlg::CDigitRecognitionTesterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIGITRECOGNITION_DIALOG, pParent)
	, m_nNumInputNode(INPUT_NODE)
	, m_nNumHiddenNode(HIDDEN_NODE)
	, m_nNumOutputNode(OUTPUT_NODE)
	, m_szTestingOutput("")
	//, m_szHumanJudgement(_T(""))
	, m_szTestingDataset(_T(""))
	, m_nRecognitionWaitingTerm(100)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDigitRecognitionTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NUM_INPUTNODE, m_nNumInputNode);
	DDX_Text(pDX, IDC_NUM_HIDDENNODE, m_nNumHiddenNode);
	DDX_Text(pDX, IDC_NUM_OUTPUTNODE, m_nNumOutputNode);
	DDX_Text(pDX, IDC_EDIT_TESTINGOUTPUT, m_szTestingOutput);
	DDX_Control(pDX, IDC_LIST_PATTERNS_LIST, m_cListPatterns);
	DDX_Text(pDX, IDC_EDIT_TESETINGDATASET, m_szTestingDataset);
	DDX_Control(pDX, IDC_PROGRESS_RUNRECOGNITION, m_cRunRecognition);
}

BEGIN_MESSAGE_MAP(CDigitRecognitionTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_HELP_ABOUT, &CDigitRecognitionTesterDlg::OnHelpAbout)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_SCREEN_CLEAR, &CDigitRecognitionTesterDlg::OnScreenClear)
	ON_COMMAND(ID_FILE_EXIT, &CDigitRecognitionTesterDlg::OnFileExit)

	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_LOADNEURALNETWORK, &CDigitRecognitionTesterDlg::OnBnClickedButtonLoadneuralnetwork)
	ON_COMMAND(ID_FILE_LOADTESTDATASET, &CDigitRecognitionTesterDlg::OnFileLoadtestdataset)
	ON_BN_CLICKED(IDC_BUTTON_LOADTESTINGDATA, &CDigitRecognitionTesterDlg::OnClickedButtonLoadtestingdata)
	ON_BN_CLICKED(IDC_BUTTON_RUNTESTING, &CDigitRecognitionTesterDlg::OnBnClickedButtonRuntesting)
	ON_BN_CLICKED(IDC_BUTTON_SAVERECOGNITIONRESULT, &CDigitRecognitionTesterDlg::OnBnClickedButtonSaverecognitionresult)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PATTERNS_LIST, &CDigitRecognitionTesterDlg::OnKeydownListPatternsList)
	ON_WM_KEYDOWN()
//	ON_BN_CLICKED(IDC_BUTTON1, &CDigitRecognitionTesterDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDigitRecognitionTesterDlg message handlers

BOOL CDigitRecognitionTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_bDataLoaded = FALSE;
	m_bDrawingActivated = FALSE;

	m_mtrxGridBlocks.SetSize(BLOCK_Y, BLOCK_X);
	m_mtrxGridBlocks.Null();

	//m_mtrxPreProcessedGridBlocks.SetSize(BLOCK_Y, BLOCK_X);
	//m_mtrxPreProcessedGridBlocks.Null();

	GetDlgItem(IDC_BUTTON_RUNTESTING)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SAVERECOGNITIONRESULT)->EnableWindow(FALSE);

	m_cListPatterns.SetExtendedStyle(m_cListPatterns.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT);
	m_cListPatterns.InsertColumn(NNDATA_NAME, _T("NNData Name"), LVCFMT_LEFT, 200);
	m_cListPatterns.InsertColumn(NNDATA_CORRECT, _T("Correct"), LVCFMT_LEFT, 180);
	m_cListPatterns.InsertColumn(NNDATA_INCORRECT, _T("Incorrect"), LVCFMT_LEFT, 180);
	m_cListPatterns.InsertColumn(NNDATA_ACCURACY, _T("Accuracy"), LVCFMT_LEFT, 180);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDigitRecognitionTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDigitRecognitionTesterDlg::OnPaint()
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
		DrawBlock();

		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDigitRecognitionTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDigitRecognitionTesterDlg::OnHelpAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CDigitRecognitionTesterDlg::CreatePatternResultsGraphs(CString& szNeuralNetwork)
{
	CRect rtRegion;
	GetDlgItem(IDC_DRAWING_REGION_RESULTS)->GetWindowRect(rtRegion);
	ScreenToClient(rtRegion);

	int width = rtRegion.Width() / (int)m_vecNeuralNetworks.size();
	CRect rtGraph = rtRegion;
	rtGraph.right = rtGraph.left + width;

	// update positions & size for the previously generated graphs
	for (auto& graph : m_mapGraphObjects) {
		graph.second->SetGraphSize(CPoint(rtGraph.Width(), rtGraph.Height()));
		graph.second->SetGraphPosition(CPoint(rtGraph.left, rtGraph.top));
		rtGraph.OffsetRect(width, 0);
	}

	//CRect tmp(40, 490, 2561, 817);
	//CRect(40, 240, 640, 840)

	// generate hashcode
	std::string sText(CW2A(szNeuralNetwork.GetString(), CP_UTF8));
	UINT hashcode = (UINT)std::hash<std::string>()(sText);

	// Create CGraphObject ( 2DBar graph )
	CGraphObject *pGraphObject = new CGraphObject();
	pGraphObject->Create(NULL, NULL, NULL, rtGraph, this, hashcode, NULL);

	// Create graph and set graph parameters
	pGraphObject->SetGraphBackgroundColor(RGB(255, 255, 255));
	pGraphObject->SetGraphTitle(szNeuralNetwork);
	pGraphObject->ShowGraphLegend(FALSE);
	pGraphObject->CreateGraph(GT_2DBAR);

	// Add graph segments
	pGraphObject->Add2DBarGraphSegment(_T("0"));
	pGraphObject->Add2DBarGraphSegment(_T("1"));
	pGraphObject->Add2DBarGraphSegment(_T("2"));
	pGraphObject->Add2DBarGraphSegment(_T("3"));
	pGraphObject->Add2DBarGraphSegment(_T("4"));
	pGraphObject->Add2DBarGraphSegment(_T("5"));
	pGraphObject->Add2DBarGraphSegment(_T("6"));
	pGraphObject->Add2DBarGraphSegment(_T("7"));
	pGraphObject->Add2DBarGraphSegment(_T("8"));
	pGraphObject->Add2DBarGraphSegment(_T("9"));

	// Add graph series
	pGraphObject->Add2DBarGraphSeries(_T("Series 1"), RGB(128, 128, 128));

	// Set graph series values
	pGraphObject->Set2DBarGraphValue(1, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(2, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(3, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(4, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(5, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(6, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(7, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(8, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(9, 1, 0.0f);
	pGraphObject->Set2DBarGraphValue(10, 1, 0.0f);

	//pGraphObject->Set2DBarGraphValue(1, 2, 50);
	//pGraphObject->Set2DBarGraphValue(2, 2, 40);
	//pGraphObject->Set2DBarGraphValue(3, 2, 60);
	//pGraphObject->Set2DBarGraphValue(1, 3, 20);
	//pGraphObject->Set2DBarGraphValue(2, 3, 30);
	//pGraphObject->Set2DBarGraphValue(3, 3, 40);

	// Set graph animation
	pGraphObject->SetGraphAnimation(FALSE, AT_BAR_DRAW_ALL);

	// add graph to list
	m_mapGraphObjects[szNeuralNetwork] = pGraphObject;
}


void CDigitRecognitionTesterDlg::DrawBlock()
{
	CClientDC	dc(this);
	CString TEMP;
	CRect rect;
	GetDlgItem(IDC_DRAWING_REGION)->GetWindowRect(rect);
	ScreenToClient(rect);

	CPen newPen2(PS_SOLID, 2, RGB(0, 0, 0));
	dc.SelectObject(&newPen2);
	dc.Rectangle(rect.left, rect.top, rect.right, rect.bottom);
	

	//	TEMP.Format("%d",TotalWriteValue);
	//	dc.TextOut(330,10,TEMP);					// WRITE TO COUNT

	m_dGridWidth = rect.Width() / (double)BLOCK_X;		// Determine the area width
	m_dGridHeight = rect.Height() / (double)BLOCK_Y;

	CPen newPen(PS_SOLID, 1, RGB(120, 0, 0));

	dc.SelectObject(&newPen);

	// draw horizontal line
	for (int i = 1; i < BLOCK_Y; i++)
	{
		dc.MoveTo(rect.left, rect.top + (int)(i * m_dGridHeight));
		dc.LineTo(rect.right, rect.top + (int)(i * m_dGridHeight));
	}

	// draw vertical line
	for (int i = 1; i < BLOCK_X; i++)
	{
		dc.MoveTo(rect.left + (int)(i * m_dGridWidth), rect.top);
		dc.LineTo(rect.left + (int)(i * m_dGridWidth), rect.bottom);
	}

	for (int i = 0; i < BLOCK_X; i++) // column
	{
		for (int j = 0; j < BLOCK_Y; j++) // row
		{
			if (m_mtrxGridBlocks(j, i) == TRUE) {
				// if selected, highlight the block
				int x = rect.left + (int)(i * m_dGridWidth);
				int y = rect.top + (int)(j * m_dGridHeight);
				int cx = (int)((i + 1) * m_dGridWidth) - (int)(i * m_dGridWidth);
				int cy = (int)((j + 1) * m_dGridHeight) - (int)(j * m_dGridHeight);
				dc.FillSolidRect(x, y, cx, cy, RGB(0, 0, 0));
			}
		}
	}

}

void CDigitRecognitionTesterDlg::ResetGridBlocks()
{
	// Initialize Grid Blocks
	for (int j = 0; j < BLOCK_Y; j++) {
		for (int i = 0; i < BLOCK_X; i++) {
			m_mtrxGridBlocks(j, i) = FALSE;
		}
	}
}

void CDigitRecognitionTesterDlg::RedrawGridBlocks()
{
	// redraw Grid Blocks
	CRect rect;
	GetDlgItem(IDC_DRAWING_REGION)->GetWindowRect(rect);
	InvalidateRect(rect, FALSE);
}

BOOL CDigitRecognitionTesterDlg::LoadNeuralNetwork(CString &szNeuralNetworkPathName, CString& szNeuralNetworkFileName)
{
	FILE* fp = NULL;
	fopen_s(&fp, CStringWrapper(szNeuralNetworkPathName).getchardata(), "rt");
	if (fp == NULL) {
		AfxMessageBox(_T("Not able to open the selected file!!"));
		return FALSE; // error opening the file
	}

	// check if the pattern data is loaded already or not.
	if (m_cListPatterns.GetItemCount() > 0) {
		for (int i = 0; i < m_cListPatterns.GetItemCount(); i++) {
			CString szNNData = m_cListPatterns.GetItemText(i, NNDATA_NAME);
			if (szNeuralNetworkFileName == szNNData)
			{
				// the same pattern exists
				AfxMessageBox(_T("Not able to add the data. The same data exists already!"));
				return FALSE;
			}
		}
	}

	NN NeuralNetwork;	// init neural network
	NeuralNetwork.szNeuralNetwork = szNeuralNetworkFileName;
	// Initialize neural network 
	for (int i = 0; i < INPUT_NODE; i++) {
		vector<double> temp;
		for (int p = 0; p < HIDDEN_NODE; p++) {
			temp.push_back(0);	// Initialize Hidden weight
		}
		NeuralNetwork.dWeight_btwnInputHidden.push_back(temp);
	}
	for (int p = 0; p < HIDDEN_NODE; p++) {
		vector<double> temp;
		for (int q = 0; q < OUTPUT_NODE; q++) {
			temp.push_back(0);	// Initialize Hidden weight
		}
		NeuralNetwork.dWeight_btwnHiddenOutput.push_back(temp);
	}


	int tmp;
	fscanf_s(fp, "%d ", &tmp);	// Number of Pattern 
	fscanf_s(fp, "%d ", &tmp);   // Number of Hidden Layer Node
	fscanf_s(fp, "%d ", &tmp);	// Number of Output Layer Node
	
	float temp;
	// set weight in between input and hidden layers
	for (int i = 0; i < m_nNumInputNode; i++) {
		for (int p = 0; p < m_nNumHiddenNode; p++) {
			fscanf_s(fp, "%f ", &temp);
			NeuralNetwork.dWeight_btwnInputHidden[i][p] = temp;
		}
	}

	// set weight in between hidden and output layers
	for (int p = 0; p < m_nNumHiddenNode; p++) {
		for (int q = 0; q < m_nNumOutputNode; q++) {
			fscanf_s(fp, "%f ", &temp);
			NeuralNetwork.dWeight_btwnHiddenOutput[p][q] = temp;
		}
	}

	// set hidden layer nodes
	for (int p = 0; p < m_nNumHiddenNode; p++) {
		NeuralNetwork.dHiddenLayerNodes.push_back(0);
	}

	// set output layer nodes
	for (int q = 0; q < m_nNumOutputNode; q++) {
		NeuralNetwork.dOutputLayerNodes.push_back(0); // default zero
	}
	fclose(fp);
	NeuralNetwork.nCorrect = 0;
	NeuralNetwork.nIncorrect = 0;

	m_bDataLoaded = TRUE;
	UpdateData(FALSE);

	// store neural network data to memory
	m_vecNeuralNetworks.push_back(NeuralNetwork);


	// Add neural network data into list
	int nIndex(1);
	if (m_cListPatterns.GetItemCount() > 1)
		nIndex = m_cListPatterns.GetItemCount() - 1;

	// convert int to CString format
	int nPos = m_cListPatterns.InsertItem(nIndex, szNeuralNetworkFileName);
	m_cListPatterns.SetItemText(nPos, NNDATA_CORRECT, _T("0"));
	m_cListPatterns.SetItemText(nPos, NNDATA_INCORRECT, _T("0"));
	m_cListPatterns.SetItemText(nPos, NNDATA_ACCURACY, _T("0"));

	CreatePatternResultsGraphs(szNeuralNetworkFileName);

	return TRUE;
}

void CDigitRecognitionTesterDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//if (m_bDataLoaded == FALSE) {
	//	AfxMessageBox(_T("Please load neural network data first!"));
	//}
	//else {

	//	CRect rect;
	//	GetDlgItem(IDC_DRAWING_REGION)->GetWindowRect(rect);
	//	ScreenToClient(rect);

	//	if (rect.left < point.x && point.x < rect.right
	//		&& rect.top < point.y && point.y < rect.bottom) {
	//		// check if the user interaction is happening within the grid block.

	//		int column = (int)((point.x - rect.left) / m_dGridWidth);
	//		int row = (int)((point.y - rect.top) / m_dGridHeight);

	//		m_mtrxGridBlocks(row, column) = !m_mtrxGridBlocks(row, column);	// Enable or Disable Grid block
	//		m_bDrawingActivated ^= TRUE;	// Activate drawing

	//		RedrawGridBlocks();

	//		GetDlgItem(IDC_RUN_RECOGNITION)->EnableWindow(TRUE);

	//	}
	//}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CDigitRecognitionTesterDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrawingActivated = FALSE;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CDigitRecognitionTesterDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDrawingActivated) {
		CRect rect;
		GetDlgItem(IDC_DRAWING_REGION)->GetWindowRect(rect);
		ScreenToClient(rect);

		if (rect.left < point.x && point.x < rect.right
			&& rect.top < point.y && point.y < rect.bottom) {
			// check if the user interaction is happening within the grid block.

			int column = (int)((point.x - rect.left) / m_dGridWidth);
			int row = (int)((point.y - rect.top) / m_dGridHeight);

			if (m_mtrxGridBlocks(row, column) == FALSE) {
				// the selected block region needs to be activated. 
				m_mtrxGridBlocks(row, column) = TRUE;

				RedrawGridBlocks();
			}
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CDigitRecognitionTesterDlg::OnScreenClear()
{
	ResetGridBlocks(); // reset grid
	RedrawGridBlocks();

	m_szTestingOutput.Empty();
	//m_szHumanJudgement.Empty();
	UpdateData(FALSE);

	//GetDlgItem(IDC_RUN_RECOGNITION)->EnableWindow(FALSE);
}


void CDigitRecognitionTesterDlg::NNPreProcessingEachPattern(const DIGITPATTERN& inPattern, DIGITPATTERN& outPattern)
{
	int SmallX = -1, SmallY = -1;
	int LargeX = -1, LargeY = -1;
	CString temp, ctemp;
	Matrix mtrixTempBlocks, mtrixScaledBlocks;
	mtrixTempBlocks.SetSize(LBLOCK_Y, LBLOCK_X);
	mtrixScaledBlocks.SetSize(LBLOCK_Y, LBLOCK_X);

	// Initialize
	mtrixTempBlocks.Null();
	mtrixScaledBlocks.Null();


#ifdef DEBUG_SCREEN

	// Backup the original grid block information
	for (int row = 0; row < BLOCK_Y; row++) {
		for (int column = 0; column < BLOCK_X; column++) {
			m_mtrxGridBlocks(row, column) = inPattern[row * (int)(BLOCK_X)+column];
		}
	}
	Invalidate(FALSE);
	AfxMessageBox(_T("Current Pattern"));
#endif

	// Finding the boundary of the digit.
	// It determines the region.
	for (int row = 0; row < BLOCK_Y; row++) {	// row
		for (int column = 0; column < BLOCK_X; column++) {	// column
			int IdxPos = row * (int)(BLOCK_X)+column;
			if (inPattern[IdxPos] == TRUE) {
				if (SmallX == -1 && SmallY == -1) // default initialize
				{
					SmallX = column; SmallY = row; LargeX = column; LargeY = row;
				}
				else {
					if (SmallX > column) SmallX = column;
					if (SmallY > row) SmallY = row;
					if (LargeX < column) LargeX = column;
					if (LargeY < row) LargeY = row;
				}
			}
		}
	}

#ifdef DEBUG_SCREEN
	temp.Format(_T("SmallX : %d, SmallY: %d, LargeX : %d, LargeY : %d"),
		SmallX, SmallY, LargeX, LargeY);
	Invalidate(FALSE);
	AfxMessageBox(temp);
#endif

	// Move Pattern to (0,0) coordinate
	for (int row = SmallY; row <= LargeY; row++)
		for (int column = SmallX; column <= LargeX; column++) {
			int y_newPos = row - SmallY;
			int x_newPos = column - SmallX;

			if (x_newPos >= LBLOCK_X) x_newPos = LBLOCK_X - 1;
			if (x_newPos < 0) x_newPos = 0;
			if (y_newPos >= LBLOCK_Y) y_newPos = LBLOCK_Y - 1;
			if (y_newPos < 0) y_newPos = 0;

			int IdxPos = row * (int)(BLOCK_X)+column;
			mtrixTempBlocks(y_newPos, x_newPos) = inPattern[IdxPos];
		}


#ifdef DEBUG_SCREEN
	// Update the drawing
	for (int row = 0; row < BLOCK_Y; row++)
		for (int column = 0; column < BLOCK_X; column++) {
			m_mtrxGridBlocks(row, column) = mtrixTempBlocks(row, column);
		}

	Invalidate(FALSE);
	AfxMessageBox(_T("Pattern is moved to 0.0"));
#endif


	//////////////////////// Scaling 8 x 12 -> 16 x 24 ////////////////////////////////////
	double ratio = 0;
	double lvalue = 0; //remainder
	int sourcePos = 0, targetPos = 0;

	// Scaling horizontally
	ratio = LBLOCK_X / (double)(LargeX - SmallX + 1);
	for (int row = 0; row < LBLOCK_Y; row++) {
		targetPos = 0; sourcePos = 0;
		for (int column = 0; column < (LargeX - SmallX + 1); column++) {
			for (int z = 1; z <= ratio; z++) {
				mtrixScaledBlocks(row, targetPos++) = mtrixTempBlocks(row, sourcePos);
				if (targetPos >= LBLOCK_X) targetPos = LBLOCK_X - 1;
				lvalue += (ratio - (int)ratio);
			}
			if (lvalue > 1) {
				mtrixScaledBlocks(row, targetPos++) = mtrixTempBlocks(row, sourcePos);
				if (targetPos >= LBLOCK_X) targetPos = LBLOCK_X - 1;
				lvalue = (lvalue - (int)lvalue);
			}
			sourcePos++;
			if (sourcePos >= LBLOCK_X) sourcePos = LBLOCK_X - 1;
		}
	}


#ifdef DEBUG_SCREEN
	//FILE* fp = NULL;
	//fopen_s(&fp, "test.txt", "w+");
	//for (int row = 0; row < LBLOCK_Y; row++) {
	//	for (int column = 0; column < LBLOCK_X; column++) {
	//		if (mtrixScaledBlocks(row, column) == TRUE)
	//			fprintf(fp, "1 ");
	//		else
	//			fprintf(fp, "0 ");
	//	}
	//	fprintf(fp, "\n");
	//}
	//fclose(fp);

	for (int row = 0; row < BLOCK_Y; row++)
		for (int column = 0; column < BLOCK_X; column++)
			m_mtrxGridBlocks(row, column) = mtrixScaledBlocks(row, column);

	Invalidate(FALSE);
	AfxMessageBox(_T("X Scaled Pattern"));
	//return;
#endif

	for (int row = 0; row < LBLOCK_Y; row++)		// Copy Pattern to TempBlock
		for (int column = 0; column < LBLOCK_X; column++)
			mtrixTempBlocks(row, column) = mtrixScaledBlocks(row, column);

	targetPos = 0; sourcePos = 0; lvalue = 0;
	ratio = LBLOCK_Y / (float)(LargeY - SmallY + 1);
	for (int column = 0; column < LBLOCK_X; column++) {		// Scaling via Y
		targetPos = 0; sourcePos = 0;
		for (int row = 0; row < (LargeY - SmallY + 1); row++) {
			for (int z = 1; z <= ratio; z++) {
				mtrixScaledBlocks(targetPos++, column) = mtrixTempBlocks(sourcePos, column);
				if (sourcePos >= LBLOCK_Y) sourcePos = LBLOCK_Y - 1;
				if (targetPos >= LBLOCK_Y) targetPos = LBLOCK_Y - 1;
			}
			lvalue += (ratio - (int)ratio);
			if (lvalue > 1) {
				mtrixScaledBlocks(targetPos++, column) = mtrixTempBlocks(sourcePos, column);
				lvalue = (lvalue - (int)lvalue);
			}
			sourcePos++;
			if (sourcePos >= LBLOCK_Y) sourcePos = LBLOCK_Y - 1;
			if (targetPos >= LBLOCK_Y) targetPos = LBLOCK_Y - 1;
		}
	}

#ifdef DEBUG_SCREEN
	for (int row = 0; row < BLOCK_Y; row++)
		for (int column = 0; column < BLOCK_X; column++)
			m_mtrxGridBlocks(row, column) = mtrixScaledBlocks(row, column);

	Invalidate(FALSE);
	AfxMessageBox(_T("Y Scaled Pattern"));
#endif

	//SmallX = 0;	SmallY = 0;
	//LargeX = 0; LargeY = 0;
	//BOOL ValueBlocks[LBLOCK_Y][LBLOCK_X];
	//for (int row = 0; row < LBLOCK_Y; row++)		// Copy Pattern to TempBlock
	//	for (int column = 0; column < LBLOCK_X; column++) {
	//		mtrixTempBlocks(row, column) = mtrixScaledBlocks(row, column);
	//		ValueBlocks(row, column) = FALSE;
	//	}


	// duplicate patterns
	for (const auto& item : inPattern)
		outPattern.push_back(item);

	/////////////////////////// Collect Feature ///////////////////////////////
	int AddValue = 0;
	int srow = 0, scolumn = 0;	// small grid 
	for (int row = 0; row < LBLOCK_Y; row += 2) {			// Formalize 4 column 4 pattern
		for (int column = 0; column < LBLOCK_X; column += 2) {

			int newIdxPos = srow * (int)(BLOCK_X)+scolumn;
			AddValue = (int)mtrixScaledBlocks(row, column) + (int)mtrixScaledBlocks(row, column + 1)
				+ (int)mtrixScaledBlocks(row + 1, column) + (int)mtrixScaledBlocks(row + 1, column + 1);
			if (AddValue > 1)
				outPattern[newIdxPos] = TRUE;
			else
				outPattern[newIdxPos] = FALSE;

			AddValue = 0;
			scolumn++;
		}
		scolumn = 0;
		srow++;
	}

#ifdef DEBUG_SCREEN

	for (int row = 0; row < BLOCK_Y; row++) {
		for (int column = 0; column < BLOCK_X; column++) {
			m_mtrxGridBlocks(row, column) = outPattern[row * (int)(BLOCK_X)+column];
		}
	}

	Invalidate(FALSE);
	AfxMessageBox(_T("Mofied Pattern"));
#endif

}

void CDigitRecognitionTesterDlg::NNForwardComputation(const DIGITPATTERN& inputPattern,
	NN& NeuralNetwork, vector<double> &dOutputValues)
{
	double NET = 0.0;
	// Calculate Hidden Layer ////////////////////////
	for (int p = 0; p < m_nNumHiddenNode; p++)
	{
		NET = 0.0;
		for (int i = 0; i < m_nNumInputNode; i++)
			NET += inputPattern[i] * NeuralNetwork.dWeight_btwnInputHidden[i][p];
		NeuralNetwork.dHiddenLayerNodes[p] = sigmoid(NET);
	}

	// Calculate Output Layer ////////////////////////
	for (int q = 0; q < m_nNumOutputNode; q++)
	{
		NET = 0.0;
		for (int p = 0; p < m_nNumHiddenNode; p++)
			NET += NeuralNetwork.dHiddenLayerNodes[p] * NeuralNetwork.dWeight_btwnHiddenOutput[p][q];

		dOutputValues[q] = NET;
		NeuralNetwork.dOutputLayerNodes[q] = sigmoid(NET);
	}
}

void CDigitRecognitionTesterDlg::OnFileExit()
{
	PostMessage(WM_CLOSE);
}

void CDigitRecognitionTesterDlg::OnClose()
{

	m_vecNeuralNetworks.clear();

	CDialogEx::OnClose();
}

void CDigitRecognitionTesterDlg::OnBnClickedButtonLoadneuralnetwork()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("NeuralNetwork Files (*.net)|*.net|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString szNeuralNetworkPathName = fileDlg.GetPathName();
		CString szNeuralNetworkFileName = fileDlg.GetFileName();

		if (LoadNeuralNetwork(szNeuralNetworkPathName, szNeuralNetworkFileName)) {
			UpdateData(TRUE);
		}
	}
}


void CDigitRecognitionTesterDlg::OnFileLoadtestdataset()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY,
		_T("Digit Pattern Files (*.ptn)|*.ptn|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		m_mtmpTestdataPatterns.clear(); // remove all existing patterns
		m_mtrxGridBlocks.Null();
		for (int i = 0; i < BLOCK_X; i++) {
			for (int j = 0; j < BLOCK_Y; j++) {
				m_mtrxGridBlocks(j, i) = FALSE;
			}
		}

		CString szTestingDatasetFilePathName = fileDlg.GetPathName();
		m_szTestingDataset = fileDlg.GetFileName();
		UpdateData(FALSE);

		LoadTestingDataFile(szTestingDatasetFilePathName);

		GetDlgItem(IDC_BUTTON_RUNTESTING)->EnableWindow(TRUE);
	}
}


void CDigitRecognitionTesterDlg::OnClickedButtonLoadtestingdata()
{
	OnFileLoadtestdataset();
}


void CDigitRecognitionTesterDlg::OnBnClickedButtonRuntesting()
{
	if (m_vecNeuralNetworks.size() == 0) {
		AfxMessageBox(_T("No neural network data has been loaded. Please load it first!"));
		return;
	}
	m_cRunRecognition.SetRange(0, 100);
	m_cRunRecognition.SetPos(0);

	GetDlgItem(IDC_BUTTON_SAVERECOGNITIONRESULT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_LOADNEURALNETWORK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_LOADTESTINGDATA)->EnableWindow(FALSE);
	
	m_pRunTestingThread = (CThread*)::AfxBeginThread(RunTestingThread, this);

}

UINT CDigitRecognitionTesterDlg::RunTestingThread(LPVOID pParam)
{
	CDigitRecognitionTesterDlg* pDlg = (CDigitRecognitionTesterDlg*)pParam;

	CWaitCursor wait;

	auto compareWeight = [](DIGITWEIGHT i, DIGITWEIGHT j) {
		return (i.NNOutput > j.NNOutput);
	};

	int item = 0;
	for (auto& pattern : pDlg->m_mtmPreprocessedTestdataPatterns) {

		int nCorrectDigit = pattern.first;
		DIGITPATTERN preprocessPattern = pattern.second;

		int idx = 0;
		for (int row = 0; row < BLOCK_Y; row++) {
			for (int column = 0; column < BLOCK_X; column++) {
				if (preprocessPattern[idx++] == 1) {
					pDlg->m_mtrxGridBlocks(row, column) = TRUE;
				}
				else {
					pDlg->m_mtrxGridBlocks(row, column) = FALSE;
				}
			}
		}
		pDlg->RedrawGridBlocks();
		Sleep(pDlg->m_nRecognitionWaitingTerm);

		pDlg->m_szTestingOutput.Empty();
		for (auto& NeuralNetwork : pDlg->m_vecNeuralNetworks) {
			//if (NeuralNetwork.szNeuralNetwork == _T("group1.net"))
				//continue; // remove

			// Forward computation
			vector<double> dOutputValues(10);
			pDlg->NNForwardComputation(preprocessPattern, NeuralNetwork, dOutputValues);

			// Determine recognized digit by neural network
			DIGITWEIGHT DigitWeights[OUTPUT_NODE];

			int recognizedDigit = -1;
			double recognizedDigitOutput = 0.0;
			for (int digit = 0; digit < NeuralNetwork.dOutputLayerNodes.size(); digit++) {
				if (NeuralNetwork.dOutputLayerNodes[digit] > recognizedDigitOutput) {
					recognizedDigit = digit;
					recognizedDigitOutput = NeuralNetwork.dOutputLayerNodes[digit];
				}
				DigitWeights[digit].Digit = digit;
				DigitWeights[digit].NNOutput = NeuralNetwork.dOutputLayerNodes[digit];
			}

			if (recognizedDigit == nCorrectDigit) {
				NeuralNetwork.nCorrect++;
			}
			else {
				NeuralNetwork.nIncorrect++;
			}

			// find the position in the list
			for (int nPos = 0; nPos < pDlg->m_cListPatterns.GetItemCount(); nPos++) {
				CString szNNData = pDlg->m_cListPatterns.GetItemText(nPos, NNDATA_NAME);
				if (NeuralNetwork.szNeuralNetwork == szNNData) {
					CString Tmp;
					Tmp.Format(_T("%d"), NeuralNetwork.nCorrect);
					pDlg->m_cListPatterns.SetItemText(nPos, NNDATA_CORRECT, Tmp);
					Tmp.Format(_T("%d"), NeuralNetwork.nIncorrect);
					pDlg->m_cListPatterns.SetItemText(nPos, NNDATA_INCORRECT, Tmp);
					Tmp.Format(_T("%.2f%%"), NeuralNetwork.nCorrect / (double)(NeuralNetwork.nCorrect + NeuralNetwork.nIncorrect) * 100.);
					pDlg->m_cListPatterns.SetItemText(nPos, NNDATA_ACCURACY, Tmp);

					break;
				}
			}

			// Sorting
			int n = sizeof(DigitWeights) / sizeof(DigitWeights[0]);
			std::sort(DigitWeights, DigitWeights + n, compareWeight);

			pDlg->m_szTestingOutput += NeuralNetwork.szNeuralNetwork + _T("------\r\n");
			auto it = std::find_if(pDlg->m_mapGraphObjects.begin(), pDlg->m_mapGraphObjects.end(),
				[&](const std::pair<CString, CGraphObject*> item) { return item.first == NeuralNetwork.szNeuralNetwork;  });
			CGraphObject* pGraph = (*it).second;

			//std::string sText(CW2A(NeuralNetwork.szNeuralNetwork.GetString(), CP_UTF8));
			//std::cout << sText.c_str() << endl;
			for (int digit = 0; digit < 10; digit++) {
				CString temp;
				temp.Format(_T("DIGIT %d : %.4f"), DigitWeights[digit].Digit, DigitWeights[digit].NNOutput);
				temp += "\r\n";
				pDlg->m_szTestingOutput += temp;
			}

			pGraph->GetBarGraph()->emphasize_index = DigitWeights[0].Digit;
			if (recognizedDigit == nCorrectDigit)
				pGraph->GetBarGraph()->color_indicator = 1;
			else 
				pGraph->GetBarGraph()->color_indicator = -1;

			// compute Softmax - output probabilities
			double sum_digits_exp = 0;
			for (int digit = 0; digit < 10; digit++) {
				sum_digits_exp += exp(dOutputValues[digit]);
			}

			for (int digit = 0; digit < 10; digit++) {
				// update graph
				double prob = exp(dOutputValues[digit]) / sum_digits_exp;
				pGraph->Set2DBarGraphValue(digit + 1, 1, (float)prob);
				//std::cout << digit << " " << (float)prob << " " << NeuralNetwork.dOutputLayerNodes[digit] << endl;
			}


			pGraph->Invalidate(FALSE);

		}
		// display error rate
		pDlg->GetDlgItem(IDC_EDIT_TESTINGOUTPUT)->SetWindowText(pDlg->m_szTestingOutput);

		// update progress bar
		pDlg->m_cRunRecognition.SetPos((double)item / pDlg->m_mtmPreprocessedTestdataPatterns.size() * 100);

		item++;
	}
	pDlg->m_cRunRecognition.SetPos(100);
	pDlg->GetDlgItem(IDC_BUTTON_SAVERECOGNITIONRESULT)->EnableWindow(TRUE);
	pDlg->GetDlgItem(IDC_BUTTON_LOADNEURALNETWORK)->EnableWindow(TRUE);
	pDlg->GetDlgItem(IDC_BUTTON_LOADTESTINGDATA)->EnableWindow(TRUE);

	return 0;
}


void CDigitRecognitionTesterDlg::LoadTestingDataFile(CString& szfilePathName)
{
	// checking if the file can be opened.
	FILE* fp = NULL;
	fopen_s(&fp, CStringWrapper(szfilePathName).getchardata(), "r+");
	if (fp == NULL) {
		AfxMessageBox(_T("Not able to open the pattern file."));
		return; // error opening the file
	}
	fclose(fp);

	std::vector<std::string> dataset;
	std::string s = stdext::cvt::wstring_convert<cvt_t>().to_bytes(szfilePathName);
	int rows = CSVParser::LoadingCSVFile(s.c_str(), dataset);
	BOOL bDuplicatedPatternFound = FALSE;
	int rowidx = 0, colidx = 0;
	for (const auto& row : dataset) {
		std::vector<std::string> instance;
		CSVParser::csvline_populate_values(instance, row, ",");

		DIGITPATTERN digitpattern;
		int nDigit = atoi(instance[0].c_str());
		for (size_t i = 1; i < instance.size(); i++)
		{
			std::string s = instance[i];
			int nitem = atoi(s.c_str());
			digitpattern.push_back(nitem);
		}
		m_mtmpTestdataPatterns.insert(pair<int, DIGITPATTERN>(nDigit, digitpattern));

		// pre-processing pattern
		DIGITPATTERN PreProcessedPattern;
		NNPreProcessingEachPattern(digitpattern, PreProcessedPattern);
		m_mtmPreprocessedTestdataPatterns.insert(pair<int, DIGITPATTERN>(nDigit, PreProcessedPattern));
	}


	//FILE* fp = NULL;
	//fopen_s(&fp, CStringWrapper(szfilePathName).getchardata(), "r+");
	//if (fp == NULL) {
	//	AfxMessageBox(_T("Not able to open the pattern file."));
	//	return; // error opening the file
	//}

	//int temp, nTotalPatterns, nNumInputNode;
	//fscanf_s(fp, "%d ", &nTotalPatterns); // TOTAL DIGITPATTERN
	//fscanf_s(fp, "%d ", &nNumInputNode); // TOTAL INPUT NODE
	//for (int r = 0; r < nTotalPatterns; r++) {
	//	int digit;
	//	fscanf_s(fp, "%d ", &digit);	// DIGIT

	//	DIGITPATTERN digitpattern;
	//	for (int i = 0; i < m_nNumInputNode; i++) {
	//		fscanf_s(fp, "%d ", &temp);
	//		digitpattern.push_back(temp);
	//	}
	//	m_mtmpTestdataPatterns.insert(pair<int, DIGITPATTERN>(digit, digitpattern));

	//	// pre-processing pattern
	//	DIGITPATTERN PreProcessedPattern;
	//	NNPreProcessingEachPattern(digitpattern, PreProcessedPattern);
	//	m_mtmPreprocessedTestdataPatterns.insert(pair<int, DIGITPATTERN>(digit, PreProcessedPattern));
	//}
	//fclose(fp);

	UpdateData(FALSE);
}

void CDigitRecognitionTesterDlg::OnBnClickedButtonSaverecognitionresult()
{
	CFileDialog fileDlg(FALSE, _T(".nnlog"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Digit Pattern Files (*.nnlog)|*.nnlog|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString szNNLogFilePathName = fileDlg.GetPathName();

		SaveRecognitionResult(szNNLogFilePathName);
	}

}


void CDigitRecognitionTesterDlg::SaveRecognitionResult(CString& szfilePathName)
{
	FILE* fp = NULL;
	fopen_s(&fp, CStringWrapper(szfilePathName).getchardata(), "w+");
	if (fp == NULL) {
		AfxMessageBox(_T("Not able to create a pattern file."));
		return; // error opening the file
	}

	// find the position in the list
	for (int nPos = 0; nPos < m_cListPatterns.GetItemCount(); nPos++) {
		CString szNNData = m_cListPatterns.GetItemText(nPos, NNDATA_NAME);
		fprintf_s(fp, "%s\t", CStringWrapper(szNNData).getchardata());

		CString correct = m_cListPatterns.GetItemText(nPos, NNDATA_CORRECT);
		CString nIncorrect = m_cListPatterns.GetItemText(nPos, NNDATA_INCORRECT);
		CString accuracy = m_cListPatterns.GetItemText(nPos, NNDATA_ACCURACY);

		fprintf_s(fp, "%s\t", CStringWrapper(correct).getchardata());
		fprintf_s(fp, "%s\t", CStringWrapper(nIncorrect).getchardata());
		fprintf_s(fp, "%s\n", CStringWrapper(accuracy).getchardata());

	}

	fclose(fp);
}

BOOL CDigitRecognitionTesterDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE; // Do not process further (not close the dialog window)
		}

		int ctrlValue = ::GetKeyState(VK_CONTROL);
		if (ctrlValue)
		{
			switch (pMsg->wParam)
			{
			case VK_UP:
				m_nRecognitionWaitingTerm--;
				m_nRecognitionWaitingTerm = (m_nRecognitionWaitingTerm < 10) ? 10 : m_nRecognitionWaitingTerm;
				break;
			case VK_DOWN:
				m_nRecognitionWaitingTerm++;
				m_nRecognitionWaitingTerm = (m_nRecognitionWaitingTerm > 500) ? 500 : m_nRecognitionWaitingTerm;
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDigitRecognitionTesterDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	while (m_mapGraphObjects.size() > 0) {
		auto it = m_mapGraphObjects.begin();
		CGraphObject* pGraph = (*it).second;
		pGraph->DestroyWindow();
		delete pGraph;
		m_mapGraphObjects.erase(it);
	}
}


void CDigitRecognitionTesterDlg::OnKeydownListPatternsList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	

	if (pLVKeyDow->wVKey == VK_DELETE)
	{
		int row = m_cListPatterns.GetSelectionMark();
		if (row >= 0) {
			CString selectedItem = m_cListPatterns.GetItemText(row, 0);

			// find the position in the list
			auto it = std::find_if(m_vecNeuralNetworks.begin(), m_vecNeuralNetworks.end(), [&selectedItem](const NN& elem) { return elem.szNeuralNetwork == selectedItem; });
			if (it != m_vecNeuralNetworks.end()) {
				// remove the found element
				m_vecNeuralNetworks.erase(it);
				m_cListPatterns.DeleteItem(row);

				auto it = std::find_if(m_mapGraphObjects.begin(), m_mapGraphObjects.end(), [&selectedItem](const std::pair<CString, CGraphObject*>& t) { return t.first == selectedItem; });
				if (it != m_mapGraphObjects.end()) {
					CGraphObject* pGraph = (*it).second;
					pGraph->DestroyWindow();
					delete pGraph;
					m_mapGraphObjects.erase(it);
				}
			}
		}
	}

	*pResult = 0;
}


void CDigitRecognitionTesterDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


//void CDigitRecognitionTesterDlg::OnBnClickedButton1()
//{
//	cout << "TEST" << endl;
//	auto it = std::find_if(m_mapGraphObjects.begin(), m_mapGraphObjects.end(),
//		[&](const std::pair<CString, CGraphObject*> item) { return item.first == _T("group2.net");  });
//	CGraphObject* pGraph = (*it).second;
//
//	pGraph->Set2DBarGraphValue(1, 1, 2);
//	pGraph->Set2DBarGraphValue(2, 1, 7);
//	pGraph->Set2DBarGraphValue(3, 1, 3);
//	pGraph->Set2DBarGraphValue(4, 1, 2);
//	pGraph->Set2DBarGraphValue(5, 1, 6);
//	pGraph->Set2DBarGraphValue(6, 1, 2);
//	pGraph->Set2DBarGraphValue(7, 1, 1);
//	pGraph->Set2DBarGraphValue(8, 1, 1);
//	pGraph->Set2DBarGraphValue(9, 1, 8);
//	pGraph->Set2DBarGraphValue(10, 1, 5);
//
//	pGraph->Invalidate(FALSE);
//}
