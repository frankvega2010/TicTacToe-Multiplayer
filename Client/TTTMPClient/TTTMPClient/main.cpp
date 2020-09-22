#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

/*typedef struct
{
	byte cmd;
	char data[255];
} message_t;*/

struct message
{
	byte cmd;
	char data[255];
};

void main()
{
	// inicializar winsock

	WORD version = MAKEWORD(2, 2);
	WSADATA data;

	if (WSAStartup(version, &data) == 0)
	{
		string ipToUse = "";
		int portToUse = 0;

		cout << "Escribe la ip a usar: ";
		cin >> ipToUse;
		cout << endl;
		cout << "Escribe el puerto a usar: ";
		cin >> portToUse;
		cout << endl;

		fflush(stdin);

		// indicar ip y puerto de destino
		sockaddr_in server;
		int serverSize = sizeof(server);
		server.sin_family = AF_INET; // AF_INET == IPV4
		server.sin_port = htons(portToUse); // puerto destino
		inet_pton(AF_INET, ipToUse.c_str(), &server.sin_addr); // direccion ip destino		

		// crear socket, UDP
		SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);
		if (out == INVALID_SOCKET)
		{
			cout << "Invalid socket: " << WSAGetLastError() << endl;

			return;
		}

		message msgRcd;
		message msg;
		//msgRcd.cmd = '1';
		byte cmd = '1';

		// enviar data a traves del socket
		string msgToSend;
		getline(cin, msgToSend);

		/*char buf[1024];
		memset(buf, 0, sizeof(buf));*/
		memset(&msg, 0, sizeof(msg));
		memset(&msgRcd, 0, sizeof(msgRcd));

		do
		{
			//memset(buf, 0, sizeof(buf));
			memset(&msg, 0, sizeof(msg));
			memset(&msgRcd, 0, sizeof(msgRcd));

			switch (cmd)
			{
			case '0':
			{
				/*cout << "Esperando respuesta del servidor... ";
				cout << endl;*/
			}
				// No input.
				break;
			case '1':
			{
				cout << "Escribe el mensaje a mandar: ";
				getline(cin, msgToSend);
				cout << endl;
				//msgToSend = msgToSend + '\n';
				msg = *((message*)msgToSend.c_str());
				sendto(out, (char*)&msg, sizeof(msg), 0, (sockaddr*)&server, sizeof(server));
			}
				break;
			default:
				break;
			}

			int bytesIn = recvfrom(out, (char*)&msgRcd, sizeof(msgRcd), 0, (sockaddr*)&server, &serverSize);

			if (bytesIn == SOCKET_ERROR)
			{
				cerr << "Error al recibir data" << endl;
			}

			cout << msgRcd.data << endl;
			cout << endl;
			cmd = msgRcd.cmd;
		} while (cmd != '4');

		// cerrar el socket
		closesocket(out);
	}
	else
	{
		return;
	}



	// cerrar winsock
	WSACleanup();
}