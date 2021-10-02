/**
* @file scanner.c
 */

#include "scanner.h"

#include "string.h"
#include <stdio.h>

#define cmp(s1, s2) strcmp(s1, s2)


// Definition of all possible scanner states
#define SCANNER_STATE_START 11                      // Starting state, scanner comes back after every token

#define SCANNER_STATE_KEYWORD_IDENTIFIER            // State for a possible identifier or reserved keyword

// States for scanning correct number notations
#define SCANNER_STATE_NUMBER 21                     // State for reading integer digits
#define SCANNER_STATE_DECIMAL_POINT 22              // State for decimal point
#define SCANNER_STATE_DECIMAL 23                    // State for decimal digits
#define SCANNER_STATE_EXPONENT 24                   // State for "e" or "E"
#define SCANNER_STATE_EXPONENT_SIGN 25              // State for exponent sign (+/-)
#define SCANNER_STATE_EXPONENT_VALUE 26             // State for exponent value, ONLY integer exponent value is accepted

// States for comment scanning
// In inline comment, everything is ignored till EOL
// In block comment, everything is ignored till double closing square brackets
#define SCANNER_STATE_COMMENT_DASH_1 31             // State for the first dash at the beginning of commentary
#define SCANNER_STATE_COMMENT_DASH_2 32             // State for the second dash at the beginning of commentary
#define SCANNER_STATE_BLOCK_COMMENT_BRACKET_1 33    // State for the first square opening bracket
#define SCANNER_STATE_BLOCK_COMMENT 34              // State for insides of block comment to ignore
#define SCANNER_STATE_CLOSING_BRACKET 35            // State for the first closing bracket of block comment

// States for string scanning
#define SCANNER_STATE_STRING 41                     //
#define SCANNER_STATE_ESCAPE_CHAR_SEQ 42            //
#define SCANNER_STATE_ESCAPE_SEQ_1 43               //
#define SCANNER_STATE_ESCAPE_SEQ_2 44               //

/*
#define SCANNER_STATE_                              //
#define SCANNER_STATE_                              //
#define SCANNER_STATE_                              //
#define SCANNER_STATE_                              //
#define SCANNER_STATE_                              //
#define SCANNER_STATE_                              //
*/

FILE *fptr;
string_t *str;

/*int main() {
    // printf("%d", cmp("a", "a"));
    // printf("%d", cmp("a", "b"));

    return 0;
}*/


/**
 * What is it doing?
 *
 * @param int
 * @return what is returned
 */
int initialise_file_ptr(FILE *source_file) {
    fptr = source_file;
}

/**
 * What is it doing?
 *
 * @param int
 * @return what is returned
 */
int free_str(string_t *s, int error_number) {
    // In case of NULL pointer, nothing is returned anyway
    str_free(s);

    return error_number;
}


/**
 * What is it doing?
 *
 * @param int
 * @return what is returned
 */
int find_keyword(token_t *t) {
    switch
}

/**
 * What is it doing?
 *
 * @param int Pointer to something
 * @param int number of something
 * @return what is returned
 */
int get_next_token() {

    // Checks if the file to be read is present
    if(!fptr) {
        return -1;
    }

    // Initialises a null terminated empty string
    if(!str_create_empty(str)) {
        return -1;
    }

    while(true) {

    }


    return 0;
}

