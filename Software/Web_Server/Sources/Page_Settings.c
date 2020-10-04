/** @file Page_Settings.c
 * Generate the settings page. See Pages.h for description.
 * @author Adrien RICCIARDI
 */
#include <Boiler.h>
#include <Configuration.h>
#include <Pages.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int PageSettings(struct MHD_Connection *Pointer_Connection, char *Pointer_String_Response)
{
	strcpy(Pointer_String_Response,
		"<html>\n"
		"	<head>\n"
		"		<title>Chaudi&egrave;re - Configuration</title>\n"
		"		<meta charset=\"utf-8\" />\n"
		"	</head>\n"
		"\n"
		"	<body>\n"
		"		<center>\n"
		"		<h1>Configuration de la courbe de chauffe</h1>\n"
		"\n"
		"		<p><b>TEST</p>\n"
		"	</body>\n"
		"</html>\n");
	
	return 0;
}
