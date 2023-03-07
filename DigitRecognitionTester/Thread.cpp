// Thread.cpp : implementation file
//

#include "pch.h"
#include "Thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThread

IMPLEMENT_DYNCREATE(CThread, CWinThread)

CThread::CThread()
{
}

CThread::~CThread()
{
}

BOOL CThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread, CWinThread)
	//{{AFX_MSG_MAP(CThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThread message handlers

BOOL CThread::ExitThread() 
{	
	TRY
    {
		if (!KillThread())
		{	// Fail - Thread Finish
			DWORD dwExitCode;
			GetExitCodeThread(m_hThread, &dwExitCode);
			
			if (dwExitCode == STILL_ACTIVE)
				if (!TerminateThread(m_hThread, 0))	// Terminate Thread
					delete this;	// Fail - Kill Thread
		}
	}
    CATCH(CUserException, e)
    {
        // Validation failed - user already alerted, setfocus()
        // back to the original control.
   } 
	END_CATCH
	return TRUE;
}

BOOL CThread::KillThread() 
{	
	BOOL nReturn = TRUE;
	TRY
    {
		int nCount = 0;
		DWORD dwExitCode;
		if (m_hThread != NULL && GetExitCodeThread(m_hThread, &dwExitCode) &&
		dwExitCode == STILL_ACTIVE)
		{
			// Kill thread by setting the "kill thread" event.
			if (WaitForSingleObject(m_hThread, 5000L) == WAIT_TIMEOUT)
				TerminateThread(m_hThread,1);
		}
	}
    CATCH(CUserException, e)
    {
		nReturn = FALSE;
        // Validation failed - user already alerted, setfocus()
        // back to the original control.
   } 
	END_CATCH
	return nReturn;
}

//	INFO: Managing Thread Termination
//	http://support.microsoft.com/default.aspx?scid=kb%3ben-us%3b254956
//	This article was previously published under Q254956 
//	SUMMARY
//	A multithreaded application may need to terminate long running threads. This should be done by using application constructs to allow for graceful termination.
//
//	Threads that are waiting on kernel-mode objects can be stopped only by using the TerminateThread function. This is the only function that satisfies any wait condition for the thread. Because the terminated thread is not allowed any clean up, the TerminateThread function should be used only when there is no other alternative.
//
//	The best alternative is to plan out a strategy such that wait conditions can be ended. This can be accomplished by using extra synchronization objects that are used to exit the wait condition, or by only using alertable waits and user-mode asynchronous procedure calls (APCs). 
//	MORE INFORMATION
//	Application constructs for terminating a thread could include suspending the thread execution and changing its thread context to a clean-up routine, and then resuming the thread. The clean-up routine could do some clean up and cause termination of the thread. Another approach is to induce an exception to occur and have a high-level exception handler clean up. This approach is discussed in part in a Win32 Q&A article by Jeffrey Richter in the Microsoft System Journal, March, 1996. 
//
//	The disadvantage of any schemes that depend on suspending thread execution and changing the thread context is that the new thread context does not take effect until the thread is scheduled to be run. This means that if the thread is in a wait condition for a kernel-mode synchronization object, such as an event, mutex, socket, file object, process object, thread object, and soon, it will not change to the new thread context until the wait condition has been satisfied. 
//
//	Threads can be terminated at any time even while waiting in kernel mode by using the TerminateThread function. The TerminateThread function causes an APC to be queued for that thread (similar to the other schemes already discussed) and then forces any and all wait conditions for that thread to be satisfied. Satisfying all wait condition causes the thread to become ready to run. At the appropriate time, the schedule picks the thread to be scheduled for execution. 
//
//	Because the thread has a queued APC as soon as the thread is scheduled to run, this APC is dequeued and executed first. In that case, the APC runs the ExitThread function on that thread. This causes the thread to stop execution. 
//
//	The disadvantage of using the TerminateThread function is that the thread to be terminated has no recourse to clean up its state. Specifically, you should consider the following leakage of thread state: 
//	Owned mutexes are considered abandoned but this can be detected.
//	Other synchronization objects that the thread acquired are now left unreleased. For example, critical sections that were entered by the terminated thread are not released.
//	If the target thread was executing certain kernel32 calls when it is terminated, the kernel32 state for the thread's process could be inconsistent.
//	If the target thread is manipulating the global state of a shared DLL, the state of the DLL could be inconsistent, affecting other users of the DLL.
//	The stack memory that is used by the thread is lost and not recovered until the process ends. A long-running process must not do this because eventually all its address space will be used. A few of these terminations a day over the course of a few months will cause this problem.
//	Because of all these issues with state leakage and corruption, it is not a good idea to use the TerminateThread function. 
//
//	The best approach to terminate threads gracefully is either to use a wait condition which includes extra wait events that can always be satisfied by a controlling thread, or to use alertable waits: 
//	Sleep calls:
//
//	Replace Sleep function calls with a WaitForSingleObject function with a controlling object (probably an event) specified as the object to wait for and the sleep time as the timeout. The controlling thread ends the sleep early by signaling the controlling object. This would indicate to the thread that something needs to be done (cleanup).
//	WaitForSingleObject:
//
//	Replace the single object wait to a wait for two objects--the original object and an extra object (probably an event) that can control ending the wait. The WaitForMultipleObjects function would be used, specifying FALSE for fWaitAll, which would allow a controlling thread to set to event to end the wait. The thread code would detect that this event was signaled and would know that it needs to clean up.
//	WaitForMultipleObjects:
//
//	This is straight forward if the wait was for any single object to become signaled (fWaitAll was FALSE). Just add the control event as in the case of the single-object wait. For a multiple-object wait, there is no easy answer because the WaitForMultipleObjects function cannot be asked to wait for one set of objects to become signaled or another.
//
//	Jeffrey Richter wrote a column in Microsoft Systems Journal (January, 1997) that discusses an approach of using alertable waits to create an API that allows you to accomplish this.
//	It might be easier to use alertable waits in that you do not need to add an event. This method also affords extra flexibility in that any extra work can be queued up for the waiting thread. To use this method, use the alertable version of the synchronization functions (WaitForSingleObjectEx, WaitForMultipleObjectsEx, MsgWaitForMultipleObjectsEx, SignalObjectAndWait, SleepEx), and specify that the thread is alertable (bAltertable set to TRUE). The controlling thread now can specify that the thread should perform cleanup by queuing a user-mode APC by using the QueueUserAPC function. 
//	REFERENCES
//	Inside Windows NT (2nd Edition) discusses APC and the effect of APC processing on wait conditions (pp. 92-94). 
//
//	Windows NT Device Driver Development (1st Edition) discusses the scheduling of APC (pp. 126-127). 
//
//	Microsoft Systems Journal January 1997 Win32 Q&A discusses the implementation of WaitForExpression, which allows the waiting for any multiple set of objects to become signaled. It shows how to use an alertable wait to accomplish this. 
//
