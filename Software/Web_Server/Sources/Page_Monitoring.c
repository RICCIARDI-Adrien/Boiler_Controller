/** @file Page_Monitoring.c
 * Generate the monitoring page. See Pages.h for description.
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
int PageMonitoring(struct MHD_Connection *Pointer_Connection, char *Pointer_String_Response)
{
	int Outside_Temperature, Radiator_Start_Water_Temperature, Target_Radiator_Start_Water_Temperature, Heating_Curve_Coefficient, Heating_Curve_Parallel_Shift, Has_Error_Occurred = 0;
	
	// Read all needed values from the board
	// Sensor temperatures
	if (BoilerGetSensorsCelsiusTemperatures(&Outside_Temperature, &Radiator_Start_Water_Temperature) != 0)
	{
		syslog(LOG_ERR, "Failed to read sensor temperatures from board.");
		Has_Error_Occurred = 1;
	}
	// Target radiator start water temperature
	if (BoilerGetTargetRadiatorStartWaterTemperature(&Target_Radiator_Start_Water_Temperature) != 0)
	{
		syslog(LOG_ERR, "Failed to read target radiator start water temperature from board.");
		Has_Error_Occurred = 1;
	}
	// Heating curve parameters
	if (BoilerGetHeatingCurveParameters(&Heating_Curve_Coefficient, &Heating_Curve_Parallel_Shift) != 0)
	{
		syslog(LOG_ERR, "Failed to read heating curve parameters from board.");
		Has_Error_Occurred = 1;
	}
	
	// Generate the right page
	if (Has_Error_Occurred) strcpy(Pointer_String_Response,
		"<html>\n"
		"	<head>\n"
		"		<title>Chaudi&egrave;re - Monitoring</title>\n"
		"		<meta charset=\"utf-8\" />\n"
		"	</head>\n"
		"\n"
		"	<body>\n"
		"		<center>\n"
		"		<h1>Monitoring des capteurs</h1>\n"
		"\n"
		"		<p><b>Erreur de communication avec la carte. Veuillez recharger la page.</b></p>\n"
		"	</body>\n"
		"</html>\n");
	else sprintf(Pointer_String_Response,
		"<html>\n"
		"	<head>\n"
		"		<title>Chaudi&egrave;re - Monitoring</title>\n"
		"		<meta charset=\"utf-8\" />\n"
		"	</head>\n"
		"\n"
		"	<body>\n"
		"		<center>\n"
		"		<h1>Monitoring des capteurs</h1>\n"
		"\n"
		"		<p><b>TEST</b></p>\n"
		"	</body>\n"
		"</html>\n");
	
	return 0;
}
