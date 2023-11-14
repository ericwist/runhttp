//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
//#pragma hdrstop

#ifdef	_WIN32_WCE	// [
#include <windows.h>
#endif				// ]

#include "StringTokenizer.h"

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
StringTokenizer::StringTokenizer(const char& str, const char& delim, bool returnDelims) :
	currentPosition(0), newPosition(-1), delimsChanged(false), str(str),
    /*delimiters(delim),*/ retDelims(returnDelims)
{
	delimiters = new char[strlen(&delim)+1];
	strcpy(delimiters,&delim);

	maxPosition = strlen(&str);
	setMaxDelimChar();
}


/**
 * Constructs a string tokenizer for the specified string. The
 * characters in the <code>delim</code> argument are the delimiters
 * for separating tokens. Delimiter characters themselves will not
 * be treated as tokens.
 *
 * @param   str     char&, a reference to the char string to be parsed.
 * @param   delim   char&, reference to the char delimiters.
 */
StringTokenizer::StringTokenizer(const char& str, const char& delim) :
	currentPosition(0), newPosition(-1), delimsChanged(false), str(str),
    /*delimiters(delim),*/ retDelims(false)
{
//	StringTokenizer(str,delim,false);

	delimiters = new char[strlen(&delim)+1];
	strcpy(delimiters,&delim);

	maxPosition = strlen(&str);
	setMaxDelimChar();

}


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
StringTokenizer::StringTokenizer(const char& str) : str(str)
{
	//StringTokenizer(str, *" \t\n\r\f", false);

	char * delim = " \t\n\r\f";

	delimiters = new char[strlen(delim)+1];
	strcpy(delimiters,delim);

	maxPosition = strlen(&str);
	setMaxDelimChar();
}

StringTokenizer::~StringTokenizer()
{
/*
	if ( str )
    	delete str;
*/
	if ( delimiters )
    	delete[] delimiters;
}

/**
 * Tests if there are more tokens available from this tokenizer's string.
 * If this method returns <tt>true</tt>, then a subsequent call to
 * <tt>nextToken</tt> with no argument will successfully return a token.
 *
 * @return  <code>true</code> if and only if there is at least one token
 *          in the string after the current position; <code>false</code>
 *          otherwise.
 */
bool StringTokenizer::hasMoreTokens()
{
	/*
	 * Temporary store this position and use it in the following
	 * nextToken() method only if the delimiters have'nt been changed in
	 * that nextToken() invocation.
	 */
	newPosition = skipDelimiters(currentPosition);
	return (newPosition < maxPosition);
}

/**
 * Returns the next token from this string tokenizer.
 *
 * @return     const char *, pointer to the next token from this string tokenizer.
 * or NULL if there are no more tokens. If NON NULL, the caller must delete the char string pointer.
 */
char * StringTokenizer::nextToken()
{
	/*
	 * If next position already computed in hasMoreElements() and
	 * delimiters have changed between the computation and this invocation,
	 * then use the computed value.
	 */
	currentPosition = (newPosition >= 0 && !delimsChanged) ?
		newPosition : skipDelimiters(currentPosition);

	/* Reset these anyway */
	delimsChanged = false;
	newPosition = -1;

	if (currentPosition >= maxPosition)
		return NULL;;

	int start = currentPosition;

	currentPosition = scanToken(currentPosition);

	char	* tstr;
	int		  size = currentPosition-start;

	if ( size > 0 )
	{
		tstr = new char[size+1];
		strncpy(tstr,&(&str)[start],size);
		tstr[size] = 0;

		D(cout << endl << "()()()()()()()" << endl << "size= " << size << " currentPosition= " <<
			currentPosition << " start= " << start << " tstr= '" << tstr << "'" << endl;)
	}
	else
		tstr = NULL;

    return tstr;
}

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
 *					or NULL if there are no more tokens.
 */
char * StringTokenizer::nextToken(const char& delim)
{
	if ( delimiters )
		delete[] delimiters;

	delimiters = new char[strlen(&delim)+1];
	strcpy(delimiters,&delim);

	/* delimiter string specified, so set the appropriate flag. */
	delimsChanged = true;

	setMaxDelimChar();
	return nextToken();
}

/**
 * Calculates the number of times that this tokenizer's
 * <code>nextToken</code> method can be called before it generates an
 * exception. The current position is not advanced.
 *
 * @return  the number of tokens remaining in the string using the current
 *          delimiter set.
 * @see     java.util.StringTokenizer#nextToken()
 */
int StringTokenizer::countTokens()
{
	int count = 0;
	int currpos = currentPosition;

	while (currpos < maxPosition)
	{
		currpos = skipDelimiters(currpos);

		if (currpos >= maxPosition)
			break;

		currpos = scanToken(currpos);
		count++;
	}

	return count;
}


/**
 * Reset so that if tokens exist, hasMoreTokens() will return TRUE, and nextToken()
 * will return the first token.
 */
void StringTokenizer::reset()
{
	currentPosition = 0;
	newPosition = -1;
	delimsChanged = false;
}


/**
 * Set maxDelimChar to the highest char in the delimiter set.
 */
void StringTokenizer::setMaxDelimChar()
{
	int	length;

	if ( !delimiters || ((length = strlen(delimiters)) == 0) )
	{
		maxDelimChar = 0;
		return;
	}

	char m = 0;

	for (int i = 0; i < length; i++)
	{
        char c = delimiters[i];

		if (m < c)
			m = c;
	}

	maxDelimChar = m;
}

/**
 * Skips delimiters starting from the specified position. If retDelims
 * is false, returns the index of the first non-delimiter character at or
 * after startPos. If retDelims is true, startPos is returned.
 * Returns -1 upon error;
 */
int StringTokenizer::skipDelimiters(int startPos)
{
	if (!delimiters || (strlen(delimiters) == 0) )
		return -1;

	int position = startPos;

	while (!retDelims && position < maxPosition)
	{
        char c = (&str)[position];

		if ( (c > maxDelimChar) || !strchr(delimiters,c) )
			break;

		position++;
	}

	return position;
}

/**
 * Skips ahead from startPos and returns the index of the next delimiter
 * character encountered, or maxPosition if no such delimiter is found.
 */
int StringTokenizer::scanToken(int startPos)
{
	int position = startPos;

	while (position < maxPosition)
	{
        char c = (&str)[position];

		if ( (c <= maxDelimChar) && strchr(delimiters,c) )
			break;

		position++;
	}

	if (retDelims && (startPos == position))
	{
        char c = (&str)[position];

		if ( (c <= maxDelimChar) && strchr(delimiters,c) )
			position++;
	}

	return position;
}



//---------------------------------------------------------------------------
//#pragma package(smart_init)
