## Relatório de Análise com IA

Durante o desenvolvimento do projeto **Servidor TCP Concorrente**, foi fundamental compreender e aplicar corretamente diversos conceitos de **programação concorrente** e **comunicação em rede**.  
A Inteligência Artificial (IA) foi utilizada como ferramenta de apoio em diferentes momentos do processo, auxiliando na resolução de dúvidas teóricas e práticas, bem como na melhoria da estrutura do código.

---

### 1. Compreensão de Threads e Concorrência

No início do projeto, uma das minhas principais dúvidas era **como gerenciar múltiplas threads de forma segura**, especialmente no contexto de um servidor que aceita várias conexões simultâneas.  
Eu sabia que cada cliente deveria ser tratado em uma thread própria, mas não entendia bem como evitar problemas como **condições de corrida** e **deadlocks**.

> A IA me explicou que cada `ClientHandler` deveria rodar em sua própria thread, mas que o acesso a recursos compartilhados — como a lista de clientes conectados — deveria ser protegido com `std::mutex`.  
> Também aprendi sobre o uso de `std::lock_guard` para garantir liberação automática do bloqueio, mesmo em caso de exceção.

Com essa orientação, implementei o controle de acesso na função `broadcast()` do servidor, evitando que duas threads escrevessem simultaneamente nos sockets ou alterassem a lista de conexões em paralelo.

---

### 2. Uso de Logging Thread-Safe

A primeira etapa do projeto envolveu criar uma **biblioteca de logging concorrente** (`libtslog`).  
No começo, eu tinha dúvidas sobre **como tornar o logger thread-safe sem travar o desempenho** do sistema.  
A IA sugeriu o uso de uma **fila protegida por `mutex` e `condition_variable`**, processada por uma thread dedicada para escrita em arquivo.

> Essa explicação me ajudou a entender que, ao invés de cada thread escrever diretamente no arquivo, elas poderiam apenas colocar mensagens em uma fila — evitando contenção e garantindo desempenho mesmo com várias threads ativas.

Essa arquitetura foi essencial para o restante do projeto, pois o logger foi integrado em todos os módulos (`server`, `client`, `client_handler`, `sim_client`) de forma segura e eficiente.

---

### 3. Sincronização e Encerramento Seguro de Threads

Outro ponto de dificuldade foi **como encerrar threads corretamente** sem causar erros de “resource deadlock avoided” ou `std::terminate()`.  
No início, algumas threads eram encerradas abruptamente, gerando falhas ao destruir objetos `std::thread` ainda ativos.

> A IA me explicou que uma thread precisa ser **joinada ou detachada** antes de seu destrutor ser chamado, e que o ideal era o servidor (`Server`) gerenciar explicitamente o ciclo de vida dos `ClientHandler`.  
> Implementamos então o método `stop()` com verificações de segurança (`joinable()` e comparação de `thread_id`) para evitar auto-join e garantir o encerramento limpo.

Com essa correção, o servidor passou a lidar corretamente com a desconexão de clientes, sem causar aborts nem vazamento de recursos.

---

### 4. Simulação de Múltiplos Clientes

A criação do **cliente simulado (`sim_client`)** foi uma etapa de aprendizado prático sobre **concorrência e paralelismo**.  
Eu queria testar o comportamento do servidor com vários clientes simultâneos, mas sem precisar abrir múltiplos terminais manualmente.

> A IA me orientou a usar o operador `&` no terminal para rodar processos em background e também a criar scripts de teste (`stress_test.sh`) que executam diversos `sim_client` em paralelo.  
> Além disso, foi explicada a diferença entre **concorrência (várias tarefas progredindo ao mesmo tempo)** e **paralelismo (várias tarefas executadas exatamente ao mesmo tempo em múltiplos núcleos)**.

Com esses testes automatizados, pude comprovar que o servidor realmente processava conexões e mensagens de forma concorrente, mantendo estabilidade e registrando logs consistentes.

---

### 5. Análise Final e Aprendizados

Ao longo do desenvolvimento, a IA funcionou como uma **assistente técnica e didática**, ajudando não apenas a corrigir erros de código, mas principalmente a entender **por que eles aconteciam**.  
Com esse suporte, aprendi a:

- Controlar o acesso a recursos compartilhados usando `mutex` e `lock_guard`.  
- Implementar logging thread-safe com fila e thread dedicada.  
- Gerenciar corretamente o ciclo de vida de threads (join/detach).  
- Estruturar testes de concorrência com clientes automatizados.  
- Organizar o código modularmente, mantendo o servidor, cliente e logger independentes.  

No final, o sistema resultante é **estável, concorrente e modular**, refletindo uma compreensão sólida dos conceitos de **threads, sincronização e comunicação TCP**.  
A IA contribuiu não apenas na implementação, mas também no **entendimento conceitual** que sustenta todo o projeto.

---

### Conclusão

O projeto permitiu unir teoria e prática de **programação concorrente em sistemas distribuídos**.  
O uso da IA foi essencial para esclarecer dúvidas conceituais, propor melhorias no código e garantir boas práticas de sincronização.  
Mais do que gerar respostas, a interação com a IA me ajudou a **aprender de forma guiada e contextualizada**, resultando em um sistema completo, funcional e seguro.
