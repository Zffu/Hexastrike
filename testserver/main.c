#include <server.h>
#include <client/client.h>
#include <client/connection.h>

#include <stdio.h>
#include <string.h>

HEXASTRIKE_SERVER* server;

unsigned char state = 0x00;

int readVarInt(unsigned char* b, int* index) {
    int value = 0;
    int position = 0;
    unsigned char currentByte;

    while (1) {
        currentByte = b[(*index)++];
        value |= (currentByte & 0x7F) << position;
        
        if ((currentByte & 0x80) == 0) {
            break;
        }

        position += 7;
        if (position >= 32) {
            return -1;
        }
    }

    return value;
}

char* readString(unsigned char *b, int* index) {
    int len = readVarInt(b, index);
    char* str = malloc(len);

    for(int i = 0; i < len; ++i) {
        str[i] = b[*index];
        ++*index;
    }

    return str;
}

void writeInt(unsigned char *b, int* index, int num) {
    b[*index] = num >> 24;
    b[++*index] = num >> 16;
    b[++*index] = num >> 8;
    b[++*index] = num;
}

void writeShort(unsigned char *b, int* index, int num) {
    b[*index] = num >> 8;
    b[++*index] = num;
}

void writeVarInt(unsigned char *b, int* index, int value) {
    do {
        unsigned char temp = value & 0x7F;
        value >>= 7;
        if (value != 0) {
            temp |= 0x80;
        }
        b[(*index)++] = temp;
    } while (value != 0);
}

int getSizeForVarInt(int value) {
    if((value & (0xFFFFFFFF << 7)) == 0) return 1;
    else if((value & (0xFFFFFFFF << 14)) == 0) return 2;
    else if((value & (0xFFFFFFFF << 21)) == 0) return 3;
    else if((value & (0xFFFFFFFF << 28)) == 0) return 4;
    else return 5;
}

long readLong(unsigned char *b, int* index) {
    long result = 0;

    for(int i = 0; i < 8; ++i) {
        result = (result << 8) | (b[*index] & 0xFF);
        ++*index;
    }

    return result;
}

void writeLong(unsigned char *b, int* index, unsigned long l) {
    b[*index] = l >> 56;
    b[++*index] = l >> 48;
    b[++*index] = l >> 40;
    b[++*index] = l >> 32;
    b[++*index] = l >> 24;
    b[++*index] = l >> 16;
    b[++*index] = l >> 8;
    b[++*index] = l;
}

char response[] = "{\"version\": {\"name\":\"Thunderstrike\",\"protocol\": 767},\"players\":{\"max\":1000,\"online\":0},\"description\": {\"text\": \"§7A Thunderstrike test server!\"}}";

void handle(CONNECTION* c, unsigned char* b, int size, int index) {
    while(size > 0) {
        int i = 0;
        int sz = readVarInt(b, &i);
        size -= i; // Only subtract bytes read for length
        
        int packetID = readVarInt(b, &i);
        
        if(packetID == 0x00) {
            if(state == 0) { // Handshake
                int protocolVersion = readVarInt(b, &i);
                char* serverAddress = readString(b, &i);
                unsigned short serverPort = (b[i] << 8) | b[i+1];
                i += 2;
                int nextState = readVarInt(b, &i);
                
                printf("Handshake: protocol=%d, nextState=%d\n", protocolVersion, nextState);
                state = nextState;
                free(serverAddress);
            }
            else if(state == 1) { // Status request
                int jsonLen = strlen(response);
                // Calculate proper lengths
                int stringLen = getSizeForVarInt(jsonLen) + jsonLen;
                int packetLen = getSizeForVarInt(0) + stringLen; // Packet ID (0) + string
                int totalLen = getSizeForVarInt(packetLen) + packetLen;
                
                char* bb = malloc(totalLen);
                int ind = 0;
            
                writeVarInt(bb, &ind, packetLen); // Packet length
                writeVarInt(bb, &ind, 0); // Packet ID
                writeVarInt(bb, &ind, jsonLen); // String length
                memcpy(bb + ind, response, jsonLen);
            
                send(c->socket, bb, totalLen, 0);
                free(bb);
            }
        }
        else if(packetID == 0x01 && state == 1) { // Ping
            long l = readLong(b, &i);
            
            // Calculate proper lengths for ping response
            int totalLen = getSizeForVarInt(9) + 9; // 9 = packetID(1) + long(8)
            char *bb = malloc(totalLen);
            int ind = 0;
            
            writeVarInt(bb, &ind, 9); // Packet length
            writeVarInt(bb, &ind, 0x01); // Packet ID
            writeLong(bb, &ind, l); // Echo the long value
            
            send(c->socket, bb, totalLen, 0);
            free(bb);
            
            // Close connection after ping response
            c_dsconn(server, index, c);
            return;
        }

        printf("Packet ID: %d\n", packetID);

        size -= (i - sz); // Subtract remaining bytes read
    }
}

int main() {
    server = hexastrike_sinit(3000);

    server->r_handler = handle;

    hexastrike_iopinit(server);
    hexastrike_dloop(server);
} 