#include "../inc/Comms_UART.hpp"

Comms_UART::Comms_UART() {

    uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY, O_NDELAY);
    if(uart0_filestream == -1) {
        std::cout << "Erro - Não foi possível iniciar a UART" << '\n';
        throw("Wtf");
    } else {
        std::cout << "UART inicializada!" << '\n';
    }

    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);
}

Comms_UART::~Comms_UART() {
    close(uart0_filestream);
    std::cout << "UART desligada!" << '\n';

}

void Comms_UART::solicitacao(unsigned char codigoProtocolo) {

    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = codigoProtocolo;
    *p_tx_buffer++ = 7;
    *p_tx_buffer++ = 5;
    *p_tx_buffer++ = 4;
    *p_tx_buffer++ = 8;

    count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
    std::cout << (count < 0 ? "UART TX error" : "Mandei a mensagem!") << '\n';

    sleep(1);

    rx_length = read(uart0_filestream, (void*)&rx_buffer, 255);
    if(rx_length < 0) {
        std::cout << "Erro na leitura" << '\n';
    } else if(rx_length == 0) {
        std::cout << "Nenhum dado disponível." << '\n';
    } else {
        rx_buffer[rx_length] = '\0';
        
        std::cout << "Recebi a mensagem!" << '\n';

    switch (codigoProtocolo)
        {
        case SOLICITACAO_INTEIRO:
            std::cout << 
                "Retornou o inteiro: " << 
                convertBufferInteiro(rx_buffer) << 
            '\n';
            break;    
        case SOLICITACAO_FLOAT:
            std::cout << 
                "Retornou o float: " << 
                convertBufferFloat(rx_buffer) << 
            '\n';
            break;    
        case SOLICITACAO_STRING:
            std::cout << 
                "Retornou a string de tamanho " << 
                int(rx_buffer[0]) <<
                " : " <<
                convertBufferString(rx_buffer) << 
            '\n';
            break;    
        default:
            break;
        }



    }


}


void Comms_UART::enviar(int valorEnviado) {

    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = ENVIAR_INTEIRO;
    memcpy(p_tx_buffer, &valorEnviado, sizeof(int));
    p_tx_buffer += sizeof(int);
    *p_tx_buffer++ = 7;
    *p_tx_buffer++ = 5;
    *p_tx_buffer++ = 4;
    *p_tx_buffer++ = 8;

    count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
    std::cout << (count < 0 ? "UART TX error" : "Mandei a mensagem!") << '\n';

    sleep(1);

    rx_length = read(uart0_filestream, (void*)&rx_buffer, 255);
    if(rx_length < 0) {
        std::cout << "Erro na leitura" << '\n';
    } else if(rx_length == 0) {
        std::cout << "Nenhum dado disponível." << '\n';
    } else {
        rx_buffer[rx_length] = '\0';
        
        std::cout << "Recebi a mensagem!" << '\n';
        
        std::cout << 
            "Retornou o inteiro: " << 
            convertBufferInteiro(rx_buffer) << 
        '\n';
    }
}

void Comms_UART::enviar(float valorEnviado) {

    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = ENVIAR_FLOAT;
    memcpy(p_tx_buffer, &valorEnviado, sizeof(float));
    p_tx_buffer += sizeof(float);
    *p_tx_buffer++ = 7;
    *p_tx_buffer++ = 5;
    *p_tx_buffer++ = 4;
    *p_tx_buffer++ = 8;

    count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
    std::cout << (count < 0 ? "UART TX error" : "Mandei a mensagem!") << '\n';

    sleep(1);

    rx_length = read(uart0_filestream, (void*)&rx_buffer, 255);
    if(rx_length < 0) {
        std::cout << "Erro na leitura" << '\n';
    } else if(rx_length == 0) {
        std::cout << "Nenhum dado disponível." << '\n';
    } else {
        rx_buffer[rx_length] = '\0';
        
        std::cout << "Recebi a mensagem!" << '\n';
        
        std::cout << 
            "Retornou o float: " << 
            convertBufferFloat(rx_buffer) << 
        '\n';
    }
}

void Comms_UART::enviar(std::string valorEnviado) {

    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = ENVIAR_STRING;
    *p_tx_buffer++ = (unsigned char)(valorEnviado.size()+1);
    memcpy(p_tx_buffer, valorEnviado.c_str(), valorEnviado.size());
    p_tx_buffer += valorEnviado.size();
    *p_tx_buffer++ = '\0';
    *p_tx_buffer++ = 7;
    *p_tx_buffer++ = 5;
    *p_tx_buffer++ = 4;
    *p_tx_buffer++ = 8;

    count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
    std::cout << (count < 0 ? "UART TX error" : "Mandei a mensagem!") << '\n';

    sleep(1);

    rx_length = read(uart0_filestream, (void*)&rx_buffer, 255);
    if(rx_length < 0) {
        std::cout << "Erro na leitura" << '\n';
    } else if(rx_length == 0) {
        std::cout << "Nenhum dado disponível." << '\n';
    } else {
        rx_buffer[rx_length] = '\0';
        
        std::cout << "Recebi a mensagem!" << '\n';
        
        std::cout << 
            "Retornou a string de tamanho " << 
            int(rx_buffer[0]) <<
            " : " <<
            convertBufferString(rx_buffer) << 
        '\n';
    }
}

int Comms_UART::convertBufferInteiro(unsigned char *buffer) {
    int convertedValue;
    memcpy(&convertedValue, buffer, sizeof(int));
    return convertedValue;
}

float Comms_UART::convertBufferFloat(unsigned char *buffer) {
    float convertedValue;
    memcpy(&convertedValue, buffer, sizeof(float));
    return convertedValue;
}

std::string Comms_UART::convertBufferString(unsigned char *buffer) {
    int sizeBuffer{int(buffer[0])};
    std::string convertedValue((char *)(&buffer[1]), sizeBuffer);
    return convertedValue;
}