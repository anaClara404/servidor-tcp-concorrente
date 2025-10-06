## Diagrama de Sequência — Comunicação Cliente-Servidor

O diagrama abaixo representa o fluxo básico de mensagens no sistema de **chat TCP concorrente**, mostrando a interação entre múltiplos clientes e o servidor.

```mermaid
Diagrama
    participante C1 como Cliente 1
    participante C2 como Cliente 2
    participante S como Servidor

    Note over C1,S: Início da conexão
    C1->>S: Solicita conexão TCP
    S-->>C1: Confirma conexão (socket aceito)
    C2->>S: Solicita conexão TCP
    S-->>C2: Confirma conexão (socket aceito)

    Note over S: Cada cliente é tratado por uma thread (ClientHandler)

    C1->>S: Envia mensagem "Olá!"
    S-->>C2: Broadcast: "Cliente 1 disse: Olá!"
    S-->>C1: Confirma recebimento da mensagem

    C2->>S: Envia mensagem "Oi, tudo bem?"
    S-->>C1: Broadcast: "Cliente 2 disse: Oi, tudo bem?"
    S-->>C2: Confirma recebimento da mensagem

    Note over S,C1,C2: Logging simultâneo em arquivo (libtslog)

    C1->>S: Envia comando "/quit"
    S-->>C1: Fecha conexão
    S-->>C2: Informa "Cliente 1 saiu do chat"

    Note over S: Thread de C1 é encerrada com segurança
