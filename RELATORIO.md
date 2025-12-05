# Relatório Técnico do Sistema de Gerenciamento de Clientes

## 1. Propósito e escopo
Este relatório apresenta, de forma formal e concisa, a arquitetura, os algoritmos e os procedimentos operacionais do **Sistema de Gerenciamento de Clientes** implementado em C++ para ambiente de terminal. O documento busca registrar o estado atual da solução, justificar decisões de projeto, evidenciar mecanismos de qualidade e indicar evoluções recomendadas.

## 2. Visão geral das funcionalidades
O sistema permite cadastrar, consultar, editar e remover clientes, mantendo consistência entre a memória principal e o arquivo binário `clientes.dat`. Há suporte para exportação em CSV, ordenação por identificador ou por nome e consultas eficientes por meio de busca binária. Todas as interações são conduzidas por menu textual, com validação de entradas e mensagens claras ao usuário.

## 3. Arquitetura e estruturas de dados
- **Estrutura `Cliente`**: agrupa identificador incremental, nome, endereço, ano de nascimento, documento, tipo de cliente, sexo, estado civil, limite de crédito e situação cadastral. Campos textuais utilizam buffers de tamanho fixo, facilitando a serialização binária.
- **Estrutura `BaseClientes`**: mantém ponteiro para vetor dinâmico de clientes, tamanho lógico, capacidade alocada e próximo identificador disponível. Essa organização permite expansão controlada sem realocações frequentes.
- **Separação de responsabilidades**: funções utilitárias cuidam de leitura e validação de entradas; rotinas específicas tratam ordenação, busca, manipulação de registros e persistência, favorecendo testes isolados e manutenção.

## 4. Persistência e integridade
- **Arquivo binário principal (`clientes.dat`)**: armazena registros ordenados por `id`, garantindo compatibilidade com busca binária e reconstrução da base na inicialização.
- **Exportação CSV (`clientes.csv`)**: disponibiliza dados em formato tabular para integração externa e auditoria, convertendo tipos primitivos e caracteres de classe em colunas legíveis.
- **Garantia de consistência**: após inserção, edição ou remoção, a base em memória é reordenada e sincronizada com o arquivo binário. Falhas de E/S são reportadas de forma descritiva, preservando o estado anterior em caso de erro.

## 5. Algoritmos e desempenho
- **Ordenação**: utiliza *selection sort* para organizar registros tanto por `id` quanto por `nome`. Embora a complexidade seja O(n²), o algoritmo atende ao volume acadêmico previsto e evita dependências externas.
- **Busca**: aplica **busca binária** sobre vetores ordenados, reduzindo o tempo de localização para O(log n) e mantendo previsibilidade mesmo com conjuntos maiores.
- **Gestão de memória**: a expansão de capacidade dobra o buffer quando necessário, amortizando realocações e preservando a validade dos ponteiros antes da gravação em disco.

## 6. Interface e experiência do usuário
- **Menu textual**: organiza operações em opções numeradas, com separadores e títulos que favorecem leitura em terminais simples.
- **Validação de entrada**: campos numéricos são convertidos com tratamento de erros e repetição de prompt; caracteres de classe são normalizados; textos são truncados com *null-termination* garantida.
- **Operação assistida**: funções utilitárias `limpar_tela` e `pausar` ajudam o usuário a acompanhar mensagens e confirmações, independentemente do ambiente de execução.

## 7. Qualidade e verificações
- **Compilação estrita**: o projeto é compilado com `g++ -std=c++17 -Wall -Wextra -Werror`, prevenindo avisos silenciosos e garantindo conformidade ao padrão.
- **Testes de fumaça**: a execução manual do binário cobre o ciclo completo de cadastro, edição, exclusão e exportação, confirmando a integridade da persistência binária/CSV.

## 8. Riscos e limitações
- A ordenação quadrática pode se tornar gargalo em bases extensas; para cenários maiores, recomenda-se adoção de algoritmos O(n log n), como *quicksort* ou *mergesort*.
- Buffers de tamanho fixo simplificam a serialização, mas limitam o comprimento dos campos e podem truncar entradas extensas.
- O sistema não contempla autenticação nem controle de acesso, pois o escopo atual é acadêmico e monousuário.

## 9. Recomendações de evolução
- Adicionar testes automatizados para inserção, busca, edição e remoção, evitando regressões funcionais.
- Migrar a persistência para mecanismo estruturado (por exemplo, SQLite) se houver necessidade de consultas complexas ou acesso concorrente.
- Substituir *selection sort* por algoritmo mais eficiente e aprimorar a interface para suportar internacionalização e configuração de formatos.

## 10. Conclusão
O sistema cumpre o objetivo de prover gerenciamento confiável de clientes em ambiente de terminal, com persistência consistente, validação de dados e algoritmos compatíveis com o escopo. Este relatório documenta o estado atual da solução e serve de referência para manutenção contínua e melhorias futuras.
