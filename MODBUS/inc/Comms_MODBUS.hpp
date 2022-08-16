#ifndef COMMS_MODBUS_HPP
#define COMMS_MODBUS_HPP

#include <iostream>
#include <unistd.h>
#include <fcntl.h> 
#include <termios.h>
#include <cstring>
#include "crc16.h"
#include <stdlib.h>

const unsigned char ESP32 = 0x01;

const unsigned char PEDIR = 0x23;
const unsigned char ENVIAR = 0x16;

const unsigned char SOLICITACAO_INTEIRO = 0xA1;
const unsigned char SOLICITACAO_FLOAT = 0xA2;
const unsigned char SOLICITACAO_STRING = 0xA3;

const unsigned char ENVIAR_INTEIRO = 0xB1;
const unsigned char ENVIAR_FLOAT = 0xB2;
const unsigned char ENVIAR_STRING = 0xB3;

class Comms_MODBUS {

    private:
        int uart0_filestream;
        struct termios options;

        unsigned char tx_buffer[255];
        unsigned char rx_buffer[255];
        unsigned char crcMessage[2];
        short crcValue;
        int rx_length;
        unsigned char *p_tx_buffer;
        int count;
        unsigned char matricula[4];

    public:
        Comms_MODBUS();
        ~Comms_MODBUS();

        void solicitacao(unsigned char codigoProtocolo);
        void enviar(int valorEnviado);
        void enviar(float valorEnviado);
        void enviar(std::string valorEnviado);
        int convertBufferInteiro(unsigned char *buffer);
        float convertBufferFloat(unsigned char *buffer);
        std::string convertBufferString(unsigned char *buffer);

        bool enviarBuffer(int flag, unsigned char *buffer);

        bool validarCRC(unsigned char *bufferValidacao, size_t tamanho);


};

#endif