#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

// Estrutura para uma instrução do tipo R
// uint32_t: Isso define o tipo base da variável, ou seja, ela é armazenada em uma unidade de 32 bits.
typedef struct {
    uint32_t opcode : 6;  //Isso indica que dentro desses 32 bits, apenas 6 bits serão usados para o campo opcode, segue para o resto.
    uint32_t rs : 5; 
    uint32_t rt : 5; 
    uint32_t rd : 5; 
    uint32_t shamt : 5;  
    uint32_t funct : 6;  
} InstrucaoR;

// Estrutura para uma instrução do tipo I
typedef struct {
    uint32_t opcode : 6; 
    uint32_t rs : 5; 
    uint32_t rt : 5; 
    uint32_t immediate : 16; 
} InstrucaoI;

// Estrutura para uma instrução do tipo J
typedef struct {
    uint32_t opcode : 6;   
    uint32_t address : 26; 
} InstrucaoJ;

/*typedef enum {
    FUNC_ADD = 32,
    FUNC_SUB = 34,
    FUNC_SLL = 0,
} FuncaoInstrucao;
*/

// Função para inicializar os registradores
void inicializar_registradores(Registrador registradores[]) {
    registradores[0] = (Registrador){"$zero", 0, "Sempre contem o valor zero"};
    registradores[1] = (Registrador){"$at", 1, "Usado pelo montador para auxiliar nas expansoes de pseudo-instrucoes."};
    registradores[2] = (Registrador){"$v0", 2, "Registrador de valor de retorno de funcoes."};
    registradores[3] = (Registrador){"$v1", 3, "Registrador de valor de retorno de funcoes (quando ultrapassa 32 bits)."};
    registradores[4] = (Registrador){"$a0", 4, "Registrador de argumento."};
    registradores[5] = (Registrador){"$a1", 5, "Registrador de argumento."};
    registradores[6] = (Registrador){"$a2", 6, "Registrador de argumento."};
    registradores[7] = (Registrador){"$a3", 7, "Registrador de argumento."};
    registradores[8] = (Registrador){"$t0", 8, "Registrador temporario."};
    registradores[9] = (Registrador){"$t1", 9, "Registrador temporario."};
    registradores[10] = (Registrador){"$t2", 10, "Registrador temporario."};
    registradores[11] = (Registrador){"$t3", 11, "Registrador temporario."};
    registradores[12] = (Registrador){"$t4", 12, "Registrador temporario."};
    registradores[13] = (Registrador){"$t5", 13, "Registrador temporario."};
    registradores[14] = (Registrador){"$t6", 14, "Registrador temporario."};
    registradores[15] = (Registrador){"$t7", 15, "Registrador temporario."};
    registradores[16] = (Registrador){"$t8", 16, "Registrador temporario."};
    registradores[17] = (Registrador){"$t9", 17, "Registrador temporario."};
    registradores[18] = (Registrador){"$s0", 18, "Registrador salvo."};
    registradores[19] = (Registrador){"$s1", 19, "Registrador salvo."};
    registradores[20] = (Registrador){"$s2", 20, "Registrador salvo."};
    registradores[21] = (Registrador){"$s3", 21, "Registrador salvo."};
    registradores[22] = (Registrador){"$s4", 22, "Registrador salvo."};
    registradores[23] = (Registrador){"$s5", 23, "Registrador salvo."};
    registradores[24] = (Registrador){"$s6", 24, "Registrador salvo."};
    registradores[25] = (Registrador){"$s7", 25, "Registrador salvo."};
    registradores[26] = (Registrador){"$k0", 26, "Registrador reservado para o kernel."};
    registradores[27] = (Registrador){"$k1", 27, "Registrador reservado para o kernel."};
    registradores[28] = (Registrador){"$gp", 28, "Ponto de base global para dados estaticos."};
    registradores[29] = (Registrador){"$sp", 29, "Ponteiro de pilha."};
    registradores[30] = (Registrador){"$fp", 30, "Ponteiro de quadro (frame) usado para referencia a variaveis locais."};
    registradores[31] = (Registrador){"$ra", 31, "Registrador de retorno de endereco apos chamada de funcao."};
}

// Função para definir o valor de um registrador usando o nome
// Para cada registrador, ela compara o nome fornecido (nome) com o nome do registrador no array.
void definir_registrador(Registradores *regs, Registrador registradores[], const char *nome, int32_t valor) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(nome, registradores[i].nome) == 0) {
            regs->reg[i] = valor;
            printf("Definindo %s = %d\n", nome, valor);
            return;
        }
    }
    printf("Registrador %s não encontrado.\n", nome);
}

// Função para obter o nome do registrador pelo índice
const char* obter_nome_registrador(Registrador registradores[], int num) {
    if (num >= 0 && num < 32) {
        return registradores[num].nome;
    }
    return "Registrador Inexistente";
}

// Função para executar uma instrução do tipo R
void executar_instrucaoR(InstrucaoR instrucao, Registradores *regs) {

    // Verifica se o opcode é diferente de 0
    if (instrucao.opcode != 0) {
        printf("Erro: Opcode invalido para instrucao do tipo R: %d\n", instrucao.opcode);
        return;
    }
    
    int32_t resultado;
    
    switch(instrucao.funct) {
        case 32: // ADD
            resultado = regs->reg[instrucao.rs] + regs->reg[instrucao.rt];
            regs->reg[instrucao.rd] = resultado;
            printf("ADD: %d + %d = %d (salvo em $%d)\n", 
                   regs->reg[instrucao.rs], 
                   regs->reg[instrucao.rt], 
                   resultado, 
                   instrucao.rd);
            break;
        case 34:  // SUB
            resultado = regs->reg[instrucao.rs] - regs->reg[instrucao.rt];
            regs->reg[instrucao.rd] = resultado;
            printf("SUB: %d - %d = %d (salvo em $%d)\n", 
                   regs->reg[instrucao.rs], 
                   regs->reg[instrucao.rt], 
                   resultado, 
                   instrucao.rd);
            break;
        case 0: // SLL
            resultado = regs->reg[instrucao.rt] << instrucao.shamt; // Desloca à esquerda
            regs->reg[instrucao.rd] = resultado;
            printf("SLL: %d << %d = %d (salvo em $%d)\n", 
                   regs->reg[instrucao.rt], 
                   instrucao.shamt, 
                   resultado, 
                   instrucao.rd);
            break;
        default:
            printf("Instrucao do tipo R nao suportada: %d\n", instrucao.funct);
            break;
    }
}

void ler_instrucaoR(char *input, InstrucaoR *instrucao) {
    // Remove o comentário da entrada
    char *comentario = strchr(input, ';');
    if (comentario) {
        *comentario = '\0'; // Trunca a string na posição do ';'
    }

    // Variáveis para armazenar os valores temporários
    int values[6];
    int count = 0;

    // Tokeniza a string de entrada
    char *token = strtok(input, ", ");
    while (token != NULL && count < 6) {
        values[count++] = atoi(token); // Converte para inteiro e armazena
        token = strtok(NULL, ", ");
    }

    // Atribui os valores à estrutura
    if (count == 6) {
        instrucao->opcode = values[0];
        instrucao->rs = values[1];
        instrucao->rt = values[2];
        instrucao->rd = values[3];
        instrucao->shamt = values[4];
        instrucao->funct = values[5];
    } else {
        printf("Erro: Numero de valores invalido.\n");
    }
}

void executar_instrucaoI(InstrucaoI instrucao, Registradores *regs) {

    int32_t resultado;

     switch(instrucao.opcode) {
        case 8: // ADDI
            resultado = regs->reg[instrucao.rt] = regs->reg[instrucao.rs] + instrucao.immediate;
            printf("ADDI: %d + %d = %d (salvo em $%d)\n", 
                regs->reg[instrucao.rs], 
                instrucao.immediate, 
                regs->reg[instrucao.rt], 
                instrucao.rt);
            break;
            // Implementação das outras instruções I...
        default:
            printf("Instrucao do tipo I nao suportada: %d\n", instrucao.opcode);
            break;
    }
}

void ler_instrucaoI(char *input, InstrucaoI *instrucao) {
    // Remove o comentário da entrada
    char *comentario = strchr(input, ';');
    if (comentario) {
        *comentario = '\0'; // Trunca a string na posição do ';'
    }

    // Variáveis para armazenar os valores temporários
    int values[4];
    int count = 0;

    // Tokeniza a string de entrada
    char *token = strtok(input, ", ");
    while (token != NULL && count < 4) {
        values[count++] = atoi(token); // Converte para inteiro e armazena
        token = strtok(NULL, ", ");
    }

    // Atribui os valores à estrutura
    if (count == 4) {
        instrucao->opcode = values[0];  // 8 para ADDI
        instrucao->rs = values[1];       // Registrador fonte
        instrucao->rt = values[2];       // Registrador destino
        instrucao->immediate = values[3]; // Valor imediato
    } else {
        printf("Erro: Numero de valores invalido.\n");
    }
}

int verificar_tipo_instrucao(char *input) {
    // Analisa o primeiro valor para determinar se é do tipo R ou I
    int opcode = atoi(input); // Supõe que o opcode está no início do input
    return opcode == 8 ? 1 : 0; // 1 para instrução I (ADDI), 0 para R
}

// Função para mostrar informações dos registradores
void mostrar_informacoes_registradores(Registrador registradores[]) {
    printf("Nome do Registrador\tNumeracao do Registrador\tFuncionalidade\n");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < 32; i++) {
        printf("%s\t\t\t%d\t\t\t%s\n", registradores[i].nome, registradores[i].num, registradores[i].funcionalidade);
    }
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
}

// Função para mostrar os registradores e suas funcionalidades
void mostrar_registradores(Registradores *regs, Registrador registradores[]) {
    printf("\nEstado atual dos Registradores:\n");
    for (int i = 0; i < 32; i++) {
        printf("%s = %d\n", obter_nome_registrador(registradores, i), regs->reg[i]);
    }
}


int main() {
    // Inicializando os registradores com 0
    Registradores regs = {{0}};
    Registrador registradores[32];

    inicializar_registradores(registradores);
    mostrar_informacoes_registradores(registradores);

    // Definindo alguns valores nos registradores
    definir_registrador(&regs, registradores, "$t0", 27);  //REGISTRADOR 8
    definir_registrador(&regs, registradores, "$t1", 10); //REGISTRADOR 9
    definir_registrador(&regs, registradores, "$s0", 20); //REGISTRADOR 18


    char instrucao_input[100];
    printf("Insira a instrucao MIPS: ");
    fgets(instrucao_input, sizeof(instrucao_input), stdin);

    // Verifica o tipo da instrução usando a nova função
    if (verificar_tipo_instrucao(instrucao_input)) {
        InstrucaoI instrI;
        ler_instrucaoI(instrucao_input, &instrI); // Lê a instrução I
        executar_instrucaoI(instrI, &regs); // Executa a instrução I
        // EXEMPLO ADDI -> 8, 18, 19, 5
    } else {
        InstrucaoR instrR1;
        // EXEMPLO ADD -> 0, 8, 9, 10, 0, 32
        // EXEMPLO SUB -> 0, 8, 9, 11, 0, 34
        // EXEMPLO SLL -> 0, 0, 8, 12, 2, 0
        ler_instrucaoR(instrucao_input, &instrR1); // Lê a instrução R
        executar_instrucaoR(instrR1, &regs); // Executa a instrução R
    }

    // Mostrando os registradores e suas informações
    mostrar_registradores(&regs, registradores);

    //TODO
    //ARRUMAR FORMATAÇÃO COLOCANDO REGISTRADORES = (ATUAL)SLL: 27 << 2 = 108 (salvo em $12) -> sll $t0 => 27 << 2 = 108
    //ARRUMAR EXCEÇÃO DE OPCODE NA INSTRUÇÃO I, QUANDO É 8 OKAY MAS QUANDO NÃO É ESTÁ RETORNANDO O ERRO DA INSTRUÇÃO R DE OPCODE

    return 0;
}