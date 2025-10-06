# Servidor TCP Concorrente — Programação Concorrente e Distribuída

## Objetivo
Desenvolver um sistema de **servidor de chat multiusuário TCP concorrente**, em C++, aplicando conceitos de programação concorrente (threads, exclusão mútua, sockets e logging thread-safe).  

O projeto é dividido em etapas:  
- **Etapa 1** → Biblioteca de logging thread-safe (`libtslog`) + programa de teste.  
- **Etapa 2** → Protótipo de cliente/servidor TCP com comunicação básica, logging integrado e scripts de teste com múltiplos clientes.  

---

## Etapa 1 — Logging Thread-Safe

### Implementações
- **Biblioteca `libtslog`**:  
  - Suporte a logs concorrentes.  
  - Fila protegida por `mutex` + `condition_variable`.  
  - Thread dedicada para escrita em arquivo/console.  
  - Níveis de log: `DEBUG`, `INFO`, `WARN`, `ERROR`.  

- **Programa de teste (`tslog_test`)**:  
  - Cria múltiplas threads simulando geração de logs em paralelo.  
  - Permite configurar número de threads, quantidade de mensagens e saída para arquivo/console.  

### Execução do Teste
Compilar:
```bash
make
```
Rodar teste:
```bash
./tslog_test [nthreads] [nmsgs] [logfile] [consoleOutput]
```

---

## Etapa 2 — Protótipo de Comunicação (Chat TCP)

### Implementações
- **Servidor (`server`)**  
  - Aceita conexões TCP concorrentes (uma thread por cliente).  
  - Mantém lista de clientes conectados.  
  - Mensagens recebidas são retransmitidas (broadcast) para os demais clientes.  
  - Logging integrado (`libtslog`) registra conexões, desconexões e mensagens.  

- **Cliente (`client`)**  
  - Conecta-se ao servidor via CLI.  
  - Duas threads:  
    - uma para enviar mensagens digitadas pelo usuário;  
    - outra para receber e exibir mensagens.  
  - Comandos: `/quit` ou `/exit` para sair.  

- **Cliente Simulado (`sim_client`)**  
  - Envia mensagens automaticamente, sem interação do usuário.  
  - Parâmetros: identificador, quantidade de mensagens, delay entre mensagens (ms).  

- **Scripts de Teste**  
  - `run_server.sh` → inicia o servidor.  
  - `run_client.sh` → inicia cliente interativo.  
  - `stress_test.sh` → simula vários clientes simultâneos.  

---

### Execução

#### 1. Iniciar o servidor
```bash
./server 3333 server.log
```
### 2. Iniciar um cliente interativo
```bash
./client 127.0.0.1 3333 client.log
```
Digite mensagens no prompt e, para sair, digite
```bash
\quit
```
### 3. Simular clientes automáticos em background
```bash
./sim_client c1 5 200 &
```
- O cliente c1 envia 5 mensagens, com intervalo de 200 ms entre elas.
### 4. Teste de estresse com script
Simular vários clientes de uma vez:
```bash
./scripts/stress_test.sh 5 10 100
```
Simula 5 clientes, cada um enviando 10 mensagens, com 100 ms de intervalo.

## Resultados
- **Cliente/servidor funcionando em rede.**
- **Logging integrado em todos os componentes.**
- **Scripts de teste permitem simular múltiplos clientes facilmente.**
- **Servidor processa conexões concorrentes, registra mensagens e remove clientes desconectados com segurança.**
