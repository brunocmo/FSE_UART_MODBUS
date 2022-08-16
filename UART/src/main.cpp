#include <iostream>
#include "../inc/Comms_UART.hpp"

int main() {

  try{
  Comms_UART teste;
  teste.solicitacao(SOLICITACAO_INTEIRO);
  teste.solicitacao(SOLICITACAO_FLOAT);
  teste.solicitacao(SOLICITACAO_STRING);
  teste.enviar(42);
  teste.enviar(4.20f);
  teste.enviar("Valve pls ban fga");

  }
  catch(std::exception &e) {
    std::cerr << e.what();
  }

  return 0;

}
