//#include "Buffer.h"
//#include <string>
//#include <iostream>
//
//
//
//
//
//
//
//
//
//int main()
//{
//	/*PacketHeader _header;
//	_header.packetSize = 1024;  
//	_header.messageType = STRING;*/
//
//	ChatMessage _chatMessage;
//
//	_chatMessage.packetSize;               // 4 bytes
//	_chatMessage.messageType = STRING;     // 4 bytes
//	std::string hey = "HEYDA";				// 7 bytes (length =4, char "H" =1,E = 1,Y = 1)
//	_chatMessage.messageLength;				// 4 byte
//	_chatMessage.messageToLoad = &hey;      // 8 byts (void*) 
//
//	_chatMessage.clientName = "Ibrahim";
//;
//
//
////PacketHeader _header2;
////_header2.packetSize = 1024;
////_header2.messageType = U_INT16;
//
//ChatMessage _chatMessage2;
//_chatMessage2.packetSize;
//_chatMessage2.messageType = U_INT16;
//
//_chatMessage2.messageToLoad = (unsigned short*)5;
//
//
//
//
//
////PacketHeader _header3;
////_header3.packetSize = 1024;
////_header3.messageType = MY_FLOAT;
//
//ChatMessage _chatMessage3;
//_chatMessage3.packetSize;                               // 4 byte
//_chatMessage3.messageType = U_INT32;                   // 4 byte
//_chatMessage3.messageToLoad = (unsigned long*)65;           // 8 byte
//_chatMessage3.messageLength;                           // 4 byte
//
//
//
//	Buffer buffer;
//	buffer.WriteBaseOnMessageType(_chatMessage);
//
//	Buffer buffer2;
//	buffer2.WriteBaseOnMessageType(_chatMessage2);
//
//
//	Buffer buffer3;
//	buffer3.WriteBaseOnMessageType(_chatMessage3);
//
//	ChatMessage result =  buffer.ReadChatMessage();
//	int number = buffer3.ReadUInt32BE();
//	uint32_t temp32 = buffer3.ReadUInt32BE();
//
//	 //float number2 = buffer3.ReadUInt32BE();
//	 //float number2 = buffer3.ReadFloatBE();
//	uint16_t tempUint = 5;
//
//	//std::cout << " word : " << word << std::endl;
//	std::cout << " Number 16: " << number << std::endl;
//	std::cout << " temp : " << temp32 << std::endl;
//	std::cout << " size of packetSize f: " << sizeof(_chatMessage2) << std::endl;
//	std::cout << " msg of length : " << _chatMessage3.packetSize << std::endl;
//	switch (result.messageType) {
//	case U_INT32:
//		std::cout << "chat result messageToLoad: " << *static_cast<uint32_t*>(result.messageToLoad) << std::endl;
//		break;
//	case U_INT16:
//		std::cout << "chat result messageToLoad: " << *static_cast<uint16_t*>(result.messageToLoad) << std::endl;
//		break;
//	case STRING:
//		std::cout << "chat result messageToLoad: " << *static_cast<std::string*>(result.messageToLoad) << std::endl;
//		std::cout << "client name : " << result.clientName << std::endl;
//		break;
//	default:
//		// Handle other cases
//		break;
//	}
//	std::cout << " chat result  messageType: " << (eMessageType)result.messageType << std::endl;
//	std::cout << " chat result msglength : " << result.messageLength << std::endl;
//
//	//printf("numf %f",number2);
//	return 0;
//}