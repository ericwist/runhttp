// Author: Eric Wistrand 11/12/2023
#ifndef SessionIdGeneratorH
#define SessionIdGeneratorH


#include "WServer.h"


/**
 * This class generates a unique 10+ character id used for a session Identifier.
 */
class WSERVER_API SessionIdGenerator
{
public:

	SessionIdGenerator();
	~SessionIdGenerator();

    // MAX_RADIX is 36
	static const long MAX_RADIX;

    /*
     * we want to have a random string with a length of
     * 6 characters. Since we encode it BASE 36, we've to
     * modulo it with the following value:
     */
    static const long maxRandomLen;	// = 2176782336L; // 36 ** 6

    /*
     * The session identifier must be unique within the typical lifespan
     * of a Session, the value can roll over after that. 3 characters:
     * (this means a roll over after over an day which is much larger
     *  than a typical lifespan)
     */
    static const long maxSessionLifespanTics;	// = 46656; // 36 ** 3

    /*
     *  millisecons between different tics. So this means that the
     *  3-character time string has a new value every 2 seconds:
     */
    static const long ticDifference;	// = 2000;

	/**
	 * Generate a unique session ID. Optionally append the supplied appendID (if appendID is NON NULL).
	 * This function is re-entrant as it is protected by a Mutex so it can be called by multiple threads.
	 * @param appendID - const char *, if NON NULL, append this to the generated ID.
	 * @ret char * - A pointer to the newly allocated session char *. Callers have the responsibility
	 *	of deleting this pointer.
	 */
	char * generateId(const char * appendID);

private:

    /*
     * Create a suitable string for session identification
     * Use synchronized count and time to ensure uniqueness.
     * Use random string to ensure timestamp cannot be guessed
     * by programmed attack.
     *
     * format of id is <6 chars random><3 chars time><1+ char count>
     */
    static int session_count;
    static long lastTimeVal;

	/**
	 * Platform specific Mutez object.
	 */
	void * generateMutex;
};

#endif