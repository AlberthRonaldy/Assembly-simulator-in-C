#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
// Estrutura de registradores (32 registradores de 32 bits)
typedef struct {
    int32_t reg[32]; 
} Registradores;

// Estrutura para um registrador
typedef struct {
    const char *nome;               // Nome do registrador
    uint32_t num;            // Número do registrador
    const char *funcionalidade;    // Funcionalidade do registrador
} Registrador;

typedef enum { TIPO_R, TIPO_I, TIPO_J } TipoInstrucao;

typedef struct {
    TipoInstrucao tipo;
    int opcode;
    int rs;
    int rt;
    int rd;
    int shamt;
    int funct;
    int address;     // Para instruções do tipo J
    int immediate;   // Para instruções do tipo I
} Instrucao;

const char* nomes_registradores[32] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",  // 0-7
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",  // 8-15
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",  // 16-23
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"   // 24-31
};


void listar_registradores(Registradores *registradores, const Registrador *tabela, int tamanho_tabela) {
    printf("Registradores:\n");
    
    for (int i = 0; i < tamanho_tabela; i++) {
        printf("%s: %d\n", tabela[i].nome, registradores->reg[tabela[i].num]);
    }
}
// Exibe as instruções disponíveis
void exibir_instrucoes_disponiveis() {
    printf("Instrucoes disponiveis:\n");
    printf(" - ADD\n");
    printf(" - SUB\n");
    printf(" - SLL\n");
    printf(" - ADDI\n");
    printf(" - BEQ\n");
    printf(" - BNE\n");
    printf(" - J\n");
    printf(" - JAL\n");
    printf(" - JR\n");
    // Adicione outras instruções conforme necessário
    printf("Digite uma instrucao:\n");
}
// Mapeia nomes para índices dos registradores
int buscar_numero_registrador(const char *nome, const Registrador *tabela, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(tabela[i].nome, nome) == 0) {
            return tabela[i].num;
        }
    }
    return -1;
}

// Função para remover as vírgulas da entrada
void remover_virgulas(char *input) {
    char *src = input, *dst = input;
    while (*src) {
        if (*src != ',') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0'; // Termina a string
}


void executar_instrucaoR(Instrucao *instrucao, Registradores *registradores, int *pc) {
    if (instrucao->funct == 32) { // ADD
        registradores->reg[instrucao->rd] = registradores->reg[instrucao->rs] + registradores->reg[instrucao->rt];
        *pc += 4;
    } else if (instrucao->funct == 34) { // SUB
        registradores->reg[instrucao->rd] = registradores->reg[instrucao->rs] - registradores->reg[instrucao->rt];
        *pc += 4;
    } else if (instrucao->funct == 0) { // SLL
        registradores->reg[instrucao->rd] = registradores->reg[instrucao->rt] << instrucao->shamt;
        *pc += 4;
    }
}

void executar_instrucaoI(Instrucao *instrucao, Registradores *registradores, int *pc) {
    if (instrucao->opcode == 4) { // BEQ
        if (registradores->reg[instrucao->rs] == registradores->reg[instrucao->rt]) {
            *pc += instrucao->immediate; // Salta para o endereço de destino
        }
    } else if (instrucao->opcode == 5) { // BNE
        if (registradores->reg[instrucao->rs] != registradores->reg[instrucao->rt]) {
            *pc += instrucao->immediate; // Salta para o endereço de destino
        }
     } else if (instrucao->opcode == 8) { // ADDI
        registradores->reg[instrucao->rs] = registradores->reg[instrucao->rt] + instrucao->immediate;
    }
}

void executar_instrucaoJ(Instrucao *instrucao, Registradores *registradores, int *pc) {
    if (instrucao->opcode == 2) { // J
        *pc = instrucao->address;
    } else if (instrucao->opcode == 3) { // JAL
        registradores->reg[31] = *pc + 1; // Guarda o endereço de retorno no registrador $ra
        *pc = instrucao->address;
    } else if (instrucao->opcode == 4) { // JR (Jump Register)
        *pc = registradores->reg[instrucao->rs]; // Ajusta o PC para o endereço armazenado em $rs
    }
}

// Função principal para executar instruções
void executar_instrucao(Instrucao *instrucao, Registradores *registradores, int *pc) {
    if (instrucao->tipo == TIPO_R) {
        executar_instrucaoR(instrucao, registradores, pc);
    } else if (instrucao->tipo == TIPO_I) {
        executar_instrucaoI(instrucao, registradores, pc);
    } else if (instrucao->tipo == TIPO_J) {
        executar_instrucaoJ(instrucao, registradores, pc);
    }
}

void ler_e_executar(char *input, Instrucao *instrucao, Registradores *registradores, const Registrador *tabela, int tamanho_tabela, int *pc) {

    // Remover as vírgulas da entrada
    remover_virgulas(input);
    
    char *token = strtok(input, " ");
    if (!token) return;

    // Identifica a instrução e configura os campos apropriados
    if (strcmp(token, "ADD") == 0) {
        instrucao->tipo = TIPO_R;
        instrucao->opcode = 0;
        instrucao->funct = 32;
    } else if (strcmp(token, "SUB") == 0) {
        instrucao->tipo = TIPO_R;
        instrucao->opcode = 0;
        instrucao->funct = 34;
    } else if (strcmp(token, "SLL") == 0) {
        instrucao->tipo = TIPO_R;
        instrucao->opcode = 0;
        instrucao->funct = 0;
    } else if (strcmp(token, "ADDI") == 0) { 
        instrucao->tipo = TIPO_I;
        instrucao->opcode = 8;
    } else if (strcmp(token, "BEQ") == 0) {
        instrucao->tipo = TIPO_I;
        instrucao->opcode = 4;
    } else if (strcmp(token, "BNE") == 0) {
        instrucao->tipo = TIPO_I;
        instrucao->opcode = 5;
    } else if (strcmp(token, "J") == 0) {
        instrucao->tipo = TIPO_J;
        instrucao->opcode = 2;
    } else if (strcmp(token, "JAL") == 0) {
        instrucao->tipo = TIPO_J;
        instrucao->opcode = 3;
    } else if (strcmp(token, "JR") == 0) {
        instrucao->tipo = TIPO_J;
        instrucao->opcode = 4;
    }

    // Lê os registradores ou valores específicos da instrução
    if (instrucao->tipo == TIPO_R) {
        if (instrucao->funct == 0) { // SLL (formato: SLL RD, RT, SHAMT)
            token = strtok(NULL, " ");
            instrucao->rd = buscar_numero_registrador(token, tabela, tamanho_tabela);

            token = strtok(NULL, " ");
            instrucao->rt = buscar_numero_registrador(token, tabela, tamanho_tabela);

            token = strtok(NULL, " ");
            instrucao->shamt = atoi(token); // Converte o valor de shamt para inteiro
        } else { // ADD, SUB (formato: OP RD, RS, RT)
            token = strtok(NULL, " ");
            instrucao->rd = buscar_numero_registrador(token, tabela, tamanho_tabela);

            token = strtok(NULL, " ");
            instrucao->rs = buscar_numero_registrador(token, tabela, tamanho_tabela);

            token = strtok(NULL, " ");
            instrucao->rt = buscar_numero_registrador(token, tabela, tamanho_tabela);
        }
    } else if (instrucao->tipo == TIPO_I) {
        // Instruções tipo I: ADDI, BEQ, BNE (formato: OP RS, RT, IMMEDIATE)
        token = strtok(NULL, " ");
        instrucao->rs = buscar_numero_registrador(token, tabela, tamanho_tabela);

        token = strtok(NULL, " ");
        instrucao->rt = buscar_numero_registrador(token, tabela, tamanho_tabela);

        token = strtok(NULL, " ");
        instrucao->immediate = atoi(token); // Converte o imediato para inteiro
    } else if (instrucao->tipo == TIPO_J) {
        // Instruções tipo J: J, JAL, JR (formato: OP ADDRESS)
        token = strtok(NULL, " ");
        instrucao->address = atoi(token); // Converte o endereço para inteiro
    }

    // Executa a instrução com o contador de programa (PC)
    executar_instrucao(instrucao, registradores, pc);
}


void inicializar_registradores_aleatoriamente(Registradores *registradores) {
    srand(time(NULL));
    for (int i = 0; i < 32; i++) {
        registradores->reg[i] = rand() % 100;  // Inicializa com valores aleatórios de 0 a 99
    }
}

void imprimir_registradores_iniciais(const Registradores *registradores, const Registrador *tabela, int tamanho_tabela) {
    printf("Registradores iniciais:\n");
    for (int i = 0; i < tamanho_tabela; i++) {
        printf("%s (num: %u) = %d\n", tabela[i].nome, tabela[i].num, registradores->reg[tabela[i].num]);
    }
}

int main() {
    Registradores registradores = {0};
    Instrucao instrucao;
    Registrador tabela_registradores[] = {
        {"$t0", 8, "Temporário"},
        {"$t1", 9, "Temporário"},
        {"$t2", 10, "Temporário"},
        // Adicione outros registradores conforme necessário
    };
    int pc = 0;  // Contador de programa inicializado em 0

    // Inicializa os registradores com valores aleatórios e os imprime
    inicializar_registradores_aleatoriamente(&registradores);
    imprimir_registradores_iniciais(&registradores, tabela_registradores, sizeof(tabela_registradores) / sizeof(tabela_registradores[0]));

    while (1) {
        listar_registradores(&registradores, tabela_registradores, sizeof(tabela_registradores) / sizeof(tabela_registradores[0]));
        exibir_instrucoes_disponiveis();  // Exibe as instruções disponíveis ao usuário

        char input[100];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Erro ao ler a entrada.\n");
            break;
        }

        // Remove a nova linha da entrada, se houver
        input[strcspn(input, "\n")] = 0;

        // Lê e executa a instrução, passando `pc` como referência
        ler_e_executar(input, &instrucao, &registradores, tabela_registradores, sizeof(tabela_registradores) / sizeof(tabela_registradores[0]), &pc);

        // Exibe o valor de alguns registradores como feedback
        printf("Valor do registrador $t0: %d\n", registradores.reg[8]);
        printf("Valor do registrador $t1: %d\n", registradores.reg[9]);
        printf("Valor do registrador $t2: %d\n", registradores.reg[10]);
        printf("Contador de Programa (PC): %d\n\n", pc);  // Mostra o valor atualizado do PC
    }

    return 0;
}