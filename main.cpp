#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <limits>
#include <string>
#include <strings.h>
#include <cctype>
#include <sstream>
#include <iomanip>

using namespace std;

// ==============================================================
// Sistema de Cadastro em Arquivos com Ordenação
// Equipe: (preencher nomes e matrículas dos integrantes)
// Data: 2024
// Descrição: Aplicação de terminal para gerenciamento de clientes
// com persistência em arquivo binário ordenado.
// ==============================================================

constexpr const char *DATA_FILE = "clientes.dat";
constexpr const char *CSV_FILE = "clientes.csv";
constexpr size_t MAX_TEXT = 128;

struct Cliente {
    int id = 0;
    char nome_completo[MAX_TEXT]{};
    char endereco[MAX_TEXT]{};
    short ano_nascimento = 0;
    char documento[32]{};
    char tipo_cliente = '\0';
    char sexo = '\0';
    char estado_civil = '\0';
    float limite_credito = 0.0f;
    char situacao_cadastral = '\0';
};

struct BaseClientes {
    Cliente *dados = nullptr;
    size_t tamanho = 0;
    size_t capacidade = 0;
};

// Declarações antecipadas
bool salvar_clientes(BaseClientes &base);
void pausar();
void limpar_tela();
void desenhar_banner(const string &titulo);

// --------------------------------------------------------------
// Utilidades de entrada
// --------------------------------------------------------------

bool arquivo_existe(const char *caminho) {
    if (!caminho) {
        return false;
    }
    ifstream teste(caminho, ios::binary);
    return teste.good();
}

string ler_linha(const string &rotulo) {
    cout << rotulo << ": ";
    string entrada;
    getline(cin, entrada);
    return entrada;
}

int ler_inteiro(const string &rotulo) {
    for (;;) {
        string texto = ler_linha(rotulo);
        try {
            return stoi(texto);
        } catch (...) {
            cout << "Valor inválido. Tente novamente.\n";
        }
    }
}

short ler_short(const string &rotulo) {
    for (;;) {
        string texto = ler_linha(rotulo);
        try {
            return static_cast<short>(stoi(texto));
        } catch (...) {
            cout << "Valor inválido. Tente novamente.\n";
        }
    }
}

float ler_float(const string &rotulo) {
    for (;;) {
        string texto = ler_linha(rotulo);
        try {
            return stof(texto);
        } catch (...) {
            cout << "Valor inválido. Tente novamente.\n";
        }
    }
}

char ler_char(const string &rotulo) {
    cout << rotulo << ": ";
    string linha;
    getline(cin, linha);
    if (linha.empty()) {
        return '\0';
    }
    return static_cast<char>(toupper(static_cast<unsigned char>(linha[0])));
}

void copiar_texto(char *destino, const string &origem, size_t limite) {
    strncpy(destino, origem.c_str(), limite - 1);
    destino[limite - 1] = '\0';
}

// --------------------------------------------------------------
// Gerenciamento de memória dinâmica
// --------------------------------------------------------------

void destruir_base(BaseClientes &base) {
    delete[] base.dados;
    base.dados = nullptr;
    base.tamanho = 0;
    base.capacidade = 0;
}

bool garantir_capacidade(BaseClientes &base, size_t nova_capacidade) {
    if (nova_capacidade <= base.capacidade) {
        return true;
    }

    size_t capacidade_alvo = base.capacidade == 0 ? 4 : base.capacidade;
    while (capacidade_alvo < nova_capacidade) {
        capacidade_alvo *= 2;
    }

    Cliente *novo_buffer = new (nothrow) Cliente[capacidade_alvo];
    if (!novo_buffer) {
        perror("Falha ao alocar memória");
        return false;
    }

    for (size_t i = 0; i < base.tamanho; ++i) {
        novo_buffer[i] = base.dados[i];
    }

    delete[] base.dados;
    base.dados = novo_buffer;
    base.capacidade = capacidade_alvo;
    return true;
}

// --------------------------------------------------------------
// Ordenação manual (Selection Sort)
// --------------------------------------------------------------

void ordenar_por_id(Cliente *dados, size_t quantidade) {
    for (size_t i = 0; i + 1 < quantidade; ++i) {
        size_t indice_min = i;
        for (size_t j = i + 1; j < quantidade; ++j) {
            if (dados[j].id < dados[indice_min].id) {
                indice_min = j;
            }
        }
        if (indice_min != i) {
            Cliente temp = dados[i];
            dados[i] = dados[indice_min];
            dados[indice_min] = temp;
        }
    }
}

void ordenar_por_nome(Cliente *dados, size_t quantidade) {
    for (size_t i = 0; i + 1 < quantidade; ++i) {
        size_t indice_min = i;
        for (size_t j = i + 1; j < quantidade; ++j) {
            if (strcmp(dados[j].nome_completo, dados[indice_min].nome_completo) < 0) {
                indice_min = j;
            }
        }
        if (indice_min != i) {
            Cliente temp = dados[i];
            dados[i] = dados[indice_min];
            dados[indice_min] = temp;
        }
    }
}

// --------------------------------------------------------------
// Persistência em arquivo binário
// --------------------------------------------------------------

bool salvar_csv(const BaseClientes &base) {
    ofstream out(CSV_FILE, ios::trunc);
    if (!out) {
        perror("Não foi possível abrir o CSV para escrita");
        return false;
    }

    out << "id;nome_completo;endereco;ano_nascimento;documento;tipo_cliente;sexo;estado_civil;limite_credito;situacao_cadastral\n";
    out << fixed << setprecision(2);
    for (size_t i = 0; i < base.tamanho; ++i) {
        const Cliente &c = base.dados[i];
        out << c.id << ';' << c.nome_completo << ';' << c.endereco << ';'
            << c.ano_nascimento << ';' << c.documento << ';' << c.tipo_cliente
            << ';' << c.sexo << ';' << c.estado_civil << ';' << c.limite_credito
            << ';' << c.situacao_cadastral << "\n";
    }
    return true;
}

bool importar_de_csv(BaseClientes &base) {
    ifstream in(CSV_FILE);
    if (!in) {
        return true; // CSV opcional
    }

    string linha;
    bool primeira = true;
    while (getline(in, linha)) {
        if (primeira) {
            primeira = false;
            if (linha.find("id;") == 0) {
                continue;
            }
        }
        if (linha.empty()) {
            continue;
        }

        stringstream ss(linha);
        string campo;
        Cliente cli{};

        getline(ss, campo, ';');
        cli.id = stoi(campo);

        getline(ss, campo, ';');
        strncpy(cli.nome_completo, campo.c_str(), MAX_TEXT - 1);

        getline(ss, campo, ';');
        strncpy(cli.endereco, campo.c_str(), MAX_TEXT - 1);

        getline(ss, campo, ';');
        cli.ano_nascimento = static_cast<short>(stoi(campo));

        getline(ss, campo, ';');
        strncpy(cli.documento, campo.c_str(), sizeof(cli.documento) - 1);

        getline(ss, campo, ';');
        cli.tipo_cliente = campo.empty() ? '\0' : campo[0];

        getline(ss, campo, ';');
        cli.sexo = campo.empty() ? '\0' : campo[0];

        getline(ss, campo, ';');
        cli.estado_civil = campo.empty() ? '\0' : campo[0];

        getline(ss, campo, ';');
        cli.limite_credito = stof(campo);

        getline(ss, campo, ';');
        cli.situacao_cadastral = campo.empty() ? '\0' : campo[0];

        if (!garantir_capacidade(base, base.tamanho + 1)) {
            return false;
        }
        base.dados[base.tamanho++] = cli;
    }

    return true;
}

bool carregar_clientes(BaseClientes &base) {
    if (arquivo_existe(DATA_FILE)) {
        ifstream in(DATA_FILE, ios::binary);
        if (!in) {
            perror("Não foi possível abrir o arquivo de dados");
            return false;
        }

        Cliente temp{};
        while (in.read(reinterpret_cast<char *>(&temp), sizeof(Cliente))) {
            if (!garantir_capacidade(base, base.tamanho + 1)) {
                return false;
            }
            base.dados[base.tamanho++] = temp;
        }
        return true;
    }

    if (!importar_de_csv(base)) {
        return false;
    }

    if (base.tamanho == 0) {
        return true;
    }

    return salvar_clientes(base);
}

bool salvar_clientes(BaseClientes &base) {
    // Ordena em memória antes de gravar para manter o arquivo organizado
    ordenar_por_id(base.dados, base.tamanho);

    ofstream out(DATA_FILE, ios::binary | ios::trunc);
    if (!out) {
        perror("Não foi possível abrir o arquivo de dados");
        return false;
    }

    for (size_t i = 0; i < base.tamanho; ++i) {
        out.write(reinterpret_cast<const char *>(&base.dados[i]), sizeof(Cliente));
        if (!out) {
            perror("Falha ao salvar dados");
            return false;
        }
    }
    return salvar_csv(base);
}

// --------------------------------------------------------------
// Operações de busca (binária)
// --------------------------------------------------------------

int busca_binaria_id(const Cliente *dados, size_t quantidade, int alvo) {
    size_t inicio = 0;
    size_t fim = quantidade;
    while (inicio < fim) {
        size_t meio = inicio + (fim - inicio) / 2;
        if (dados[meio].id == alvo) {
            return static_cast<int>(meio);
        }
        if (dados[meio].id < alvo) {
            inicio = meio + 1;
        } else {
            fim = meio;
        }
    }
    return -1;
}

int busca_binaria_nome(Cliente *dados, size_t quantidade, const string &nome) {
    ordenar_por_nome(dados, quantidade);

    size_t inicio = 0;
    size_t fim = quantidade;
    while (inicio < fim) {
        size_t meio = inicio + (fim - inicio) / 2;
        int comparacao = strcasecmp(dados[meio].nome_completo, nome.c_str());
        if (comparacao == 0) {
            return static_cast<int>(meio);
        }
        if (comparacao < 0) {
            inicio = meio + 1;
        } else {
            fim = meio;
        }
    }
    return -1;
}

// --------------------------------------------------------------
// CRUD
// --------------------------------------------------------------

bool existe_id(const BaseClientes &base, int id) {
    // Necessário estar ordenado por ID para busca binária correta
    ordenar_por_id(base.dados, base.tamanho);
    return busca_binaria_id(base.dados, base.tamanho, id) != -1;
}

bool existe_documento(const BaseClientes &base, const string &documento) {
    for (size_t i = 0; i < base.tamanho; ++i) {
        if (documento == base.dados[i].documento) {
            return true;
        }
    }
    return false;
}

Cliente ler_cliente() {
    Cliente c;
    c.id = ler_inteiro("ID (inteiro)");
    string nome = ler_linha("Nome completo");
    string endereco = ler_linha("Endereço");
    string documento = ler_linha("CPF/CNPJ (somente números)");
    c.ano_nascimento = ler_short("Ano de nascimento (AAAA)");
    c.tipo_cliente = ler_char("Tipo de cliente (F/J)");
    c.sexo = ler_char("Sexo (M/F/O)");
    c.estado_civil = ler_char("Estado civil (S/C/V/D)");
    c.limite_credito = ler_float("Limite de crédito");
    c.situacao_cadastral = ler_char("Situação cadastral (A/I)");

    copiar_texto(c.nome_completo, nome, MAX_TEXT);
    copiar_texto(c.endereco, endereco, MAX_TEXT);
    copiar_texto(c.documento, documento, sizeof(c.documento));
    return c;
}

void imprimir_cartao(const Cliente &c) {
    cout << "+------------------------------------------------+\n";
    cout << "| ID: " << setw(6) << left << c.id << " Nome: " << setw(27) << left << c.nome_completo << "|\n";
    cout << "| Documento: " << setw(14) << left << c.documento << " Tipo: " << setw(1) << left << c.tipo_cliente
         << " Sexo: " << setw(1) << left << c.sexo << " Estado Civil: " << setw(1) << left << c.estado_civil << "             |\n";
    cout << "| Limite: R$ " << setw(10) << left << fixed << setprecision(2) << c.limite_credito
         << " Situação: " << setw(1) << left << c.situacao_cadastral << " Ano Nasc.: " << setw(4) << left << c.ano_nascimento << "      |\n";
    cout << "| Endereço: " << setw(39) << left << c.endereco << "|\n";
    cout << "+------------------------------------------------+\n\n";
}

void listar_clientes(BaseClientes &base) {
    desenhar_banner("Clientes cadastrados");

    if (base.tamanho == 0) {
        cout << "Nenhum cliente cadastrado ainda.\n\n";
        return;
    }

    ordenar_por_id(base.dados, base.tamanho);
    const size_t por_pagina = 10;
    size_t indice = 0;

    while (indice < base.tamanho) {
        size_t ate = indice + por_pagina;
        if (ate > base.tamanho) {
            ate = base.tamanho;
        }

        cout << "Mostrando registros " << (indice + 1) << " a " << ate << " de " << base.tamanho << "\n\n";
        for (size_t i = indice; i < ate; ++i) {
            imprimir_cartao(base.dados[i]);
        }

        if (ate == base.tamanho) {
            return;
        }

        string opcao = ler_linha("Exibir próxima página? (S/N)");
        if (!opcao.empty() && (opcao[0] == 's' || opcao[0] == 'S')) {
            indice = ate;
        } else {
            return;
        }
    }
}

bool inserir_cliente(BaseClientes &base) {
    desenhar_banner("Novo cadastro de cliente");
    Cliente novo = ler_cliente();

    if (existe_id(base, novo.id)) {
        cout << "\nJá existe um cliente com o ID " << novo.id << ".\n\n";
        return false;
    }
    if (existe_documento(base, novo.documento)) {
        cout << "\nDocumento " << novo.documento << " já cadastrado.\n\n";
        return false;
    }

    if (!garantir_capacidade(base, base.tamanho + 1)) {
        return false;
    }

    base.dados[base.tamanho++] = novo;
    if (!salvar_clientes(base)) {
        return false;
    }

    cout << "\nCliente cadastrado com sucesso!\n\n";
    return true;
}

bool atualizar_cliente(BaseClientes &base) {
    desenhar_banner("Atualizar cliente");
    int id = ler_inteiro("Informe o ID para atualização");
    ordenar_por_id(base.dados, base.tamanho);
    int indice = busca_binaria_id(base.dados, base.tamanho, id);
    if (indice < 0) {
        cout << "\nCliente não encontrado.\n\n";
        return false;
    }

    cout << "\nAtualizando registro de " << base.dados[indice].nome_completo << " (ID " << id << ")\n";
    Cliente atualizado = ler_cliente();

    for (size_t i = 0; i < base.tamanho; ++i) {
        if (static_cast<int>(i) == indice) {
            continue;
        }
        if (base.dados[i].id == atualizado.id) {
            cout << "\nJá existe outro cliente com o ID " << atualizado.id << ".\n\n";
            return false;
        }
        if (strcmp(base.dados[i].documento, atualizado.documento) == 0) {
            cout << "\nDocumento " << atualizado.documento << " já cadastrado em outro cliente.\n\n";
            return false;
        }
    }

    base.dados[indice] = atualizado;
    if (!salvar_clientes(base)) {
        return false;
    }

    cout << "\nRegistro atualizado com sucesso!\n\n";
    return true;
}

bool remover_cliente(BaseClientes &base) {
    desenhar_banner("Remover cliente");
    int id = ler_inteiro("Informe o ID para exclusão");
    ordenar_por_id(base.dados, base.tamanho);
    int indice = busca_binaria_id(base.dados, base.tamanho, id);
    if (indice < 0) {
        cout << "\nCliente não encontrado.\n\n";
        return false;
    }

    for (size_t i = static_cast<size_t>(indice); i + 1 < base.tamanho; ++i) {
        base.dados[i] = base.dados[i + 1];
    }
    --base.tamanho;

    if (!salvar_clientes(base)) {
        return false;
    }

    cout << "\nRegistro removido.\n\n";
    return true;
}

void buscar_por_id(BaseClientes &base) {
    desenhar_banner("Busca por ID");
    int id = ler_inteiro("Informe o ID para busca");
    ordenar_por_id(base.dados, base.tamanho);
    int indice = busca_binaria_id(base.dados, base.tamanho, id);
    if (indice < 0) {
        cout << "\nNenhum cliente com ID " << id << " encontrado.\n\n";
        return;
    }

    const Cliente &c = base.dados[indice];
    imprimir_cartao(c);
}

void buscar_por_nome(BaseClientes &base) {
    desenhar_banner("Busca por nome");
    string termo = ler_linha("Digite o nome completo para busca exata");

    // cria uma cópia para ordenar por nome sem perder a ordem de gravação
    Cliente *copia = new (nothrow) Cliente[base.tamanho];
    if (!copia) {
        perror("Falha ao alocar memória");
        return;
    }

    for (size_t i = 0; i < base.tamanho; ++i) {
        copia[i] = base.dados[i];
    }

    int indice = busca_binaria_nome(copia, base.tamanho, termo);
    if (indice < 0) {
        cout << "\nNenhum cliente chamado '" << termo << "' encontrado.\n\n";
    } else {
        const Cliente &c = copia[indice];
        imprimir_cartao(c);
    }

    delete[] copia;
}

// --------------------------------------------------------------
// Interface
// --------------------------------------------------------------

void pausar() {
    cout << "Pressione ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void limpar_tela() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void desenhar_banner(const string &titulo) {
    limpar_tela();
    cout << "==================================================\n";
    cout << "  " << titulo << "\n";
    cout << "==================================================\n\n";
}

void exibir_menu() {
    desenhar_banner("Sistema de Gerenciamento de Clientes");
    cout << "1 - Listar clientes\n";
    cout << "2 - Inserir novo cliente\n";
    cout << "3 - Atualizar cliente\n";
    cout << "4 - Remover cliente\n";
    cout << "5 - Buscar por ID (binária)\n";
    cout << "6 - Buscar por nome (binária)\n";
    cout << "0 - Sair\n";
}

int main() {
    BaseClientes base;
    if (!carregar_clientes(base)) {
        return 1;
    }

    int opcao = -1;
    while (opcao != 0) {
        exibir_menu();
        opcao = ler_inteiro("Escolha uma opção");

        switch (opcao) {
            case 1:
                listar_clientes(base);
                pausar();
                break;
            case 2:
                inserir_cliente(base);
                pausar();
                break;
            case 3:
                atualizar_cliente(base);
                pausar();
                break;
            case 4:
                remover_cliente(base);
                pausar();
                break;
            case 5:
                buscar_por_id(base);
                pausar();
                break;
            case 6:
                buscar_por_nome(base);
                pausar();
                break;
            case 0:
                break;
            default:
                cout << "Opção inválida!\n\n";
                pausar();
                break;
        }
        cout << "\n";
    }

    cout << "Encerrando o sistema.\n";
    destruir_base(base);
    return 0;
}
