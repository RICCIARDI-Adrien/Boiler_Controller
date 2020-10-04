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
	int Has_Error_Occurred = 0, Heating_Curve_Coefficient, Heating_Curve_Parallel_Shift;
	
	// Read heating curve current parameters
	if (BoilerGetHeatingCurveParameters(&Heating_Curve_Coefficient, &Heating_Curve_Parallel_Shift) != 0)
	{
		syslog(LOG_ERR, "Failed to read heating curve parameters from board in settings page.");
		Has_Error_Occurred = 1;
	}
	
	// Generate the right page
	if (Has_Error_Occurred) strcpy(Pointer_String_Response,
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
		"		<p><b>Erreur de communication avec la carte. Veuillez recharger la page.</b></p>\n"
		"	</body>\n"
		"</html>\n");
	else sprintf(Pointer_String_Response,
		"<html>\n"
		"	<head>\n"
		"		<title>Chaudi&egrave;re - Configuration</title>\n"
		"		<meta charset=\"utf-8\" />\n"
		"	</head>\n"
		"\n"
		"	<body>\n"
		"		<h1>Configuration de la courbe de chauffe</h1>\n"
		"\n"
		"		<h3>Param&egrave;tres de la courbe actuellement utilis&eacute;e</h2>\n"
		"		<p>\n"
		"			Coefficient : %0.1f<br />\n"
		"			D&eacute;placement parall&egrave;le : %d\n"
		"		</p>\n"
		"\n"
		"		<h3>D&eacute;finir une nouvelle courbe</h3>\n"
		"		<form action=\"settings.html\">\n"
		"			<input type=\"radio\" id=\"0\" name=\"heating_curve\" value=\"0\" onClick=\"enableSubmitButton()\">\n"
		"			<label for=\"0\">Mi-saison (coefficient : 1.4, d&eacute;placement parall&egrave;le : 15)</label><br>\n"
		"			<input type=\"radio\" id=\"1\" name=\"heating_curve\" value=\"1\" onClick=\"enableSubmitButton()\">\n"
		"			<label for=\"0\">Hiver (coefficient : 1.8, d&eacute;placement parall&egrave;le : 20)</label><br>\n"
		"\n"
		"			<p>\n"
		"				<input id=\"id_submit_button\" type=\"submit\" value=\"Valider\" disabled/>\n"
		"			</p>\n"
		"		</form>\n"
		"\n"
		"		<center>\n"
		"			<p>\n"
		"				<a href=\"/index.html\">Retour</a>\n"
		"			</p>\n"
		"		</center>\n"
		"\n"
		"		<script>\n"
		"			function enableSubmitButton()\n"
		"			{\n"
		"				document.getElementById(\"id_submit_button\").disabled = false;\n"
		"			}\n"
		"		</script>\n"
		"	</body>\n"
		"</html>\n", Heating_Curve_Coefficient / 10.f, Heating_Curve_Parallel_Shift / 10);
	
	return 0;
}
