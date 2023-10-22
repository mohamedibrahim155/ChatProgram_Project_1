#include <vector>
#include <cstring>
#include <cstdint>
#include <string>
#include <iostream>



enum eMessageType
{
    U_INT32,
    U_INT16,
    STRING,
    MY_FLOAT
};
//struct PacketHeader
//{
//    uint32_t packetSize;
//    eMessageType messageType;
//};


struct ChatMessage
{
    void* messageToLoad;  // 8
    size_t messageLength; // 4
    uint32_t packetSize;  //  4
    eMessageType messageType; // 4
};

struct Client_ID
{
    std::string ClientName;
    uint32_t RoomID;
};

class Buffer {
public:
    Buffer(int size = 512);

    ~Buffer() { }



    void WriteUInt32BE(uint32_t Intvalue);
    void WriteUInt16BE(uint16_t Intvalue);


    //void WriteFloatBE(float floatValue)
    //{
    //    uint16_t* floatAsUInt16 = reinterpret_cast<uint16_t*>(&floatValue);
    //    WriteUInt16BE(floatAsUInt16[0]);
    //    WriteUInt16BE(floatAsUInt16[1]);
    //};


    void WriteBaseOnMessageType(ChatMessage& chatMessage);

    void WriteBaseOnMessageType(Client_ID& cliendID);

    void WriteString(const char* str)
    {
        uint16_t strLength = static_cast<uint16_t>(strlen(str));
        WriteUInt16BE(strLength);
        for (uint16_t i = 0; i < strLength; i++) {
            m_BufferData[m_WriteIndex++] = static_cast<uint8_t>(str[i]);
        }
    }



    void WriteUInt8BE(uint8_t Intvalue);


    uint32_t ReadUInt32BE();
    uint16_t ReadUInt16BE();
    uint8_t ReadUInt8BE();

    ChatMessage ReadChatMessage();

    Client_ID ReadClientID();
    void WriteString(const std::string& str);
    std::string ReadString();


    //float ReadFloatBE() 
    //{
    //    uint16_t highBytes = ReadUInt16BE();
    //    uint16_t lowBytes = ReadUInt16BE();
    //    uint16_t floatAsUInt16[2] = { highBytes, lowBytes };
    //    return *reinterpret_cast<float*>(floatAsUInt16);
    //}


    std::vector<uint8_t> m_BufferData;
    int m_WriteIndex;
    int m_ReadIndex;

private:

};