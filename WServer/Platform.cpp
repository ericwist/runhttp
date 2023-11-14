/**
 * This class supplies platform specific functionality.
 * Author: Eric Wistrand 11/12/2023
 */

#include "Platform.h"

#if	WIN32 | _WIN32_WCE	// [
#include <windows.h>
#include <stdlib.h>

#ifndef	_WIN32_WCE	// [
#include <iostream>

using namespace std;
#endif				// ]

/** @name Timing Methods */
//@{
/** Gets the system time in milliseconds
  *
  * This must be implemented by the per-platform driver, which should
  * use local services to return the current value of a running
  * millisecond timer. Note that the value returned is only as accurate
  * as the millisecond time of the underyling host system.
  *
  * @return Returns the system time as an unsigned long
  */
unsigned long Platform::getCurrentMillis()
{
	return (unsigned long)::GetTickCount();
}
//@}

/** @name Mutex Methods */
//@{
/** Make a new mutex
  *
  * Each per-platform driver must implement this. Only it knows what
  * the actual content of the passed mutex handle is. The returned
  * handle pointer will be eventually passed to closeMutex() which is
  * also implemented by the platform driver.
  */
void * Platform::makeMutex()
{
	CRITICAL_SECTION* newCS = new CRITICAL_SECTION;
	InitializeCriticalSection(newCS);
	return newCS;
}

/** Closes a mutex handle
  *
  * Each per-platform driver must implement this. Only it knows what
  * the actual content of the passed mutex handle is.
  *
  * @param mtxHandle The mutex handle that you want to close
  */
void Platform::closeMutex(void * const mtxHandle)
{
	::DeleteCriticalSection((LPCRITICAL_SECTION)mtxHandle);
	delete mtxHandle;
}

/** Locks a mutex handle
  *
  * Each per-platform driver must implement this. Only it knows what
  * the actual content of the passed mutex handle is.
  *
  * @param mtxHandle The mutex handle that you want to lock
  */
void Platform::lockMutex(void * const mtxHandle)
{
	//D(cout << "Platform::lockMutex() locking mutex= " << mtxHandle << endl)
	::EnterCriticalSection((LPCRITICAL_SECTION)mtxHandle);
}

/** Unlocks a mutex
  *
  * Each per-platform driver must implement this. Only it knows what
  * the actual content of the passed mutex handle is.
  *
  * Note that, since the underlying system synchronization services
  * are used, Xerces cannot guarantee that lock/unlock operaitons are
  * correctly enforced on a per-thread basis or that incorrect nesting
  * of lock/unlock operations will be caught.
  *
  * @param mtxGandle The mutex handle that you want to unlock
  */
void Platform::unlockMutex(void * const mtxHandle)
{
	::LeaveCriticalSection((LPCRITICAL_SECTION)mtxHandle);
	//D(cout << "Platform::unlockMutex() unlocking mutex= " << mtxHandle << endl);
}
//@}
#if 0
#include "StdNew.cpp"
#endif
#endif			// ]
