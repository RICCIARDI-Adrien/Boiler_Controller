/** @file Page_Index.c
 * Generate the "index.html" page. See Pages.h for description.
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
int PageIndex(struct MHD_Connection *Pointer_Connection, char *Pointer_String_Response)
{
	int Day_Temperature, Night_Temperature, Has_Error_Occurred = 0, Is_Boiler_Running, Outside_Temperature, Radiator_Start_Water_Temperature, Target_Radiator_Start_Water_Temperature;
	const char *Pointer_String_Argument_Value;
	
	// Extract values from the URL (all values must always be present)
	// Power state
	Pointer_String_Argument_Value = MHD_lookup_connection_value(Pointer_Connection, MHD_GET_ARGUMENT_KIND, "power_state");
	if (Pointer_String_Argument_Value == NULL) goto Read_Board_Values;
	if ((sscanf(Pointer_String_Argument_Value, "%d", &Is_Boiler_Running) != 1) || (Is_Boiler_Running < 0) || (Is_Boiler_Running > 1))
	{
		syslog(LOG_ERR, "Bad 'power_state' argument value (%s), no data will be sent to the board.", Pointer_String_Argument_Value);
		goto Read_Board_Values;
	}
	
	// Day temperature
	Pointer_String_Argument_Value = MHD_lookup_connection_value(Pointer_Connection, MHD_GET_ARGUMENT_KIND, "day_temperature");
	if (Pointer_String_Argument_Value == NULL) goto Read_Board_Values;
	if ((sscanf(Pointer_String_Argument_Value, "%d", &Day_Temperature) != 1) || (Day_Temperature < CONFIGURATION_TEMPERATURE_MINIMUM_VALUE) || (Day_Temperature > CONFIGURATION_TEMPERATURE_MAXIMUM_VALUE))
	{
		syslog(LOG_ERR, "Bad 'day_temperature' argument value (%s), no data will be sent to the board.", Pointer_String_Argument_Value);
		goto Read_Board_Values;
	}
	
	// Night temperature
	Pointer_String_Argument_Value = MHD_lookup_connection_value(Pointer_Connection, MHD_GET_ARGUMENT_KIND, "night_temperature");
	if (Pointer_String_Argument_Value == NULL) goto Read_Board_Values;
	if ((sscanf(Pointer_String_Argument_Value, "%d", &Night_Temperature) != 1) || (Night_Temperature < CONFIGURATION_TEMPERATURE_MINIMUM_VALUE) || (Night_Temperature > CONFIGURATION_TEMPERATURE_MAXIMUM_VALUE))
	{
		syslog(LOG_ERR, "Bad 'night_temperature' argument value (%s), no data will be sent to the board.", Pointer_String_Argument_Value);
		goto Read_Board_Values;
	}
	
	// Set new values
	// Power mode
	if (BoilerSetBoilerRunningMode(Is_Boiler_Running) != 0)
	{
		syslog(LOG_ERR, "Failed to set boiler running mode.");
		Has_Error_Occurred = 1;
	}
	// Desired temperatures
	if (BoilerSetDesiredRoomTemperatures(Day_Temperature, Night_Temperature) != 0)
	{
		syslog(LOG_ERR, "Failed to set desired room temperatures.");
		Has_Error_Occurred = 1;
	}
	
Read_Board_Values:
	// Read all needed values from the board
	// Power mode
	if (BoilerGetBoilerRunningMode(&Is_Boiler_Running) != 0)
	{
		syslog(LOG_ERR, "Failed to read boiler running mode from board.");
		Has_Error_Occurred = 1;
	}
	// Desired temperatures
	if (BoilerGetDesiredRoomTemperatures(&Day_Temperature, &Night_Temperature) != 0)
	{
		syslog(LOG_ERR, "Failed to read desired temperatures from board.");
		Has_Error_Occurred = 1;
	}
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
	
	// Generate the right page
	if (Has_Error_Occurred) strcpy(Pointer_String_Response,
		"<html>\n"
		"	<head>\n"
		"		<title>Chaudi&egrave;re</title>\n"
		"		<meta charset=\"utf-8\" />\n"
		"	</head>\n"
		"\n"
		"	<body>\n"
		"		<center>\n"
		"		<h1>Chaudi&egrave;re</h1>\n"
		"\n"
		"		<p><b>Erreur de communication avec la carte. Veuillez recharger la page.</b></p>\n"
		"	</body>\n"
		"</html>\n");
	else sprintf(Pointer_String_Response,
		"<html>\n"
		"	<head>\n"
		"		<title>Chaudi&egrave;re</title>\n"
		"		<meta charset=\"utf-8\" />\n"
		"		<meta http-equiv=\"refresh\" content=\"1\">\n"
		"	</head>\n"
		"\n"
		"	<body>\n"
		"		<center>\n"
		"		<h1>Chaudi&egrave;re</h1>\n"
		"\n"
		"		<form action=\"index.html\">\n"
		"			<p>\n"
		"				<input type=\"radio\" name=\"power_state\" value=\"1\" %s> Activ&eacute;e <input type=\"radio\" name=\"power_state\" value=\"0\" %s> Veille\n"
		"			</p>\n"
		"			<table>\n"
		"			<tr>\n"
		"				<td>Jour</td>\n"
		"				<td><input type=\"range\" min=\"" PAGES_CONVERT_MACRO_VALUE_TO_STRING(CONFIGURATION_TEMPERATURE_MINIMUM_VALUE) "\" max=\"" PAGES_CONVERT_MACRO_VALUE_TO_STRING(CONFIGURATION_TEMPERATURE_MAXIMUM_VALUE) "\" step=\"1\" name=\"day_temperature\" onchange=\"updateDesiredDayTemperature()\" id=\"id_day_temperature\" value=\"%d\"></td>\n"
		"				<td id=\"id_desired_day_temperature\">%d&deg;C</td>\n"
		"			</tr>\n"
		"			<tr>\n"
		"				<td>Nuit</td>\n"
		"				<td><input type=\"range\" min=\"" PAGES_CONVERT_MACRO_VALUE_TO_STRING(CONFIGURATION_TEMPERATURE_MINIMUM_VALUE) "\" max=\"" PAGES_CONVERT_MACRO_VALUE_TO_STRING(CONFIGURATION_TEMPERATURE_MAXIMUM_VALUE) "\" step=\"1\" name=\"night_temperature\" onchange=\"updateDesiredNightTemperature()\" id=\"id_night_temperature\" value=\"%d\"></td>\n"
		"				<td id=\"id_desired_night_temperature\">%d&deg;C</td>\n"
		"			</tr>\n"
		"			</table>\n"
		"\n"
		"			<p>\n"
		"				<input type=\"submit\" value=\"Valider\" />\n"
		"			</p>\n"
		"		</form>\n"
		"\n"
		"		<table >\n"
		"		<tr>\n"
		"			<td>Temp&eacute;rature ext&eacute;rieure :</td>\n"
		"			<td>%d°C</td>\n"
		"		</tr>\n"
		"		<tr>\n"
		"			<td>Temp&eacute;rature de d&eacute;part :</td>\n"
		"			<td>%d°C</td>\n"
		"		</tr>\n"
		"		<tr>\n"
		"			<td>Temp&eacute;rature d'eau sortie chaudi&egrave;re :</td>\n"
		"			<td>%d°C</td>\n"
		"		</tr>\n"
		"		</table>\n"
		"		</center>\n"
		"\n"
		"		<script>\n"
		"			function updateDesiredDayTemperature()\n"
		"			{\n"
		"				var temperature = document.getElementById(\"id_day_temperature\").value;\n"
		"				temperature = temperature.concat(\"&deg;C\");\n"
		"				document.getElementById(\"id_desired_day_temperature\").innerHTML = temperature;\n"
		"			}\n"
		"\n"
		"			function updateDesiredNightTemperature()\n"
		"			{\n"
		"				var temperature = document.getElementById(\"id_night_temperature\").value;\n"
		"				temperature = temperature.concat(\"&deg;C\");\n"
		"				document.getElementById(\"id_desired_night_temperature\").innerHTML = temperature;\n"
		"			}\n"
		"		</script>\n"
		"	</body>\n"
		"</html>\n", Is_Boiler_Running ? "checked" : "", Is_Boiler_Running ? "" : "checked", Day_Temperature, Day_Temperature, Night_Temperature, Night_Temperature, Outside_Temperature, Radiator_Start_Water_Temperature, Target_Radiator_Start_Water_Temperature);
	
	return 0;
}
