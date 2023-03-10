// 2DBarGraph.h: interface for the C2DBarGraph class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXWIN_H__
#error "include 'pch.h' before including this file for PCH"
#endif

#define AT_BAR_DRAW_ALL			20
#define AT_BAR_DRAW_SEGMENT		21
#define AT_BAR_DRAW_SERIES		22
#define AT_BAR_BLEND_SEGMENT	25
#define AT_BAR_BLEND_SERIES		26


struct _2DBarGraphValues
{
	int index;
	float value;
	struct _2DBarGraphValues* next;
};

struct _2DBarGraphSeries
{
	int index;
	COLORREF color;
	CString text;
	struct _2DBarGraphValues* values;
	struct _2DBarGraphSeries* next;
};

struct _2DBarGraphSegments
{
	int index;
	CString text;
	struct _2DBarGraphSegments* next;
};

class C2DBarGraph  
{
public:
	COLORREF GetLegendTextColor();
	void SetLegendTextColor( COLORREF t_color );
	COLORREF GetLegendBackgroundColor();
	void SetLegendBackgroundColor( COLORREF l_bgcolor );
	COLORREF GetLabelsColor();
	void SetLabelsColor( COLORREF l_color );
	void SetFullSize( CSize full_size );
	void UpdateAnimation();
	void SetGraphAnimation( BOOL g_animation, int a_type );
	void DeleteSeries( int s_series );
	void AddSeries( CString s_text, COLORREF s_color );
	void SetValue( int s_segment, int s_series, float s_value );
	void DeleteSegment( int s_index );
	void AddSegment( CString s_text );
	void SetGraphSize( CSize g_size );
	void SetGraphPosition( CPoint g_position );
	void SetGraphBkColor( COLORREF g_bkColor );
	int GetGraphAnimationPercent();
	BOOL GetGraphAnimation();
	void BuildGraphLegend( HDC hDC );
	void PaintGraph( HDC hDC );
	void ClearGraph();
	C2DBarGraph( CPoint g_position, CSize g_size );
	virtual ~C2DBarGraph();

	int emphasize_index;
	int color_indicator; // 1: red / -1: blue / 0: gray

private:
	COLORREF m_LegendTextColor;
	COLORREF m_LegendBackgroundColor;
	COLORREF m_LabelsColor;
	int m_BlendSeriesCompleted;
	int m_BlendSegmentsCompleted;
	HBITMAP m_hOldBlendBitmap;
	HBITMAP m_hBlendBitmap;
	HDC m_hBlendDC;
	BOOL m_AnimationPauseStart;
	int m_AnimationPause;
	int m_DrawSeriesCompleted;
	int m_DrawSegmentsCompleted;
	int m_AnimationType;
	int m_AnimationPercent;
	BOOL m_Animation;
	COLORREF m_GraphBkColor;
	CFont* m_ValueFont;
	CFont* m_SegmentFont;
	CFont* m_LegendFont;
	int m_SeriesNumber;
	int m_SegmentsNumber;
	CSize m_FullSize;
	CSize m_Size;
	CPoint m_Position;
	struct _2DBarGraphSeries* m_Series;
	struct _2DBarGraphSegments* m_Segments;
};
