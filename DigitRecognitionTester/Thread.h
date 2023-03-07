#if !defined(AFX_THREAD_H__18CCBE22_D55F_479B_9D52_A5EEB403892E__INCLUDED_)
#define AFX_THREAD_H__18CCBE22_D55F_479B_9D52_A5EEB403892E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Thread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CThread thread

class CThread : public CWinThread
{
	DECLARE_DYNCREATE(CThread)
protected:
	CThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	BOOL ExitThread();

private:
	BOOL KillThread();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CThread();

	// Generated message map functions
	//{{AFX_MSG(CThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THREAD_H__18CCBE22_D55F_479B_9D52_A5EEB403892E__INCLUDED_)
