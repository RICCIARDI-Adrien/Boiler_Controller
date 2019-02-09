/** @file Main.c
 * An HTTP front-end for the boiler controller board.
 * @author Adrien RICCIARDI
 */
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Called when a client requests a web page.
 */
static int MainWebServerAccessHandlerCallback(void *Pointer_Custom_Data, struct MHD_Connection *Pointer_Connection, const char *Pointer_String_URL, const char __attribute__((unused)) *Pointer_String_Method, const char __attribute__((unused)) *Pointer_String_Version, const char *Pointer_String_Upload_Data, size_t *Pointer_Upload_Data_Size, void **Pointer_Persistent_Connection_Custom_Data)
{
	printf("Pointer_String_URL=%s   Pointer_String_Method=%s    Pointer_String_Version=%s\n", Pointer_String_URL, Pointer_String_Method, Pointer_String_Version);
	return 0;
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
		syslog(LOG_ERR, "Bad parameters. Usage : %s Server_Port", argv[0]);
		printf("Bad parameters. Usage : %s Server_Port\n", argv[0]);
		return EXIT_FAILURE;
	}
	Web_Server_Port = atoi(argv[1]);
	
	// Start web server
	Pointer_Web_Server = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, Web_Server_Port, NULL, NULL, MainWebServerAccessHandlerCallback, NULL, MHD_OPTION_END);
	if (Pointer_Web_Server == NULL)
	{
		syslog(LOG_ERR, "Failed to start web server daemon, exiting.\n");
		return EXIT_FAILURE;
	}
	
	while (1);
	
	return 0;
}
