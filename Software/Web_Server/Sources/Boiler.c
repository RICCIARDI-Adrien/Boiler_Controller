/** @file Boiler.c
 * See Boiler.h for description.
 * @author Adrien RICCIARDI
 */
#include <arpa/inet.h>
#include <Boiler.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The server socket. */
static int Boiler_Server_Socket = -1;
/** The board socket. */
static int Boiler_Board_Socket = -1;

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int BoilerInitializeServer(void)
{
	int Is_Enabled = 1;
	struct sockaddr_in Address;
	
	// Try to create server socket
	Boiler_Server_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Boiler_Server_Socket == -1)
	{
		syslog(LOG_ERR, "Failed to create server socket (%s).", strerror(errno));
		return -1;
	}
	
	// Configure the socket to allow reusing the address without waiting
	setsockopt(Boiler_Server_Socket, SOL_SOCKET, SO_REUSEADDR, &Is_Enabled, sizeof(Is_Enabled));
	
	// Bind the socket to the board port
	Address.sin_family = AF_INET,
	Address.sin_port = htons(1234);
	Address.sin_addr.s_addr = INADDR_ANY;
	if (bind(Boiler_Server_Socket, (const struct sockaddr *) &Address, sizeof(Address)) != 0)
	{
		close(Boiler_Server_Socket);
		syslog(LOG_ERR, "Failed to bind server socket (%s).", strerror(errno));
		return -1;
	}
	
	// Accept only one client at a time
	if (listen(Boiler_Server_Socket, 1) != 0)
	{
		close(Boiler_Server_Socket);
		syslog(LOG_ERR, "Failed to configure server socket connections listening (%s).", strerror(errno));
		return -1;
	}
	
	return 0;
}

void BoilerUninitializeServer(void)
{
	if (Boiler_Board_Socket != -1) close(Boiler_Board_Socket);
	if (Boiler_Server_Socket != -1) close(Boiler_Server_Socket);
}

int BoilerRunServer(void)
{
	struct sockaddr_in Address;
	socklen_t Address_Size;
	
	// Wait for a client to connect
	Address_Size = sizeof(Address);
	Boiler_Board_Socket = accept(Boiler_Server_Socket, (struct sockaddr *) &Address, &Address_Size);
	if (Boiler_Board_Socket == -1)
	{
		syslog(LOG_ERR, "Failed to accept next board connection (%s).", strerror(errno));
		return -1;
	}
	syslog(LOG_INFO, "Board connected with address %s:%d.", inet_ntoa(Address.sin_addr), ntohs(Address.sin_port));
	
	return 0;
}
