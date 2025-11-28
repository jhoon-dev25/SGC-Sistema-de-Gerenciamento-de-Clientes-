# Documentação do algoritmo

## Objetivo geral
O Sistema de Gerenciamento de Clientes é uma aplicação de terminal que mantém cadastros em um arquivo binário ordenado, permitindo importar dados iniciais de CSV e garantindo operações de inclusão, consulta, atualização e exclusão com ordenação e busca eficientes. O código foi escrito em C++ puro, privilegiando estruturas simples (arrays e structs) e algoritmos implementados manualmente.

## Estrutura de dados
- **Cliente**: estrutura com campos textuais (nome, endereço, documento), numéricos (ID sequencial, ano de nascimento, limite de crédito) e categóricos (tipo, sexo, estado civil, situação cadastral). Cada registro ocupa tamanho fixo para facilitar gravação binária.
- **BaseClientes**: controla o vetor dinâmico de clientes, o tamanho utilizado, a capacidade alocada e o próximo ID a ser atribuído. A capacidade cresce em potências de dois para reduzir realocações.

## Armazenamento e persistência
1. **Arquivos de dados**: `clientes.dat` armazena os registros binários em ordem de ID, enquanto `clientes.csv` é usado como fonte/espelho opcional para importação e exportação.
2. **Carregamento**: na inicialização, o programa tenta ler `clientes.dat`; se não existir, importa os registros presentes em `clientes.csv` (ignorando o cabeçalho), ajusta o próximo ID e já grava o arquivo binário ordenado.
3. **Gravação**: antes de salvar, a base é ordenada por ID (Selection Sort) para manter o arquivo sempre consistente. Em seguida, os registros são escritos sequencialmente no binário e um CSV atualizado é exportado.

## Ordenação e buscas
- **Ordenação manual**: há duas rotinas de Selection Sort — uma por ID e outra por nome — que percorrem o vetor trocando a menor chave encontrada para a posição corrente, evitando o uso de bibliotecas prontas de ordenação.
- **Busca binária**: o vetor ordenado por ID é pesquisado com busca binária iterativa. Para consultas por nome, o algoritmo cria uma cópia do vetor, ordena-a por nome e procura o termo com busca binária, preservando a ordem original de gravação.

## Operações de CRUD
- **Listagem**: os registros são ordenados por ID e exibidos em páginas de 10 itens, com atalhos para navegar, editar, remover ou inserir novos clientes.
- **Inserção**: atribui um ID incremental, coleta os campos via leitura interativa, rejeita documentos já cadastrados, garante capacidade do vetor e persiste a base logo após a inclusão.
- **Atualização**: localiza o ID via busca binária, relê todos os campos e previne duplicidade de documento antes de sobrescrever o registro e salvar a base.
- **Remoção**: encontra o índice do cliente, desloca os elementos subsequentes para fechar o espaço e grava novamente o arquivo ordenado.
- **Ações contextuais**: ao exibir um cartão de cliente individual (em buscas ou listagem), o usuário pode editar, remover ou criar um novo registro sem sair do fluxo atual.

## Entrada e validação
As leituras de inteiros, `short`, `float` e caracteres são repetidas até receberem valores válidos. Campos de texto são truncados de forma segura para caber nos buffers fixos. Caracteres são normalizados para maiúsculas, reduzindo erros de digitação em campos categóricos.

## Interface e navegação
O menu principal oferece atalhos para listar, inserir, atualizar, remover e buscar (por ID ou nome), sempre com banners de limpeza de tela e pausas para leitura. O programa finaliza liberando a memória alocada dinamicamente.
