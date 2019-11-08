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
// Private constants
//-------------------------------------------------------------------------------------------------
/** The magic number preceding all received and sent commands. */
#define BOILER_PROTOCOL_MAGIC_NUMBER 0xA5

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
/** All known commands. */
typedef enum
{
	BOILER_COMMAND_GET_FIRMWARE_VERSION,
	BOILER_COMMAND_GET_SENSORS_RAW_TEMPERATURES,
	BOILER_COMMAND_GET_SENSORS_CELSIUS_TEMPERATURES,
	BOILER_COMMAND_GET_MIXING_VALVE_POSITION,
	BOILER_COMMAND_SET_NIGHT_MODE,
	BOILER_COMMAND_GET_DESIRED_ROOM_TEMPERATURES,
	BOILER_COMMAND_SET_DESIRED_ROOM_TEMPERATURES,
	BOILER_COMMAND_GET_TRIMMERS_RAW_VALUES,
	BOILER_COMMAND_GET_BOILER_RUNNING_MODE,
	BOILER_COMMAND_SET_BOILER_RUNNING_MODE,
	BOILER_COMMAND_GET_TARGET_START_WATER_TEMPERATURE,
	BOILER_COMMAND_GET_HEATING_CURVE_PARAMETERS,
	BOILER_COMMAND_SET_HEATING_CURVE_PARAMETERS,
	BOILER_COMMANDS_COUNT
} TBoilerCommand;

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The server socket. */
static int Boiler_Server_Socket = -1;
/** The board socket. */
static int Boiler_Board_Socket = -1;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Send a command and its payload and wait for the answer.
 * @param Command The command code.
 * @param Command_Payload_Size How may bytes of payload to send (set to 0 if the command has no payload).
 * @param Answer_Payload_Size How many bytes of payload to wait for (set to 0 for a command providing no answer other than magic number and command code).
 * @param Pointer_Payload_Buffer The payload (if any). Make sure the buffer is big enough for answer.
 * @return -1 if an error occurred (board connection is automatically closed in this case),
 * @return 0 on success.
 */
static int BoilerSendCommand(TBoilerCommand Command, int Command_Payload_Size, int Answer_Payload_Size, void *Pointer_Payload_Buffer)
{
	unsigned char Buffer[16];
	
	// Create the full command
	Buffer[0] = BOILER_PROTOCOL_MAGIC_NUMBER;
	Buffer[1] = Command;
	memcpy(&Buffer[2], Pointer_Payload_Buffer, Command_Payload_Size);
	Command_Payload_Size += 2; // Adjust command size to take all fields into account
	
	// Send command
	if (write(Boiler_Board_Socket, Buffer, Command_Payload_Size) != Command_Payload_Size)
	{
		close(Boiler_Board_Socket);
		syslog(LOG_ERR, "Failed to send command (command code : %d, command size : %d, %s).", Command, Command_Payload_Size, strerror(errno));
		return -1;
	}
	
	// Wait for the answer
	Answer_Payload_Size += 2; // Adjust answer size to take all fields into account
	if (read(Boiler_Board_Socket, Buffer, Answer_Payload_Size) != Answer_Payload_Size)
	{
		close(Boiler_Board_Socket);
		syslog(LOG_ERR, "Failed to receive answer (command code : %d, command size : %d, %s).", Command, Answer_Payload_Size, strerror(errno));
		return -1;
	}
	
	// Copy answer to buffer
	memcpy(Pointer_Payload_Buffer, &Buffer[2], Answer_Payload_Size - 2);
	
	return 0;
}

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

int BoilerGetSensorsCelsiusTemperatures(int *Pointer_Outside_Temperature, int *Pointer_Radiator_Start_Water_Temperature)
{
	char Temperatures[2];
	
	if (BoilerSendCommand(BOILER_COMMAND_GET_SENSORS_CELSIUS_TEMPERATURES, 0, 2, Temperatures) != 0) return -1;
	*Pointer_Outside_Temperature = Temperatures[0];
	*Pointer_Radiator_Start_Water_Temperature = Temperatures[1];
	
	return 0;
}

int BoilerGetDesiredRoomTemperatures(int *Pointer_Day_Temperature, int *Pointer_Night_Temperature)
{
	char Temperatures[2];
	
	if (BoilerSendCommand(BOILER_COMMAND_GET_DESIRED_ROOM_TEMPERATURES, 0, 2, Temperatures) != 0) return -1;
	*Pointer_Day_Temperature = Temperatures[0];
	*Pointer_Night_Temperature = Temperatures[1];
	
	return 0;
}

int BoilerSetDesiredRoomTemperatures(int Day_Temperature, int Night_Temperature)
{
	char Payload[2];
	
	Payload[0] = (char) Day_Temperature;
	Payload[1] = (char) Night_Temperature;
	if (BoilerSendCommand(BOILER_COMMAND_SET_DESIRED_ROOM_TEMPERATURES, 2, 0, Payload) != 0) return -1;
	
	return 0;
}

int BoilerGetBoilerRunningMode(int *Pointer_Is_Boiler_Running)
{
	unsigned char Is_Running;
	
	if (BoilerSendCommand(BOILER_COMMAND_GET_BOILER_RUNNING_MODE, 0, 1, &Is_Running) != 0) return -1;
	if (Is_Running) *Pointer_Is_Boiler_Running = 1;
	else *Pointer_Is_Boiler_Running = 0;
	
	return 0;
}

int BoilerSetBoilerRunningMode(int Is_Boiler_Running)
{
	unsigned char Payload = (unsigned char) Is_Boiler_Running;
	
	if (BoilerSendCommand(BOILER_COMMAND_SET_BOILER_RUNNING_MODE, 1, 0, &Payload) != 0) return -1;
	
	return 0;
}

int BoilerGetTargetRadiatorStartWaterTemperature(int *Pointer_Temperature)
{
	if (BoilerSendCommand(BOILER_COMMAND_GET_TARGET_START_WATER_TEMPERATURE, 0, 1, Pointer_Temperature) != 0) return -1;
	
	return 0;
}
