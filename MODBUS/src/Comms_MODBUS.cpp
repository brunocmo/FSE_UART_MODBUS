#include "../inc/Comms_MODBUS.hpp"

Comms_MODBUS::Comms_MODBUS() {

    uart0_filestream = -1;
    matricula[0] = 7;
    matricula[1] = 5;
    matricula[2] = 4;
    matricula[3] = 8;
    

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

Comms_MODBUS::~Comms_MODBUS() {
    close(uart0_filestream);
    std::cout << "UART desligada!" << '\n';

}

void Comms_MODBUS::solicitacao(unsigned char codigoProtocolo) {
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = ESP32;   
    *p_tx_buffer++ = PEDIR;
    *p_tx_buffer++ = codigoProtocolo;
    memcpy(p_tx_buffer, matricula, 4);
    p_tx_buffer += 4;

    crcValue = calcula_CRC( &tx_buffer[0], 7);
    memcpy(crcMessage, &crcValue, sizeof(short));

    *p_tx_buffer++ = crcMessage[0];
    *p_tx_buffer++ = crcMessage[1];


    // for(int i{0}; i<9; i++) {
    //     std::cout << std::hex << int(tx_buffer[i]) << '\n';
    // }

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
        
        std::cout << "Recebi a mensagem! com " << rx_length << " chars! " <<  '\n';

        if( validarCRC(rx_buffer, rx_length-2)) throw("CRC invalido!");

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
                int(rx_buffer[3]) <<
                " : " <<
                convertBufferString(rx_buffer) << 
            '\n';
            break;    
        default:
            break;
        }
    }


}

void Comms_MODBUS::enviar(int valorEnviado) {
    unsigned char bufferLocal[4];
    memcpy(bufferLocal, &valorEnviado, sizeof(int));
    enviarBuffer(1, bufferLocal);
}

void Comms_MODBUS::enviar(float valorEnviado) {
    unsigned char bufferLocal[4];
    memcpy(bufferLocal, &valorEnviado, sizeof(float));
    enviarBuffer(2, bufferLocal);
}

void Comms_MODBUS::enviar(std::string valorEnviado) {
    unsigned char bufferLocal[249];
    unsigned char *ptrBuffer;

    ptrBuffer = &bufferLocal[0];

    *ptrBuffer++ = int(valorEnviado.size());
    memcpy(ptrBuffer, valorEnviado.c_str(), valorEnviado.size());
    enviarBuffer(3, bufferLocal);
}

int Comms_MODBUS::convertBufferInteiro(unsigned char *buffer) {
    int convertedValue;
    memcpy(&convertedValue, &buffer[3], sizeof(int));
    return convertedValue;
}

float Comms_MODBUS::convertBufferFloat(unsigned char *buffer) {
    float convertedValue;
    memcpy(&convertedValue, &buffer[3], sizeof(float));
    return convertedValue;

}

std::string Comms_MODBUS::convertBufferString(unsigned char *buffer) {
    int sizeBuffer{int(buffer[3])};
    std::string convertedValue((char *)(&buffer[4]), sizeBuffer);    

    return convertedValue;
}

bool Comms_MODBUS::enviarBuffer(int flag, unsigned char *buffer) {

    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = ESP32;   
    *p_tx_buffer++ = ENVIAR;

    switch (flag)
    {
    case 1:
        *p_tx_buffer++ = ENVIAR_INTEIRO;
        memcpy(p_tx_buffer, buffer, sizeof(int));
        p_tx_buffer += sizeof(int);
        crcValue = calcula_CRC( &tx_buffer[0], 7);
        break;
    case 2:
        *p_tx_buffer++ = ENVIAR_FLOAT;
        memcpy(p_tx_buffer, buffer, sizeof(float));
        p_tx_buffer += sizeof(float);
        crcValue = calcula_CRC( &tx_buffer[0], 7);
        break;
    case 3:
        *p_tx_buffer++ = ENVIAR_STRING;
        std::cout << int(buffer[0]) << " teste\n";
        memcpy(p_tx_buffer, buffer, int(buffer[0])+1);
        p_tx_buffer += int(buffer[0])+1;
        crcValue = calcula_CRC( &tx_buffer[0], int(buffer[0])+4);
        break;
    
    default:
        throw("Erro Operacao -> modoEnvio");
        break;
    }

    memcpy(crcMessage, &crcValue, sizeof(short));

    *p_tx_buffer++ = crcMessage[0];
    *p_tx_buffer++ = crcMessage[1];

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
        
        if( validarCRC(rx_buffer, rx_length-2)) throw("CRC invalido!");

        switch (flag)
        {
        case 1:
            std::cout << 
                "Retornou o inteiro: " << 
                convertBufferInteiro(rx_buffer) << 
            '\n';
            break;
        case 2:
            std::cout << 
                "Retornou o float: " << 
                convertBufferFloat(rx_buffer) << 
            '\n';
            break;
        case 3:
            std::cout << 
                "Retornou a string de tamanho " << 
                int(rx_buffer[0]) <<
                " : " <<
                convertBufferString(rx_buffer) << 
            '\n';
            break;
        
        default:
            throw("Erro Operacao -> modoEnvio");
            break;
        }
    }

    return EXIT_SUCCESS;
}

bool Comms_MODBUS::validarCRC(unsigned char *bufferValidacao, size_t tamanho) {
    short crcCheck;

    crcValue = calcula_CRC( &bufferValidacao[0], tamanho);
    memcpy(&crcCheck, &bufferValidacao[tamanho], sizeof(short));

    if (crcCheck == crcValue) {
        std::cout << "CRC validada e aceita!" << '\n';
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }

}