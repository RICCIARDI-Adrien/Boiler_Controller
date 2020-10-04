/** @file Main.c
 * An HTTP front-end for the boiler controller board.
 * @author Adrien RICCIARDI
 */
#include <Boiler.h>
#include <microhttpd.h>
#include <Pages.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** Store the HTML page to send as response. */
static char Main_String_Response[10 * 1024];

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
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
	if ((strcmp(Pointer_String_URL, "/") == 0) || (strncmp(Pointer_String_URL, "/index.html", 11) == 0))
	{
		if (PageIndex(Pointer_Connection, Main_String_Response) != 0) return MHD_NO;
	}
	else if (strncmp(Pointer_String_URL, "/settings.html", 14) == 0)
	{
		if (PageSettings(Pointer_Connection, Main_String_Response) != 0) return MHD_NO;
	}
	// Unknown page
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
