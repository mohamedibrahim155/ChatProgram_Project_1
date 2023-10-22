#include "Buffer.h"




void  Buffer:: WriteUInt32BE(uint32_t Intvalue)
{
    m_BufferData[m_WriteIndex++] = static_cast<uint8_t>((Intvalue >> 24) & 0xFF);  // bitwise AND operation with 0xFF (0xFF = 11111111 -> 8 bit)
    m_BufferData[m_WriteIndex++] = static_cast<uint8_t>((Intvalue >> 16) & 0xFF);
    m_BufferData[m_WriteIndex++] = static_cast<uint8_t>((Intvalue >> 8) & 0xFF);
    m_BufferData[m_WriteIndex++] = static_cast<uint8_t>(Intvalue & 0xFF);
}

uint32_t  Buffer:: ReadUInt32BE()
{
    uint32_t value = 0;
    value |= static_cast<uint32_t>(m_BufferData[m_ReadIndex++]) << 24;
    value |= static_cast<uint32_t>(m_BufferData[m_ReadIndex++]) << 16;
    value |= static_cast<uint32_t>(m_BufferData[m_ReadIndex++]) << 8;
    value |= static_cast<uint32_t>(m_BufferData[m_ReadIndex++]);
    return value;
}

void   Buffer::WriteUInt16BE(uint16_t Intvalue)
{
    //std::cout << "Int value : " << Intvalue << std::endl;
    m_BufferData[m_WriteIndex++] = static_cast<uint8_t>((Intvalue >> 8) & 0xFF);
    m_BufferData[m_WriteIndex++] = static_cast<uint8_t>(Intvalue & 0xFF);
}

void  Buffer::WriteBaseOnMessageType(ChatMessage& chatMessage)
{
    
    switch (chatMessage.messageType)
    {
    case MY_FLOAT:
       // float* floatValue = reinterpret_cast<float*>(chatMessage.messageToLoad);
       //float value = *floatValue;
       // WriteFloatBE(value);
        break;
    case U_INT32:
        chatMessage.messageLength = sizeof(uint32_t);
        chatMessage.packetSize = chatMessage.messageLength
            + sizeof(chatMessage.messageLength)
            + sizeof(chatMessage.messageToLoad)
            + sizeof(chatMessage.messageType)
            + sizeof(chatMessage.packetSize);

        WriteUInt32BE((uint32_t)chatMessage.messageType);
        WriteUInt32BE((uint32_t)chatMessage.messageLength);
        WriteUInt32BE((uint32_t)chatMessage.messageToLoad);
        break;
    case  U_INT16:
        chatMessage.messageLength = sizeof(uint16_t);
        chatMessage.packetSize = chatMessage.messageLength
            + sizeof(chatMessage.messageLength)
            + sizeof(chatMessage.messageToLoad)
            + sizeof(chatMessage.messageType)
            + sizeof(chatMessage.packetSize);
        WriteUInt32BE((uint32_t)chatMessage.messageType);
        WriteUInt32BE((uint32_t)chatMessage.messageLength);
        WriteUInt16BE((uint16_t)chatMessage.messageToLoad);
        break;

    case STRING:
        std::string* stringPtr = static_cast<std::string*>(chatMessage.messageToLoad);
        std::string newString = *stringPtr;
        chatMessage.messageLength = stringPtr->length();

        chatMessage.packetSize = chatMessage.messageLength
            + sizeof(chatMessage.messageLength)
            + sizeof(chatMessage.messageToLoad)
            + sizeof(chatMessage.messageType)
            + sizeof(chatMessage.packetSize);
        //    WriteString((const char*)messytype.messageToLoad);
        WriteUInt32BE((uint32_t)chatMessage.messageType);
        WriteUInt32BE((uint32_t)chatMessage.messageLength);
        WriteString(newString);
        //std::cout << "Written string : " << newString << std::endl;
        //WriteString(messytype.m);
        break;
    

    }
}


void Buffer::WriteBaseOnMessageType(Client_ID& clientID)
{
    std::string clientName = clientID.ClientName;
    uint32_t roomID = clientID.RoomID;

    uint32_t nameLength = static_cast<uint32_t>(clientName.length());

    uint32_t messageLength = nameLength + sizeof(uint32_t);
    uint32_t packetSize = messageLength
        + sizeof(messageLength)
        + nameLength
        + sizeof(uint32_t)
        + sizeof(packetSize);

    WriteUInt32BE(messageLength);
    WriteUInt32BE(packetSize);
    WriteUInt32BE(nameLength);
    WriteString(clientName);
    WriteUInt32BE(roomID);
}

void  Buffer::WriteUInt8BE(uint8_t Intvalue)
{
    //std::cout << "Int value : " << Intvalue << std::endl;
    m_BufferData[m_WriteIndex++] = static_cast<uint8_t>(Intvalue & 0xFF);
}

uint16_t  Buffer::ReadUInt16BE()
{
    uint16_t value = 0;
    value |= static_cast<uint16_t>(m_BufferData[m_ReadIndex++]) << 8;
    value |= static_cast<uint16_t>(m_BufferData[m_ReadIndex++]);
    return value;
}

uint8_t  Buffer::ReadUInt8BE()
{
    uint8_t value = 0;
    value |= static_cast<uint8_t>(m_BufferData[m_ReadIndex++]);
    return value;
}



void  Buffer::WriteString(const std::string& str)
{
    uint16_t strLength = static_cast<uint16_t>(str.length());
    WriteUInt16BE(strLength);
    for (char c : str)
    {
        m_BufferData[m_WriteIndex++] = static_cast<uint8_t>(c);
    }
}

std::string  Buffer:: ReadString()
{
    uint16_t length = ReadUInt16BE();
    std::string str;
    for (uint16_t i = 0; i < length; i++)
    {
        str.push_back(static_cast<char>(m_BufferData[m_ReadIndex++]));
    }
    return str;
}


//Redaing based on the struct ChatMessage
ChatMessage Buffer::ReadChatMessage()
{
    ChatMessage temp;
    uint32_t receivedType = this->ReadUInt32BE();
    uint32_t messageLength = this->ReadUInt32BE();
    uint32_t messageToLoad;
    uint16_t messageToLoad16;
    temp.messageLength = messageLength;
    temp.messageType = (eMessageType)receivedType;
    switch (temp.messageType)
    {
    case U_INT32 :
        messageToLoad = this->ReadUInt32BE();
        temp.messageToLoad = reinterpret_cast<void*>(messageToLoad);
        temp.messageToLoad = new uint32_t(messageToLoad);
        break;

    case U_INT16:
        messageToLoad16 = this->ReadUInt16BE();
        temp.messageToLoad = reinterpret_cast<void*>(messageToLoad16);
        temp.messageToLoad = new uint16_t(messageToLoad16);
        break;

    case STRING:
        std::string readString = this->ReadString();
       
        temp.messageLength = static_cast<uint32_t>(readString.length());
        auto strPtr = new std::string(readString);
        temp.messageToLoad = static_cast<void*>(strPtr);
        
        break;
    }
    //uint32_t* uintPtr = static_cast<uint32_t*>(temp.messageToLoad);
   // std::cout << "Value: " << *uintPtr << std::endl;
    return temp;
}


//Redaing based on the struct Client_ID
Client_ID Buffer:: ReadClientID() 
{
    Client_ID clientID;
    uint32_t messageLength;
    uint32_t packetSize;
    uint32_t nameLength;


    messageLength = ReadUInt32BE();
    packetSize = ReadUInt32BE();
    nameLength = ReadUInt32BE();

    std::string clientName = ReadString(); 

    uint32_t roomID = ReadUInt32BE();
    clientID.ClientName = clientName;
    clientID.RoomID = roomID;

    return clientID;
}



Buffer :: Buffer(int size)
{
    m_BufferData.resize(size);
    m_WriteIndex = 0;
    m_ReadIndex = 0;
}