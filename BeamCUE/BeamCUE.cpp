#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#include <bitset>
// disable deprecation
#pragma warning(disable: 4996)

// cuesdk includes
#define CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS
#include "CUESDK.h"
#include <atomic>
#include <thread>
#include <string>
#include <cmath>

using namespace std;

const char* toString(CorsairError error)
{
	switch (error) {
	case CE_Success:
		return "CE_Success";
	case CE_ServerNotFound:
		return "CE_ServerNotFound";
	case CE_NoControl:
		return "CE_NoControl";
	case CE_ProtocolHandshakeMissing:
		return "CE_ProtocolHandshakeMissing";
	case CE_IncompatibleProtocol:
		return "CE_IncompatibleProtocol";
	case CE_InvalidArguments:
		return "CE_InvalidArguments";
	default:
		return "unknown error";
	}
}

void highlightKey(CorsairLedId ledId, CorsairLedColor color) {
	CorsairLedColor off;
	off.r = 0;
	off.g = 0;
	off.b = 0;
	CorsairLedColor red;
	red.r = 255;
	red.g = 0;
	red.b = 0;
	CorsairLedColor orange;
	orange.r = 255;
	orange.g = 127;
	orange.b = 0;
	CorsairLedColor blue;
	blue.r = 0;
	blue.g = 0;
	blue.b = 255;
	CorsairLedColor aqua;
	aqua.r = 0;
	aqua.g = 255;
	aqua.b = 255;
	CorsairLedColor white;
	white.r = 255;
	white.g = 255;
	white.b = 255;
	for (auto x = .0; x < 2; x += .1) {
		auto val = static_cast<int>((1 - abs(x - 1)) * 255);
		// set color .r .g .b to val
		color.r = val;
		color.g = val;
		color.b = val;
		CorsairSetLedsColors(1, new CorsairLedColor{ ledId, color.r, color.g, color.b });
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		CorsairSetLedsColors(1, new CorsairLedColor{ ledId, 0, 0, 0 });
	}
}

// struct I4sHccfffffffIIfffcci in python:
// make unsigned int called timeMsg
unsigned int timeMsg;
// make 4 byte char called carName
char carName[4];
// make pad byte called pad1
char pad1;
// make unsiogned short called info
unsigned short info;
// make 2 unsigned char called gear AND playerID
unsigned char gear, playerID;
// make 7 floats called speed, rpm, turbo, engineTemp, fuel, oilPressure, oilTemp
float speed, rpm, turbo, engineTemp, fuel, oilPressure, oilTemp;
// make 2 unsigned ints called dashLights and showLights
unsigned int dashLights, showLights;
// make 3 floats called throttle, brake, clutch
float throttle, brake, clutch;
// make 16 byte char called display1
char display1[16];
// make pad byte called pad2
char pad2;
// make 16 byte char called display2
char display2[16];

// make threads
void recieveUDP() {
	// Initialise Winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);

	// Start Winsock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Broken, exit
		cout << "Can't start Winsock! " << wsOk;
		return;
	}

	// Create a hint structure for the server
	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(4444);

	// Bind the socket to an IP address and port
	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		cout << "Unable to bind socket! " << WSAGetLastError() << endl;
		return;
	}

	// Create a sockaddr_in structure for the client
	sockaddr_in client;
	int clientLength = sizeof(client);
	
	// while loop
	while (true) {
		// Wait for message
		char buf[265];
		ZeroMemory(buf, 265);
		int bytesIn = recvfrom(in, buf, 265, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			cout << "Error receiving from client " << WSAGetLastError() << endl;
			continue;
		}

		// Display message and client info
		char clientIp[256];
		ZeroMemory(clientIp, 256);
		
		// "unpack" the data
		memcpy(&timeMsg, buf, 4);
		memcpy(&carName, buf + 4, 5);
		//memcpy(&pad1, buf + 7, 1);
		memcpy(&info, buf + 8, 2);
		memcpy(&gear, buf + 10, 1);
		memcpy(&playerID, buf + 11, 1);
		memcpy(&speed, buf + 12, 4);
		memcpy(&rpm, buf + 16, 4);
		memcpy(&turbo, buf + 20, 4);
		memcpy(&engineTemp, buf + 24, 4);
		memcpy(&fuel, buf + 28, 4);
		memcpy(&oilPressure, buf + 32, 4);
		memcpy(&oilTemp, buf + 36, 4);
		memcpy(&dashLights, buf + 40, 4);
		memcpy(&showLights, buf + 44, 4);
		memcpy(&throttle, buf + 48, 4);
		memcpy(&brake, buf + 52, 4);
		memcpy(&clutch, buf + 56, 4);
		memcpy(&display1, buf + 60, 15);
		memcpy(&pad2, buf + 75, 1);
		memcpy(&display2, buf + 76, 15);
	}
	closesocket(in);
	WSACleanup();
}

void BeamStats() {
	while (true) {
		// print the data
		cout << "timeMsg: " << timeMsg << endl;
		cout << "Car Name: " << carName << endl;
		// need to cast any chars to int for previewing as int, otherwise it will show ascii
		cout << "Info: " << (int)info << endl;
		cout << "Gear: " << (int)gear << endl;
		cout << "Player ID: " << (int)playerID << endl;
		cout << "Speed: " << speed << endl;
		cout << "RPM: " << rpm << endl;
		cout << "Turbo: " << turbo << endl;
		cout << "Engine Temp: " << engineTemp << endl;
		cout << "Fuel: " << fuel << endl;
		cout << "Oil Pressure: " << oilPressure << endl;
		cout << "Oil Temp: " << oilTemp << endl;
		string dashLightsBits = bitset<11>(dashLights).to_string();
		string showLightsBits = bitset<11>(showLights).to_string();
		cout << "dashLightsBits string: " << dashLightsBits << endl;
		cout << "ShowLightsBits string: " << showLightsBits << endl;
		cout << "Throttle: " << throttle << endl;
		cout << "Brake: " << brake << endl;
		cout << "Clutch: " << clutch << endl;
		// show raw bits from display 1 and display 2
		cout << "Display 1: ";
		for (int i = 0; i < 16; i++) {
			cout << (int)display1[i] << " ";
		}
		cout << endl;
		cout << "Display 2: ";
		for (int i = 0; i < 16; i++) {
			cout << (int)display2[i] << " ";
		}
		cout << endl;
		
		// sleep thread for 100 ms
		this_thread::sleep_for(chrono::milliseconds(100));
		
		// clear console output
		system("cls");
	}
}

void gearBridge() {
	// set colours available
	CorsairLedColor off;
	off.r = 0;
	off.g = 0;
	off.b = 0;
	CorsairLedColor red;
	red.r = 255;
	red.g = 0;
	red.b = 0;
	CorsairLedColor orange;
	orange.r = 255;
	orange.g = 127;
	orange.b = 0;
	CorsairLedColor blue;
	blue.r = 0;
	blue.g = 0;
	blue.b = 255;
	CorsairLedColor aqua;
	aqua.r = 0;
	aqua.g = 255;
	aqua.b = 255;
	CorsairLedColor white;
	white.r = 255;
	white.g = 255;
	white.b = 255;
	while (true) {
		int iGear = (int)gear;
		// cout << "Gear: " << iGear << " RPM: " << rpm << endl;		
		string dashLightsBits = bitset<11>(dashLights).to_string();
		string showLightsBits = bitset<11>(showLights).to_string();
		
		if (showLightsBits[1] == '1' && rpm < 50) {
			// if the rpm is less than 50 and the light is on then we can assume the engine is off
			highlightKey(CorsairLedId::CLK_V, orange);
			for (int i = 13; i < 23; i++) {
				off.ledId = static_cast<CorsairLedId>(i + 1);
				CorsairSetLedsColors(1, &off);
			}
			off.ledId = CorsairLedId::CLK_LeftCtrl;
			CorsairSetLedsColors(1, &off);
			off.ledId = CorsairLedId::CLK_Q;
			CorsairSetLedsColors(1, &off);
		}
		else {
			for (int i = 13; i < 23; i++) {
				red.ledId = static_cast<CorsairLedId>(i + 1);
				CorsairSetLedsColors(1, &red);
			}
			white.ledId = CorsairLedId::CLK_LeftCtrl;
			CorsairSetLedsColors(1, &white);
			

			switch (iGear) {
			case 0:
				highlightKey(CorsairLedId::CLK_R, white);
				break;
			case 1:
				highlightKey(CorsairLedId::CLK_0, white);
				break;
			case 2:
				highlightKey(CorsairLedId::CLK_1, white);
				break;
			case 3:
				highlightKey(CorsairLedId::CLK_2, white);
				break;
			case 4:
				highlightKey(CorsairLedId::CLK_3, white);
				break;
			case 5:
				highlightKey(CorsairLedId::CLK_4, white);
				break;
			case 6:
				highlightKey(CorsairLedId::CLK_5, white);
				break;
			case 7:
				highlightKey(CorsairLedId::CLK_6, white);
				break;
			case 8:
				highlightKey(CorsairLedId::CLK_7, white);
				break;
			case 9:
				highlightKey(CorsairLedId::CLK_8, white);
				break;
			case 10:
				highlightKey(CorsairLedId::CLK_9, white);
			}
		}
	}
}

void dashBridge() {
	// set colours available
	CorsairLedColor off;
	off.r = 0;
	off.g = 0;
	off.b = 0;
	CorsairLedColor red;
	red.r = 255;
	red.g = 0;
	red.b = 0;
	CorsairLedColor green;
	green.r = 0;
	green.g = 255;
	green.b = 0;
	CorsairLedColor orange;
	orange.r = 255;
	orange.g = 127;
	orange.b = 0;
	CorsairLedColor blue;
	blue.r = 0;
	blue.g = 0;
	blue.b = 255;
	CorsairLedColor aqua;
	aqua.r = 0;
	aqua.g = 255;
	aqua.b = 255;
	CorsairLedColor white;
	white.r = 255;
	white.g = 255;
	white.b = 255;
	
	// dashlights are always available regardless if engine is on
	while (true) {
		string dashLightsBits = bitset<11>(dashLights).to_string();
		string showLightsBits = bitset<11>(showLights).to_string();

		// Indicators/hazards
		if (showLightsBits[4] == '1' && showLightsBits[5] == '1') {
			orange.ledId = CorsairLedId::CLK_SlashAndQuestionMark;
			CorsairSetLedsColors(1, &orange);
		}
		else if (showLightsBits[4] == '1') {
			orange.ledId = CorsairLedId::CLK_PeriodAndBiggerThan;
			CorsairSetLedsColors(1, &orange);
		}
		else if (showLightsBits[5] == '1') {
			orange.ledId = CorsairLedId::CLK_CommaAndLessThan;
			CorsairSetLedsColors(1, &orange);
		}
		else {
			off.ledId = CorsairLedId::CLK_SlashAndQuestionMark;
			CorsairSetLedsColors(1, &off);
			off.ledId = CorsairLedId::CLK_CommaAndLessThan;
			CorsairSetLedsColors(1, &off);
			off.ledId = CorsairLedId::CLK_PeriodAndBiggerThan;
			CorsairSetLedsColors(1, &off);
		}
		
		// Parking Brake
		if (showLightsBits[8] == '1') {
			// parking brake
			red.ledId = CorsairLedId::CLK_Space;
			CorsairSetLedsColors(1, &red);
			red.ledId = CorsairLedId::CLK_P;
			CorsairSetLedsColors(1, &red);
		}
		else {
			off.ledId = CorsairLedId::CLK_Space;
			CorsairSetLedsColors(1, &off);
			off.ledId = CorsairLedId::CLK_P;
			CorsairSetLedsColors(1, &off);

		}
		
		// full beam
		if (showLightsBits[9] == '1') {
	
			blue.ledId = CorsairLedId::CLK_N;
			CorsairSetLedsColors(1, &blue);
			// could add some logic to determine if headlights are on or not, judging by bit value
			
		}
		else {
			off.ledId = CorsairLedId::CLK_N;
			CorsairSetLedsColors(1, &off);
		}
		
		// Brake pedal/ ABS
		if (brake == 1) {
			white.ledId = CorsairLedId::CLK_DownArrow;
			CorsairSetLedsColors(1, &white);
		}
		// ABS light
		if (showLightsBits[0] == '1') {
			red.ledId = CorsairLedId::CLK_DownArrow;
			CorsairSetLedsColors(1, &red);
		}
		else if (showLightsBits[0] == '0' & brake < 0.8) {
			off.ledId = CorsairLedId::CLK_DownArrow;
			CorsairSetLedsColors(1, &off);
		}

		// throttle pedal
		if (throttle > 0.8) {
			white.ledId = CorsairLedId::CLK_UpArrow;
			CorsairSetLedsColors(1, &white);
		}
		else {
			off.ledId = CorsairLedId::CLK_UpArrow;
			CorsairSetLedsColors(1, &off);
		}

		// clutch pedal
		if (clutch > 0.5) {
			white.ledId = CorsairLedId::CLK_LeftShift;
			CorsairSetLedsColors(1, &white);
		}
		else {
			off.ledId = CorsairLedId::CLK_LeftShift;
			CorsairSetLedsColors(1, &off);
		}
		
		// engine light (reffered to as bettery warning)
		if (showLightsBits[1] == '1') {
			red.ledId = CorsairLedId::CLK_CapsLock;
			CorsairSetLedsColors(1, &red);
		}
		else {
			green.ledId = CorsairLedId::CLK_CapsLock;
			CorsairSetLedsColors(1, &green);
		}
		
		// TC light - off is 1, on is 0
		if (showLightsBits[6] == '0') {
			aqua.ledId = CorsairLedId::CLK_Q;
			CorsairSetLedsColors(1, &aqua);
		}
		else {
			white.ledId = CorsairLedId::CLK_Q;
			CorsairSetLedsColors(1, &white);
		}
		
		// Oil Warning light
		if (showLightsBits[2] == '1') {
			red.ledId = CorsairLedId::CLK_Tab;
			CorsairSetLedsColors(1, &red);
		}
		else {
			green.ledId = CorsairLedId::CLK_Tab;
			CorsairSetLedsColors(1, &green);
		}

	}
}

int main() {
	// initialise CUE SDK
	// Initialise CUE SDK
	CorsairPerformProtocolHandshake();
	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << "\nPress any key to quit." << std::endl;
		getchar();
		return 0;
	}
	CorsairRequestControl(CAM_ExclusiveLightingControl);

	thread t1(recieveUDP);
	thread t2(BeamStats);
	thread t3(gearBridge);
	thread t4(dashBridge);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}
