## Mapeamento de Requisitos



| Nº | Requisito | Implementação | Arquivo(s) / Classe(s) |
|----|------------|----------------|-------------------------|
| 1 | O sistema deve registrar logs thread-safe acessíveis por múltiplas threads. | Implementação de biblioteca de logging com `mutex` e `condition_variable`. Thread dedicada para escrita em arquivo. | `include/logger.h`, `src/logger.cpp` |
| 2 | O servidor deve aceitar múltiplas conexões simultâneas. | Criação de uma thread por cliente conectado. Uso de `std::thread` e controle de ciclo de vida com `ClientHandler`. | `include/server.h`, `src/server.cpp`, `src/client_handler.cpp` |
| 3 | O servidor deve retransmitir mensagens recebidas (broadcast) para todos os outros clientes. | Função `broadcast()` percorre lista de clientes ativos e envia mensagem a todos os sockets abertos. | `src/server.cpp` |
| 4 | O servidor deve detectar desconexão de clientes e liberar recursos. | Thread de cliente (`ClientHandler`) monitora fim de conexão e remove cliente com `server_->remove_client()`. | `src/client_handler.cpp`, `src/server.cpp` |
| 5 | O cliente deve conectar-se ao servidor via TCP e enviar/receber mensagens. | Uso de `socket()`, `connect()`, `send()` e `recv()`. Duas threads: envio e recebimento. | `src/client.cpp` |
| 6 | O cliente deve permitir encerramento manual da conexão. | Comandos `/quit` e `/exit` encerram a thread e o socket. | `src/client.cpp` |
| 7 | Deve existir cliente automatizado para testes simultâneos. | Cliente simulado (`sim_client`) que envia mensagens automáticas com atraso configurável. | `src/sim_client.cpp` |
| 8 | O sistema deve permitir execução de testes de estresse com múltiplos clientes. | Script `stress_test.sh` executa múltiplos `sim_client` em paralelo. | `scripts/stress_test.sh` |
| 9 | Todos os módulos devem registrar eventos em log (conexão, desconexão, mensagens). | Chamadas ao `Logger::instance().info()/warn()/error()` em cada módulo. | `src/server.cpp`, `src/client_handler.cpp`, `src/client.cpp`, `src/sim_client.cpp` |
| 10 | O sistema deve compilar e rodar em ambiente Linux. | Uso de sockets POSIX (`sys/socket.h`, `arpa/inet.h`, `unistd.h`) e `Makefile`. | `makefile`, `src/*.cpp` |
| 11 | Deve haver documentação explicando execução e arquitetura. | README completo com instruções de build, execução, scripts e diagrama de sequência. | `README.md` |

