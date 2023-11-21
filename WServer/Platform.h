// Author: Eric Wistrand 11/12/2023
#ifndef PlatformH
#define PlatformH

#include "WServer.h"

/**
 * This class supplies platform specific functionality.
 */
class WSERVER_API Platform
{
public:
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
    static unsigned long getCurrentMillis();
    //@}

    /** @name Mutex Methods */
    //@{
    /** Closes a mutex handle
     *
     * Each per-platform driver must implement this. Only it knows what
     * the actual content of the passed mutex handle is.
     *
     * @param mtxHandle The mutex handle that you want to close
     */
    static void closeMutex(void* const mtxHandle);

    /** Locks a mutex handle
     *
     * Each per-platform driver must implement this. Only it knows what
     * the actual content of the passed mutex handle is.
     *
     * @param mtxHandle The mutex handle that you want to lock
     */
    static void lockMutex(void* const mtxHandle);

    /** Make a new mutex
     *
     * Each per-platform driver must implement this. Only it knows what
     * the actual content of the passed mutex handle is. The returned
     * handle pointer will be eventually passed to closeMutex() which is
     * also implemented by the platform driver.
     */
    static void* makeMutex();

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
    static void unlockMutex(void* const mtxHandle);
    //@}
};

#endif