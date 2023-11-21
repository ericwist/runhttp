//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef StringTokenizerH
#define StringTokenizerH

#ifndef _WIN32_WCE // [
#include <iostream>

using namespace std;
#endif // ]

#include "WServer.h"

// using namespace std;

class WSERVER_API StringTokenizer
{
public:
    /**
     * Constructs a string tokenizer for the specified string. All
     * characters in the <code>delim</code> argument are the delimiters
     * for separating tokens.
     * <p>
     * If the <code>returnDelims</code> flag is <code>true</code>, then
     * the delimiter characters are also returned as tokens. Each
     * delimiter is returned as a string of length one. If the flag is
     * <code>false</code>, the delimiter characters are skipped and only
     * serve as separators between tokens.
     *
     * @param   str            char&, a reference to the char string to be parsed.
     * @param   delim          char&, reference to the char delimiters.
     * @param   returnDelims   bool, flag indicating whether to return the delimiters
     *                         as tokens.
     */
    StringTokenizer(const char& str, const char& delim, bool returnDelims);

    /**
     * Constructs a string tokenizer for the specified string. The
     * characters in the <code>delim</code> argument are the delimiters
     * for separating tokens. Delimiter characters themselves will not
     * be treated as tokens.
     *
     * @param   str     char&, a reference to the char string to be parsed.
     * @param   delim   char&, reference to the char delimiters.
     */
    StringTokenizer(const char& str, const char& delim);

    /**
     * Constructs a string tokenizer for the specified string. The
     * tokenizer uses the default delimiter set, which is
     * <code>"&nbsp;&#92;t&#92;n&#92;r&#92;f"</code>: the space character,
     * the tab character, the newline character, the carriage-return character,
     * and the form-feed character. Delimiter characters themselves will
     * not be treated as tokens.
     *
     * @param   str   char& a reference to the char string to be parsed.
     */
    StringTokenizer(const char& str);

    ~StringTokenizer();

    /**
     * Tests if there are more tokens available from this tokenizer's string.
     * If this method returns <tt>true</tt>, then a subsequent call to
     * <tt>nextToken</tt> with no argument will successfully return a token.
     *
     * @return  <code>true</code> if and only if there is at least one token
     *          in the string after the current position; <code>false</code>
     *          otherwise.
     */
    bool hasMoreTokens();

    /**
     * Returns the next token from this string tokenizer.
     *
     * @return     const char *, pointer to the next token from this string tokenizer.
     *					or NULL if there are no more tokens. If NON NULL, the caller
     *					must delete the char string pointer.
     */
    char* nextToken();

    /**
     * Returns the next token in this string tokenizer's string. First,
     * the set of characters considered to be delimiters by this
     * <tt>StringTokenizer</tt> object is changed to be the characters in
     * the string <tt>delim</tt>. Then the next token in the string
     * after the current position is returned. The current position is
     * advanced beyond the recognized token.  The new delimiter set
     * remains the default after this call.
     *
     * @param      delim   const char&, refrence to the new delimiters.
     * @return     const char *, pointer to the next token from this string tokenizer.
     *					or NULL if there are no more tokens. If NON NULL, the caller
     *					must delete the char string pointer.
     */
    char* StringTokenizer::nextToken(const char& delim);

    /**
     * Calculates the number of times that this tokenizer's
     * <code>nextToken</code> method can be called before it generates an
     * exception. The current position is not advanced.
     *
     * @return  the number of tokens remaining in the string using the current
     *          delimiter set.
     * @see     java.util.StringTokenizer#nextToken()
     */
    int countTokens();

    /**
     * Reset so that if tokens exist, hasMoreTokens() will return TRUE, and nextToken()
     * will return the first token.
     */
    void reset();

private:
    int currentPosition;
    int newPosition;
    int maxPosition;
    bool retDelims;
    bool delimsChanged;
    const char& str;
    char* delimiters;

    /**
     * maxDelimChar stores the value of the delimiter character with the
     * highest value. It is used to optimize the detection of delimiter
     * characters.
     */
    char maxDelimChar;

    /**
     * Set maxDelimChar to the highest char in the delimiter set.
     */
    void setMaxDelimChar();

    /**
     * Skips delimiters starting from the specified position. If retDelims
     * is false, returns the index of the first non-delimiter character at or
     * after startPos. If retDelims is true, startPos is returned.
     */
    int skipDelimiters(int startPos);

    /**
     * Skips ahead from startPos and returns the index of the next delimiter
     * character encountered, or maxPosition if no such delimiter is found.
     */
    int scanToken(int startPos);
};

//---------------------------------------------------------------------------
#endif
