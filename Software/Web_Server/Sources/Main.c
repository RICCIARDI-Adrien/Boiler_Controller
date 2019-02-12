/** @file Main.c
 * An HTTP front-end for the boiler controller board.
 * @author Adrien RICCIARDI
 */
#include <Boiler.h>
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

//-------------------------------------------------------------------------------------------------
// Private constants and macros
//-------------------------------------------------------------------------------------------------
/** Minimum allowed day or night temperature in Celsius degrees. */
#define MAIN_TEMPERATURE_MINIMUM_VALUE 15
/** Maximum allowed day or night temperature in Celsius degrees. */
#define MAIN_TEMPERATURE_MAXIMUM_VALUE 25

/** Convert the macro identifier to a C string. */
#define MAIN_CONVERT_MACRO_NAME_TO_STRING(X) #X
/** Convert the macro value to a C string. The preprocessor needs two passes to do the conversion, so the MAIN_CONVERT_MACRO_NAME_TO_STRING() is needed. */
#define MAIN_CONVERT_MACRO_VALUE_TO_STRING(X) MAIN_CONVERT_MACRO_NAME_TO_STRING(X)

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** Store the HTML page to send as response. */
static char Main_String_Response[10 * 1024];

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Create the "index.html" page response.
 * @param Pointer_Connection The connection object.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MainPrepareIndexPageResponse(struct MHD_Connection *Pointer_Connection)
{
	int Day_Temperature, Night_Temperature, Has_Error_Occurred = 0, Is_Boiler_Running;
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
	if ((sscanf(Pointer_String_Argument_Value, "%d", &Day_Temperature) != 1) || (Day_Temperature < MAIN_TEMPERATURE_MINIMUM_VALUE) || (Day_Temperature > MAIN_TEMPERATURE_MAXIMUM_VALUE))
	{
		syslog(LOG_ERR, "Bad 'day_temperature' argument value (%s), no data will be sent to the board.", Pointer_String_Argument_Value);
		goto Read_Board_Values;
	}
	
	// Night temperature
	Pointer_String_Argument_Value = MHD_lookup_connection_value(Pointer_Connection, MHD_GET_ARGUMENT_KIND, "night_temperature");
	if (Pointer_String_Argument_Value == NULL) goto Read_Board_Values;
	if ((sscanf(Pointer_String_Argument_Value, "%d", &Night_Temperature) != 1) || (Night_Temperature < MAIN_TEMPERATURE_MINIMUM_VALUE) || (Night_Temperature > MAIN_TEMPERATURE_MAXIMUM_VALUE))
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
	
	// Generate the right page
	if (Has_Error_Occurred) strcpy(Main_String_Response,
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
	else sprintf(Main_String_Response,
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
		"		<form action=\"index.html\">\n"
		"			<p>\n"
		"				<input type=\"radio\" name=\"power_state\" value=\"1\" %s> Activ&eacute;e <input type=\"radio\" name=\"power_state\" value=\"0\" %s> Veille\n"
		"			</p>\n"
		"			<table>\n"
		"			<tr>\n"
		"				<td>Jour</td>\n"
		"				<td><input type=\"range\" min=\"" MAIN_CONVERT_MACRO_VALUE_TO_STRING(MAIN_TEMPERATURE_MINIMUM_VALUE) "\" max=\"" MAIN_CONVERT_MACRO_VALUE_TO_STRING(MAIN_TEMPERATURE_MAXIMUM_VALUE) "\" step=\"1\" name=\"day_temperature\" onchange=\"updateDesiredDayTemperature()\" id=\"id_day_temperature\" value=\"%d\"></td>\n"
		"				<td id=\"id_desired_day_temperature\">%d&deg;C</td>\n"
		"			</tr>\n"
		"			<tr>\n"
		"				<td>Nuit</td>\n"
		"				<td><input type=\"range\" min=\"" MAIN_CONVERT_MACRO_VALUE_TO_STRING(MAIN_TEMPERATURE_MINIMUM_VALUE) "\" max=\"" MAIN_CONVERT_MACRO_VALUE_TO_STRING(MAIN_TEMPERATURE_MAXIMUM_VALUE) "\" step=\"1\" name=\"night_temperature\" onchange=\"updateDesiredNightTemperature()\" id=\"id_night_temperature\" value=\"%d\"></td>\n"
		"				<td id=\"id_desired_night_temperature\">%d&deg;C</td>\n"
		"			</tr>\n"
		"			</table>\n"
		"\n"
		"			<p>\n"
		"				<input type=\"submit\" value=\"Valider\" />\n"
		"			</p>\n"
		"		</form>\n"
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
		"</html>\n", Is_Boiler_Running ? "checked" : "", Is_Boiler_Running ? "" : "checked", Day_Temperature, Day_Temperature, Night_Temperature, Night_Temperature);
	
	return 0;
}

/** Called when a client requests a web page.
 * @param Pointer_Custom_Data Custom data provided to MHD_start_daemon().
 * @param Pointer_Connection The connection handle used to build the response.
 * @param Pointer_String_URL The URL requested by the client.
 * @param Pointer_String_Method The HTTP method (GET, POST...) used to send the query.
 * @param Pointer_String_Version HTTP protocol version.
 * @param Pointer_String_Upload_Data The data to upload to POST request.
 * @param Pointer_Upload_Data_Size How many bytes of data to upload to a POST request.
 * @param Pointer_Persistent_Connection_Custom_Data A custom data pointer that won't change across callback calls (for the same connection).
 * @return MHD_NO to close the connection,
 * @return MHD_YES to continue servicing the client request.
 */
static int MainWebServerAccessHandlerCallback(void __attribute__((unused)) *Pointer_Custom_Data, struct MHD_Connection *Pointer_Connection, const char *Pointer_String_URL, const char *Pointer_String_Method, const char __attribute__((unused)) *Pointer_String_Version, const char __attribute__((unused)) *Pointer_String_Upload_Data, size_t __attribute__((unused)) *Pointer_Upload_Data_Size, void __attribute__((unused)) **Pointer_Persistent_Connection_Custom_Data)
{
	struct MHD_Response *Pointer_Response;
	int Return_Value;
	
	// Handle only GET methods
	if (strcmp(Pointer_String_Method, "GET") != 0) return MHD_NO;
	
	// Callback is called when a new connection header is received, and no response must be sent at this time
	if (*Pointer_Persistent_Connection_Custom_Data == NULL) // This value is always NULL for a new connection
	{
		*Pointer_Persistent_Connection_Custom_Data = (void *) 1; // Set the value to something else to tell that connection first step has been processed
		return MHD_YES; // Continue servicing request
	}
	
	// Create the page to send as the response
	if ((strncmp(Pointer_String_URL, "/", 1) == 0) || (strncmp(Pointer_String_URL, "/index.html", 11)))
	{
		if (MainPrepareIndexPageResponse(Pointer_Connection) != 0) return MHD_NO;
	}
	else return MHD_NO;
	
	// Create the response to send
	Pointer_Response = MHD_create_response_from_buffer(strlen(Main_String_Response), Main_String_Response, MHD_RESPMEM_PERSISTENT);
	if (Pointer_Response == NULL) return MHD_NO;
	
	// Send the response
	Return_Value = MHD_queue_response(Pointer_Connection, MHD_HTTP_OK, Pointer_Response);
	MHD_destroy_response(Pointer_Response);
	
	return Return_Value;
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	unsigned short Web_Server_Port;
	struct MHD_Daemon *Pointer_Web_Server;
	
	// Start logging system
	openlog(argv[0], 0, LOG_DAEMON);
	
	// Check parameters
	if (argc != 2)
	{
		syslog(LOG_ERR, "Bad parameters. Usage : %s Web_Server_Port", argv[0]);
		printf("Bad parameters. Usage : %s Web_Server_Port\n", argv[0]);
		return EXIT_FAILURE;
	}
	Web_Server_Port = atoi(argv[1]);
	
	// Start boiler server first, so board gets a chance to connect before the first web request comes
	if (BoilerInitializeServer() != 0)
	{
		syslog(LOG_ERR, "Failed to initialize boiler server, exiting.");
		return EXIT_FAILURE;
	}
	
	// Start web server
	Pointer_Web_Server = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, Web_Server_Port, NULL, NULL, MainWebServerAccessHandlerCallback, NULL, MHD_OPTION_END);
	if (Pointer_Web_Server == NULL)
	{
		BoilerUninitializeServer();
		syslog(LOG_ERR, "Failed to start web server daemon, exiting.");
		return EXIT_FAILURE;
	}
	syslog(LOG_INFO, "Server started and ready.");
	
	// Run board server
	while (1) BoilerRunServer();
	
	return 0;
}
