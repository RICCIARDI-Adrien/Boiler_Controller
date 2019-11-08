/** @file Pages.h
 * List all website pages.
 * @author Adrien RICCIARDI
 */
#ifndef H_PAGES_H
#define H_PAGES_H

#include <microhttpd.h>

//-------------------------------------------------------------------------------------------------
// Constants and macros
//-------------------------------------------------------------------------------------------------
/** Convert the macro identifier to a C string. */
#define PAGES_CONVERT_MACRO_NAME_TO_STRING(X) #X
/** Convert the macro value to a C string. The preprocessor needs two passes to do the conversion, so the MAIN_CONVERT_MACRO_NAME_TO_STRING() is needed. */
#define PAGES_CONVERT_MACRO_VALUE_TO_STRING(X) PAGES_CONVERT_MACRO_NAME_TO_STRING(X)

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Create the "index.html" page response.
 * @param Pointer_Connection The connection object.
 * @param Pointer_String_Response On output, contain the HTML page code.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
int PageIndex(struct MHD_Connection *Pointer_Connection, char *Pointer_String_Response);

#endif
