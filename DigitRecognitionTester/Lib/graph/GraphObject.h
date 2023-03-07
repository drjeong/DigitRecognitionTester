#pragma once
#include <afxwin.h>

// GraphObject.h : header file

#include "2DPieGraph.h"
#include "2DBarGraph.h"
#include "2DLineGraph.h"
//

#define GT_2DBAR		0
#define	GT_2DPIE		1
#define GT_2DLINE		2

#define IDT_ANIMATION	50000
#define ANIMATION_TIME	50

#define GB_SOLID		10
#define GB_GRADIENT		11
#define FT_HORIZONTAL	20
#define FT_VERTICAL		21

#define MARGIN_RATIO_LEFT 0.1
#define MARGIN_RATIO_TOP 0.95

#define MARGIN_LEFT 10
#define MARGIN_BOTTOM 5

/////////////////////////////////////////////////////////////////////////////
// CGraphObject window

class CGraphObject : public CWnd
{
	// Construction
public:
	CGraphObject();

	// Attributes
public:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CGraphObject)
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:

	C2DBarGraph* GetBarGraph() { return m_2DBarGraph;}

	void ShowGraphLegend(BOOL legend);
	COLORREF GetGraphLegendTextColor();
	void SetGraphLegendTextColor(COLORREF t_color);
	COLORREF GetGraphLegendBackgroundColor();
	void SetGraphLegendBackgroundColor(COLORREF l_bgcolor);
	COLORREF GetGraphLabelsColor();
	void SetGraphLabelsColor(COLORREF l_color);
	COLORREF GetGraphSubtitleColor();
	COLORREF GetGraphTitleColor();
	void SetGraphSubtitleColor(COLORREF s_color);
	void SetGraphTitleColor(COLORREF t_color);
	BOOL GetGraphSubtitleShadow();
	BOOL GetGraphTitleShadow();
	void SetGraphSubtitleShadow(BOOL s_shadow);
	void SetGraphTitleShadow(BOOL t_shadow);
	int GetGradientFillType();
	void SetGradientFillType(int f_type);
	void SetGraphGradientColors(COLORREF g_col1, COLORREF g_col2);
	int GetGraphFillType();
	void SetGraphFillType(int g_fill);
	void Delete2DLineGraphSeries(int s_index);
	void Add2DLineGraphSeries(CString s_text, COLORREF s_color);
	void Set2DLineGraphValue(int s_segment, int s_series, int s_value);
	void Delete2DLineGraphSegment(int s_index);
	void Add2DLineGraphSegment(CString s_text);
	void Detele2DBarGraphSeries(int s_index);
	void Add2DBarGraphSeries(CString s_text, COLORREF s_color);
	void Set2DBarGraphValue(int s_segment, int s_series, float s_value);
	void Delete2DBarGraphSegment(int s_index);
	void Add2DBarGraphSegment(CString s_text);
	void SetGraphPosition(CPoint g_position); // left top
	void SetGraphSize(CSize g_size);
	void Update2DPieGraphSegment(int s_index, int s_percent, COLORREF s_color, CString s_text);
	void SetGraphSubtitle(CString g_subtitle);
	void SetGraphTitle(CString g_title);
	COLORREF GetGraphBackgroundColor();
	void SetGraphBackgroundColor(COLORREF g_bgcolor);
	BOOL GetGraphAnimation();
	void SetGraphAnimation(BOOL g_animation, int a_type);
	void Delete2DPieGraphSegment(int s_index);
	void Add2DPieGraphSegment(int s_percent, COLORREF s_color, CString s_text);
	void CreateGraph(int g_type);
	virtual ~CGraphObject();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphObject)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_ShowLegend;
	COLORREF m_GraphSubtitleColor;
	COLORREF m_GraphTitleColor;
	BOOL m_GraphSubtitleShadow;
	BOOL m_GraphTitleShadow;
	DWORD m_GraphGradientFillType;
	COLORREF m_GraphGradientColor2;
	COLORREF m_GraphGradientColor1;
	int m_GraphFillType;
	BOOL m_GraphAnimation;
	CFont* m_pGraphSubtitleFont;
	CString m_GraphSubtitle;
	CBitmap* m_pGradientBitmap;
	CDC* m_pGradientDC;
	CBitmap* m_pGraphBitmap;
	CDC* m_pGraphDC;
	BOOL m_Update;
	CFont* m_pGraphTitleFont;
	CString m_GraphTitle;
	COLORREF m_GraphBackgroundColor;
	int m_GraphType;
	CSize m_Size;	// width, height
	CPoint m_Position;	// left, top
	C2DLineGraph* m_2DLineGraph;
	C2DBarGraph* m_2DBarGraph;
	C2DPieGraph* m_2DPieGraph;
};

