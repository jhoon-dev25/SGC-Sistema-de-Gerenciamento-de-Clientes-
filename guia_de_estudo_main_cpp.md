# Guia de estudo e apresentação do main.cpp

Este guia explica a versão revisada do sistema de gerenciamento de clientes. Ele foi preparado para você estudar o código e explicá-lo com suas próprias palavras. A meta não é decorar cada linha: é entender quais dados o programa guarda, como eles passam de uma função para outra e quando são gravados em arquivo.

**Identificação do projeto:** GAC124 - Introdução aos Algoritmos, turma 10R1. Discentes: Jhônata de Oliveira Marques e Henrique César Carvalho Aguiar, conforme o cabeçalho do main.cpp.

## 1. Visão geral do programa

O sistema funciona no terminal e oferece estas operações:

- listar clientes;
- cadastrar um cliente;
- atualizar ou remover um cadastro;
- buscar por ID ou por nome;
- carregar e salvar os dados em arquivos.

Durante a execução, os clientes ficam em um vetor dinâmico. O programa tenta carregar primeiro o arquivo binário clientes.dat. Se ele não existir, importa os dados de clientes.csv. Ao salvar, grava os registros no arquivo binário e também exporta um CSV.

Fluxo geral:

    iniciar
      inicializar a base de clientes
      carregar os registros de arquivo
      repetir
        mostrar o menu
        ler a opção
        executar a operação correspondente
      até a opção de saída
      liberar a memória dinâmica
    terminar

## 2. Relação com os conteúdos da disciplina

O plano de ensino fornecido prevê variáveis e constantes, condicionais, repetições, vetores, busca, ordenação, modularização, registros, arquivos, ponteiros e alocação dinâmica. O programa usa esses assuntos.

Isso indica relação com os conteúdos previstos, mas não prova que cada assunto ou sintaxe já tenha sido explicado em aula, nem que o professor espere exatamente esta implementação. O cronograma do plano coloca alguns conteúdos, como ordenação, redimensionamento e arquivos binários, em datas posteriores. Para falar da sequência de aprendizagem, considere o que foi efetivamente dado em aula e os requisitos do projeto.

## 3. Bibliotecas e começo do arquivo

As bibliotecas incluídas no início fornecem funcionalidades usadas ao longo do programa:

| Biblioteca | Uso no código |
|---|---|
| iostream | cin, cout, cerr e endl para ler e escrever no terminal. |
| cstdio | perror, para informar erros de arquivos. |
| cstdlib | atoi, atof, system e NULL. |
| cstring | strcmp, comparação de textos em vetores de char. |
| fstream | ifstream e ofstream para leitura e gravação de arquivos. |
| string | string e operações como length, empty, find e c_str. |
| iomanip | setw, left, fixed e setprecision para formatar a exibição. |

using namespace std permite escrever string e cout em vez de std::string e std::cout. É uma conveniência de escrita; não é variável nem muda a lógica do programa.

### Constantes

    const char DATA_FILE[] = "clientes.dat";
    const char CSV_FILE[] = "clientes.csv";
    const int MAX_TEXT = 128;

const indica que o valor não deve ser alterado durante a execução.

- DATA_FILE é o nome do arquivo binário.
- CSV_FILE é o nome do arquivo CSV.
- MAX_TEXT é o tamanho reservado para os campos de texto nome e endereço.

Dar nomes a esses valores evita repetir literais pelo código e explica a finalidade de cada um. Se o limite dos campos mudar, por exemplo, pode-se alterar MAX_TEXT em um só lugar. O plano inclui constantes, então há respaldo no conteúdo programático para usá-las. Este é um ponto fácil de defender: diga o valor, o que ele representa e por que deve permanecer fixo.

## 4. Tipos de dados e registros

### Registro Cliente

struct agrupa campos relacionados, mesmo quando eles têm tipos diferentes. Cada variável Cliente representa um cadastro.

| Campo | Tipo | Por que esse tipo |
|---|---|---|
| id | int | Identificador inteiro do cliente. |
| nome_completo | char[128] | Texto em vetor de tamanho fixo. |
| endereco | char[128] | Texto em vetor de tamanho fixo. |
| ano_nascimento | short | Inteiro pequeno usado para o ano. |
| documento | char[32] | Texto com espaço limitado para CPF/CNPJ. |
| tipo_cliente | char | Uma letra, como F ou J. |
| sexo | char | Uma letra de opção. |
| estado_civil | char | Uma letra de opção. |
| limite_credito | float | Número que pode ter casas decimais. |
| situacao_cadastral | char | Uma letra, como A ou I. |

Os campos textuais do registro são vetores de char, não string. O tamanho fixo torna o registro mais simples de gravar como um bloco binário. Esses textos precisam terminar com o caractere especial \0, que marca o fim da sequência. copiar_texto garante espaço para ele.

A ordem e os tipos dos campos fazem parte do formato de clientes.dat. Mudar os campos pode mudar o tamanho ou a interpretação dos registros existentes. A revisão manteve os campos do registro nessa mesma ordem.

### Registro BaseClientes

BaseClientes guarda o estado do vetor dinâmico:

- dados: ponteiro para o primeiro Cliente do vetor;
- tamanho: quantidade de clientes efetivamente cadastrados;
- capacidade: quantidade de posições reservadas;
- proximo_id: ID usado no próximo cadastro;
- solicitar_salvar: sinalizador consultado no fluxo de saída.

Tamanho e capacidade são diferentes. Se tamanho vale 5 e capacidade vale 8, os índices 0 a 4 contêm registros válidos e há três posições reservadas ainda livres.

Em main, BaseClientes base = {}; inicia os campos numéricos em zero e o ponteiro como nulo. Depois base.proximo_id = 1 define o primeiro ID.

## 5. Funções, parâmetros e retornos

As declarações antecipadas, também chamadas de protótipos, anunciam funções que serão definidas mais abaixo. Isso permite chamar uma função antes de o compilador ter visto seu corpo completo.

O código usa:

- BaseClientes &base: referência à mesma base. Alterações feitas na função afetam o objeto original.
- const BaseClientes &base: referência de leitura, sem intenção de alterar a base e sem copiá-la.
- const string &texto: referência de leitura para uma string.
- Cliente *dados: ponteiro para elementos do vetor.
- const Cliente *dados: acesso de leitura aos registros apontados.

Retornos comuns:

- void: a função executa uma tarefa e não devolve valor;
- bool: devolve true ou false para indicar um resultado;
- int: devolve um índice ou -1 quando não há resultado;
- Cliente: devolve um registro preenchido.

## 6. Entrada e preparação de textos

### arquivo_existe

Tenta abrir o caminho recebido com ifstream em modo binário e retorna teste.good(). O resultado informa se a abertura foi bem-sucedida. Isso é usado para decidir se o programa deve carregar clientes.dat.

### ler_linha

Imprime o rótulo e lê uma linha completa com getline. É útil para nomes e endereços, que podem conter espaços.

### ler_inteiro

Repete a leitura até obter um inteiro:

1. cin >> numero tenta converter a entrada para int.
2. cin.fail() verifica se a leitura falhou.
3. Se falhou, cin.clear() remove o estado de erro e cin.ignore(10000, '\n') descarta o resto da linha inválida.
4. Se deu certo, também descarta o restante da linha e retorna o número.

O descarte evita que o restante da entrada numérica atrapalhe a próxima leitura com getline.

### ler_short e ler_float

ler_short reaproveita ler_inteiro e converte o valor para short. ler_float repete a validação de entrada usando uma variável float. O short tem faixa menor que int; o código não valida explicitamente se o ano cabe nessa faixa.

### maiusculo e ler_char

maiusculo converte letras ASCII entre a e z para A e Z. Os comandos do menu podem assim aceitar s ou S, por exemplo. ler_char lê uma linha, retorna \0 se estiver vazia e, caso contrário, devolve a primeira letra em maiúscula. A função não valida se a opção pertence ao conjunto permitido; o menu faz essa verificação depois.

A conversão manual é adequada às letras ASCII das opções. Não é uma conversão completa de maiúsculas/minúsculas para todas as letras acentuadas em UTF-8.

### copiar_texto

Recebe o destino, a origem e o limite disponível. O índice i conta quantos caracteres foram copiados. O laço para antes de ultrapassar limite - 1; no fim, escreve \0. Assim o campo permanece terminado e não ultrapassa seu vetor.

Um vetor char[128] comporta no máximo 127 caracteres de texto mais o terminador \0.

### ler_campo

Lê um campo da linha CSV até encontrar ponto e vírgula ou chegar ao fim. posicao indica onde começar a leitura e é passada por referência para avançar entre chamadas. campo é esvaziado e preenchido com os caracteres do próximo campo.

## 7. Ponteiros, vetor dinâmico e memória

### garantir_capacidade

Recebe a quantidade de posições necessárias. Se a capacidade existente já for suficiente, não muda nada e retorna true. Senão:

1. escolhe capacidade inicial 4 ou começa pela capacidade atual;
2. dobra capacidade_alvo até alcançar a quantidade pedida;
3. cria novo_buffer com new Cliente[capacidade_alvo];
4. copia nele os tamanho registros válidos;
5. libera o vetor antigo com delete[];
6. atualiza dados e capacidade.

novo_buffer guarda temporariamente o endereço da nova área. O índice i percorre apenas os clientes válidos. A capacidade pode ser maior do que tamanho para evitar reservar memória a cada inclusão.

Ponto importante desta versão: a função retorna bool, mas, quando termina normalmente, retorna sempre true. Ela não trata a falta de memória de new[] com uma recuperação própria. Assim, os if que verificam o retorno não são uma proteção contra falha de alocação.

### destruir_base

Libera com delete[] a área que foi reservada com new[] e zera o ponteiro, tamanho e capacidade. É chamada no encerramento. O par correto é new[] com delete[].

## 8. Ordenação

### ordenar_por_id

Implementa Selection Sort crescente por ID:

1. i marca a posição a preencher nesta rodada.
2. indice_min começa como a posição i.
3. j examina as posições seguintes em busca de um ID menor.
4. Ao encontrar um menor, atualiza indice_min.
5. Se o menor estiver em outra posição, temp guarda um dos registros enquanto os dois são trocados.

Depois de cada rodada, o menor ID do trecho não ordenado fica na posição i. O algoritmo é escrito com laços e comparações, sem uma função pronta de ordenação.

### ordenar_por_nome

Também é Selection Sort, comparando nome_completo com strcmp. Essa comparação diferencia maiúsculas de minúsculas.

### comparar_texto

Percorre dois textos caractere a caractere, converte cada caractere ASCII para maiúscula e compara. Retorna -1 se o primeiro texto precede o segundo, 1 se sucede, e 0 se forem iguais segundo essa comparação. É usada pela busca binária por nome.

Atenção: ordenar_por_nome usa strcmp, enquanto a busca binária usa comparar_texto, que não diferencia capitalização ASCII. A busca binária precisa que a ordenação e a comparação sigam a mesma regra. Nomes com diferenças de maiúsculas/minúsculas podem expor essa inconsistência. Se perguntarem, reconheça-a; não diga que os comparadores são iguais.

## 9. IDs e buscas

### atualizar_proximo_id

Percorre os clientes e guarda em maior o maior ID encontrado. Define proximo_id como maior + 1. Se a base estiver vazia, maior continua 0 e o próximo ID fica 1.

Como calcula com base nos registros ainda existentes, se o maior ID for removido, esse número pode ser usado de novo. Não existe um histórico separado dos IDs já atribuídos.

### encontrar_indice_por_id

Ordena a base por ID e chama busca_binaria_id. A ordenação é necessária porque busca binária depende de dados ordenados pela chave que está sendo procurada. Retorna o índice ou -1.

### busca_binaria_id

inicio e fim delimitam a parte que ainda pode conter o alvo. meio é o ponto central:

- se o ID central for o alvo, retorna meio;
- se o ID central for menor, continua na metade da direita;
- se for maior, continua na metade da esquerda;
- se o intervalo acabar, retorna -1.

Em cada repetição, o trecho possível diminui aproximadamente pela metade.

### busca_binaria_nome

Ordena por nome o vetor recebido e busca o termo usando comparar_texto. A função pode alterar esse vetor. buscar_por_nome passa uma cópia para que a ordem da base original não seja alterada por essa ordenação.

## 10. Arquivos

### CSV e binário

clientes.csv é texto com colunas separadas por ponto e vírgula e pode ser inspecionado em um editor. clientes.dat armazena bytes de registros Cliente para leitura direta pelo programa.

O binário depende da representação dos tipos, do alinhamento dos campos e da arquitetura. É prático para o mesmo programa em ambiente compatível, mas não é automaticamente portátil entre compiladores e máquinas diferentes.

### salvar_csv

Abre o arquivo com ios::trunc, que começa uma nova gravação apagando o conteúdo anterior. Escreve o cabeçalho e uma linha por cliente. fixed e setprecision(2) formatam o limite com duas casas decimais. A função verifica a abertura e o estado do arquivo após gravar.

### importar_de_csv

Abre o CSV; se não conseguir, considera-o opcional e retorna true. Lê linha a linha. primeira permite reconhecer o cabeçalho id; na primeira linha. pular_linha decide se a linha será ignorada. Linhas vazias também são ignoradas.

Para cada linha de dados, posicao começa em zero. ler_campo retira os dez campos, um a um. atoi converte texto em inteiro; atof converte texto em número decimal. copiar_texto limita o tamanho dos campos textuais. O registro cli é então acrescentado ao vetor, ampliando-o se necessário.

O parser implementa um CSV simples: não trata campos entre aspas nem ponto e vírgula escapado dentro do texto. O formato esperado precisa seguir a estrutura simples gerada por salvar_csv.

### carregar_clientes

A prioridade é:

1. Se clientes.dat existir, tenta abri-lo em modo binário.
2. Lê registros completos com read e acrescenta cada um ao vetor.
3. Atualiza o próximo ID e retorna.
4. Se não houver binário, importa do CSV.
5. Se a importação trouxe registros, chama salvar_clientes para criar o binário e exportar o CSV.

Se o binário existir, ele tem prioridade. O programa não verifica se o CSV está mais recente.

### salvar_clientes

Ordena os registros por ID, abre clientes.dat em modo binário e truncado, e grava cada Cliente com write. A expressão (char *)&base.dados[i] fornece o endereço do registro como sequência de bytes. sizeof(Cliente) informa o número de bytes do registro.

Depois verifica erros, atualiza proximo_id e chama salvar_csv. As operações de inclusão, edição e remoção chamam essa função imediatamente.

## 11. Operações de cadastro

CRUD significa criar, consultar, atualizar e remover dados.

### existe_documento

Percorre a base com i e compara o documento de cada registro com o documento recebido. Retorna true ao achar uma repetição; se terminar o laço, retorna false. É uma busca sequencial para evitar duplicidade.

### ler_dados_cliente

Inicializa c zerado, define seu ID e pergunta os campos. nome, endereco e documento guardam temporariamente os textos lidos como string. copiar_texto os transfere com limites para os vetores char do registro. A função devolve o Cliente completo.

### inserir_cliente

Usa proximo_id para montar o novo registro. Verifica se o documento já existe. Depois garante espaço, coloca novo na posição tamanho, aumenta tamanho e proximo_id e salva. Se a gravação retornar false, diminui tamanho e proximo_id para desfazer a inclusão em memória. Em sucesso, marca solicitar_salvar e mostra a mensagem de conclusão.

### atualizar_cliente

Lê o ID, procura seu índice e mostra erro se não existir. Lê os novos dados preservando o ID. O laço verifica se o documento novo já pertence a outro registro. mesmo_indice evita rejeitar o documento quando ele continua pertencendo ao próprio cliente que está sendo atualizado. Depois substitui o registro e salva.

### remover_cliente

Lê o ID e localiza a posição. O laço copia cada registro seguinte uma posição para a esquerda. Decrementa tamanho e salva. A capacidade do vetor permanece igual, mas há um registro válido a menos.

### editar_por_indice e remover_por_indice

São versões para quando a função chamadora já tem o índice do cliente. editar_por_indice repete a validação de documento e a substituição. remover_por_indice repete o deslocamento e a diminuição do tamanho. A repetição permite chamar essas ações pelo menu contextual além do menu principal.

### manipular_cliente

Mostra ações para o registro indicado por indice. switch escolhe editar, remover, cadastrar outro ou voltar. O laço for (;;) repete quando a opção é inválida. Uma opção reconhecida retorna da função.

### imprimir_cartao

Mostra os campos de um cliente. setw define larguras mínimas; left alinha o conteúdo à esquerda. fixed e setprecision(2) mostram o crédito com duas casas. Esses manipuladores afetam a apresentação, não os valores guardados.

### listar_clientes

Mostra mensagem se não há clientes. Caso contrário, ordena por ID e pagina os resultados.

- por_pagina vale 10.
- indice marca o início da página.
- ate calcula o limite final e é reduzido se ultrapassar tamanho.
- i percorre a página.
- opcao guarda a resposta; acao guarda a primeira letra em maiúscula.
- P avança, E busca um ID para edição, R busca um ID, N inicia cadastro e outras opções saem da listagem.

Atenção: os ramos E e R chamam manipular_cliente. Logo, R na listagem abre o menu contextual em vez de remover diretamente. A opção de remoção do menu principal chama remover_cliente.

### buscar_por_id

Lê o ID, procura e, se encontrar, imprime o cartão e mostra o menu contextual. Se não encontrar, informa isso.

### buscar_por_nome

Lê o termo, aloca uma cópia dinâmica da base e copia os clientes para ela. A busca ordena essa cópia por nome. Se encontrar um cadastro, usa seu ID para localizar o registro correspondente na base original antes de abrir o menu contextual. Ao terminar, libera a cópia com delete[].

## 12. Interface e função principal

### pausar

Mostra uma instrução e ignora caracteres até a quebra de linha. Serve para dar tempo de ler a tela antes do próximo menu.

### limpar_tela

O pré-processador verifica _WIN32: no Windows chama cls; caso contrário, clear. system executa o comando do sistema operacional. A função depende de esses comandos estarem disponíveis.

### desenhar_banner e exibir_menu

desenhar_banner limpa a tela e imprime o título com separadores. exibir_menu chama essa função e escreve as opções. A modularização evita repetir o desenho do cabeçalho.

### main

1. Cria a base zerada e inicia proximo_id em 1.
2. Chama carregar_clientes. Se houver falha, termina com retorno 1.
3. Inicia opcao em -1 para entrar no while.
4. Enquanto opcao não for 0, exibe o menu e lê a opção.
5. switch chama a função correspondente e pausa a tela nas opções de 1 a 6.
6. Na opção 0, entrada_valida controla o laço de resposta; resposta guarda a letra digitada e escolha sua versão maiúscula.
7. Se salvar falhar, opcao volta a -1 para manter o programa no laço. Se salvar funcionar, mostra confirmação. Ao final, destrói a base e retorna 0.

## 13. Uma inconsistência importante na saída

As operações de cadastro, edição e remoção chamam salvar_clientes imediatamente, mas também marcam solicitar_salvar como true. Ao sair, o sistema pergunta se quer salvar de novo. Responder N não desfaz as alterações já gravadas. Portanto, a mensagem “Saindo sem salvar alterações” não descreve com precisão o efeito: as operações já foram persistidas.

Se perguntarem, explique o comportamento real. Não diga que N desfaz operações.

## 14. Pseudocódigo dos algoritmos centrais

Aumentar o vetor:

    se a capacidade já for suficiente:
        terminar
    escolher capacidade maior
    criar novo vetor
    copiar os registros válidos
    liberar vetor antigo
    atualizar ponteiro e capacidade

Selection Sort por ID:

    para cada posição i até a penúltima:
        supor que i contém o menor ID restante
        procurar o menor ID entre i e o fim
        trocar o menor com a posição i

Busca binária:

    inicio = primeira posição
    fim = quantidade de registros
    enquanto inicio < fim:
        meio = centro do intervalo
        se o elemento central é o alvo: devolver meio
        se o elemento central é menor: continuar à direita
        caso contrário: continuar à esquerda
    devolver -1

Remover do vetor:

    localizar o índice
    deslocar os seguintes uma posição à esquerda
    diminuir tamanho em 1
    salvar os registros restantes

## 15. Perguntas prováveis

**Por que há tamanho e capacidade?**  
Tamanho conta os registros ocupados; capacidade conta as posições reservadas. Separá-los permite usar o espaço disponível sem realocar a cada inclusão.

**Por que usar struct?**  
Para agrupar os campos de tipos diferentes que descrevem o mesmo cliente.

**Por que ordenar antes da busca binária?**  
Porque a busca binária só descarta metades corretamente se o vetor estiver ordenado pela chave procurada.

**Por que buscar o nome em uma cópia?**  
Porque a busca por nome ordena o vetor. A cópia preserva a ordem da base principal para outras operações.

**Por que as constantes no começo?**  
Elas dão nomes a valores fixos, centralizam configurações e evitam números e nomes de arquivo repetidos. A disciplina inclui constantes.

**Por que getline para o nome?**  
Para receber a linha inteira, incluindo espaços.

**Para que servem cin.clear e cin.ignore?**  
clear permite novas leituras após uma falha; ignore descarta a entrada inválida que restou na linha.

**O que significa -1 na busca?**  
É um indicador de que nenhum índice válido foi encontrado.

**Por que new[] e delete[]?**  
O vetor é reservado dinamicamente porque sua capacidade pode crescer. delete[] libera a área no encerramento ou quando ela é substituída.

**CSV e binário são a mesma coisa?**  
Não. CSV é texto delimitado, binário é uma sequência de bytes na representação do registro.

**O binário é portátil?**  
Não necessariamente. Depende dos tamanhos e da representação dos tipos e do alinhamento usados pelo compilador e pela máquina.

**A busca por nome é garantida em qualquer capitalização?**  
Não. A ordenação usa strcmp e a busca usa comparação sem distinção de caixa ASCII. Essa diferença pode causar falha em casos de nomes com capitalização diferente.

**Escolher N ao sair desfaz minhas alterações?**  
Não. As operações CRUD já chamam o salvamento imediatamente; N não restaura arquivos anteriores.

## 16. Roteiro para apresentar

Uma apresentação pode seguir estes passos:

1. Objetivo: explicar que o programa gerencia cadastros no terminal.
2. Representação: mostrar Cliente como registro e BaseClientes como controle do vetor.
3. Entrada: explicar getline, validação numérica e cópia limitada dos textos.
4. Memória: descrever tamanho, capacidade, new[], cópia e delete[].
5. Algoritmos: explicar Selection Sort e busca binária.
6. Persistência: comparar CSV texto com DAT binário e mostrar a prioridade de carregamento.
7. Operações: resumir inclusão, busca, atualização e remoção.
8. Limitações: reconhecer a regra de comparação dos nomes, o parser CSV simples e o salvamento imediato seguido de pergunta redundante.

É melhor explicar a lógica com um exemplo pequeno do que ler o código linha a linha. Por exemplo, para a busca binária, use cinco IDs ordenados e mostre como o intervalo diminui.

## 17. Compilar e executar

Compile:

    g++ -std=c++11 -Wall -Wextra -pedantic main.cpp -o main

Execute dentro do diretório que contém os arquivos de dados:

    ./main

O programa procura clientes.dat e clientes.csv no diretório de execução atual, não necessariamente na pasta onde main.cpp está salvo.

## 18. Glossário

| Termo | Significado |
|---|---|
| Registro | Agrupamento de campos diferentes, como Cliente. |
| Vetor | Sequência de elementos acessados por índices começando em zero. |
| Vetor dinâmico | Vetor alocado durante a execução, que pode ser trocado por outro maior. |
| Ponteiro | Variável que guarda um endereço; dados aponta para o vetor. |
| Índice | Posição de um elemento no vetor. |
| Capacidade | Número de posições reservadas. |
| Selection Sort | Ordenação que escolhe o menor elemento restante e o troca para a posição atual. |
| Busca binária | Busca que reduz o intervalo possível pela metade a cada comparação. |
| Persistência | Gravação de dados para que sobrevivam ao encerramento. |
| CSV | Arquivo de texto com campos separados por delimitadores. |
| Binário | Arquivo que guarda bytes do registro na representação do programa. |
| CRUD | Criar, consultar, atualizar e remover. |
| Referência | Parâmetro que dá acesso ao objeto original. |
| \0 | Caractere que marca o fim de um texto em vetor de char. |

