#include <iostream>
#include <Comms_MODBUS.hpp>

int main() {

  try{
  Comms_MODBUS teste;
  teste.solicitacao(SOLICITACAO_INTEIRO);
  teste.solicitacao(SOLICITACAO_FLOAT);
  teste.solicitacao(SOLICITACAO_STRING);
  teste.enviar(42);
  teste.enviar(4.20f);
  teste.enviar("=> LUCAS 10 Reais a monitoria <=");

  }
  catch(const char* exception ) {
    std::cerr << exception << '\n';
  }

  return 0;

}
