#ifndef COMMS_UART_HPP
#define COMMS_UART_HPP

#include <iostream>
#include <unistd.h>
#include <fcntl.h> 
#include <termios.h>
#include <cstring>

const unsigned char SOLICITACAO_INTEIRO = 0xA1;
const unsigned char SOLICITACAO_FLOAT = 0xA2;
const unsigned char SOLICITACAO_STRING = 0xA3;

const unsigned char ENVIAR_INTEIRO = 0xB1;
const unsigned char ENVIAR_FLOAT = 0xB2;
const unsigned char ENVIAR_STRING = 0xB3;

class Comms_UART {

    private:
        int uart0_filestream;
        struct termios options;



        unsigned char tx_buffer[20];
        unsigned char rx_buffer[255];
        int rx_length;
        unsigned char *p_tx_buffer;
        int count;
    public:
        Comms_UART();
        ~Comms_UART();

        void solicitacao(unsigned char codigoProtocolo);
        void enviar(int valorEnviado);
        void enviar(float valorEnviado);
        void enviar(std::string valorEnviado);
        int convertBufferInteiro(unsigned char *buffer);
        float convertBufferFloat(unsigned char *buffer);
        std::string convertBufferString(unsigned char *buffer);


};

#endif