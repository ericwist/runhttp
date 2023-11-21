/**
 * This class generates a unique 10+ character id used for a session Identifier.
 * Author: Eric Wistrand 11/12/2023
 */

#include "SessionIDGenerator.h"
#include "Platform.h"
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32_WCE // [
#include <iostream>

using namespace std;
#endif // ]

// MAX_RADIX is 36
const long SessionIdGenerator::MAX_RADIX = 36;

/*
 * we want to have a random string with a length of
 * 6 characters. Since we encode it BASE 36, we've to
 * modulo it with the following value:
 */
const long SessionIdGenerator::maxRandomLen = 2176782336L; // 36 ** 6

/*
 * The session identifier must be unique within the typical lifespan
 * of a Session, the value can roll over after that. 3 characters:
 * (this means a roll over after over an day which is much larger
 *  than a typical lifespan)
 */
const long SessionIdGenerator::maxSessionLifespanTics = 46656; // 36 ** 3

/*
 *  millisecons between different tics. So this means that the
 *  3-character time string has a new value every 2 seconds:
 */
const long SessionIdGenerator::ticDifference = 2000;

/*
 * Create a suitable string for session identification
 * Use synchronized count and time to ensure uniqueness.
 * Use random string to ensure timestamp cannot be guessed
 * by programmed attack.
 *
 * format of id is <6 chars random><3 chars time><1+ char count>
 */
int SessionIdGenerator::session_count = 0;
long SessionIdGenerator::lastTimeVal = 0;

SessionIdGenerator::SessionIdGenerator()
{
    generateMutex = Platform::makeMutex();
    srand(Platform::getCurrentMillis());
}

SessionIdGenerator::~SessionIdGenerator()
{
    Platform::closeMutex(generateMutex);
}

/**
 * Generate a unique session ID. Optionally append the supplied appendID (if appendID is NON NULL).
 * This function is re-entrant as it is protected by a Mutex so it can be called by multiple threads.
 * @param appendID - const char *, if NON NULL, append this to the generated ID.
 * @ret char * - A pointer to the newly allocated session char *. Callers have the responsibility
 *	of deleting this pointer.
 */
char* SessionIdGenerator::generateId(const char* appendID)
{
    Platform::lockMutex(generateMutex);

    char buf[100];

    // random value ..
    unsigned long n = rand();

#ifndef _WIN32_WCE // [
    // ZZZ warning C4146: unary minus operator applied to unsigned type, result still unsigned.
    if (n < 0)
        n = -n;
#endif // ]

    // D(cout << "generateId() 0: n= " << n << endl;)
    n *= Platform::getCurrentMillis();

    // D(cout << "generateId() 1: n= " << n << endl;)
    /**/
    n %= maxRandomLen;

    // D(cout << "generateId() 2: n= " << n << endl;)

    // add maxLen to pad the leading characters with '0'; remove
    // first digit with substring.
    n += maxRandomLen;
    /**/

    /*
        n += Platform::getCurrentMillis();
        //D(cout << "generateId() 2: n= " << n << endl;)

        n %= maxRandomLen;
    /**/
    // D(cout << "generateId() 3: n= " << n << endl;)

    _itoa(n, buf, MAX_RADIX);

    // D(cout << "generateId() 4: buf= " << buf << endl;)

    // Remove the first digit.
    strcpy(buf, buf + 1);

    // D(cout << "generateId() 5: buf= " << buf << endl;)

    long timeVal = (Platform::getCurrentMillis() / ticDifference);

    // D(cout << "generateId() 6: timeVal= " << timeVal << endl;)

    // cut..
    timeVal %= maxSessionLifespanTics;

    // D(cout << "generateId() 7: timeVal= " << timeVal << endl;)

    // padding, see above
    timeVal += maxSessionLifespanTics;

    // D(cout << "generateId() 8: timeVal= " << timeVal << endl;)

    int len = strlen(buf);
    _itoa(timeVal, &buf[len], MAX_RADIX);

    // D(cout << "generateId() 9: buf= " << buf << endl;)

    // Remove the first digit.
    strcpy(&buf[len], &buf[len + 1]);

    // D(cout << "generateId() 10: buf= " << buf << endl;)

    /*
     * make the string unique: append the session count since last
     * time flip.
     */
    // count sessions only within tics. So the 'real' session count
    // isn't exposed to the public ..
    if (lastTimeVal != timeVal)
    {
        lastTimeVal = timeVal;
        session_count = 0;
    }

    len = strlen(buf);
    _itoa(++session_count, &buf[len], MAX_RADIX);

    // D(cout << "generateId() 11: buf= " << buf << endl;)

    if (appendID && *appendID)
    {
        len = sizeof(buf) - strlen(buf) - 1;
        strncat(buf, appendID, len);
        buf[sizeof(buf) - 1] = 0;
    }

    char* str = new char[strlen(buf) + 1];
    strcpy(str, buf);

    D(cout << "generateId() Returning: " << str << endl;)

    Platform::unlockMutex(generateMutex);
    return str;
}
