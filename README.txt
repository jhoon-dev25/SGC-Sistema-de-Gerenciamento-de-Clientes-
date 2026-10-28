Sistema de Gerenciamento de Clientes
Disciplina: GAC124 - Introdução aos Algoritmos
Turma: 10R1
Discentes: Jhônata de Oliveira Marques e Henrique César Carvalho Aguiar

Arquivos
- main.cpp: código-fonte da aplicação.
- clientes.csv: base textual inicial com 100 registros.
- clientes.dat: base binária, carregada prioritariamente quando existe.

Compilação (Linux)
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp -o sgc

Execução
./sgc

Execute o programa na pasta que contém clientes.dat e clientes.csv. Se clientes.dat existir, ele será carregado primeiro; caso contrário, o programa tenta importar clientes.csv. Inclusões, alterações e remoções são gravadas durante as operações.
