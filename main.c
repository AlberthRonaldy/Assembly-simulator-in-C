#include <stdio.h>
#include <stdint.h>
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
    uint32_t opcode : 6; //Isso indica que dentro desses 32 bits, apenas 6 bits serão usados para o campo opcode, segue para o resto.
    uint32_t rs : 5; 
    uint32_t rt : 5; 
    uint32_t rd : 5; 
    uint32_t shamt : 5;  
    uint32_t funct : 6;  
} InstrucaoR;

// Estrutura para uma instrução do tipo I
typedef struct {
    uint32_t opcode : 6;
    uint32_t rt : 5;  
    uint32_t rs : 5;  
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

// Prototipagem das funções
void inicializar_registradores(Registrador registradores[]);
void definir_registrador(Registradores *regs, Registrador registradores[], const char *nome, int32_t valor);
const char* obter_nome_registrador(Registrador registradores[], int num);
void executar_instrucaoR(InstrucaoR instrucao, Registradores *regs, Registrador registradores[]);
void executar_instrucaoI(InstrucaoI instrucao, Registradores *regs, Registrador registradores[]);
void mostrar_informacoes_registradores(Registrador registradores[]);
void mostrar_registradores(Registradores *regs, Registrador registradores[]);

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
void executar_instrucaoR(InstrucaoR instrucao, Registradores *regs, Registrador registradores[]) {
    int32_t resultado;
    const char *operacao;
    const char *operador;

    switch(instrucao.funct) {
        case 32: // ADD
            resultado = regs->reg[instrucao.rs] + regs->reg[instrucao.rt];
            operacao = "add";
            operador = "+";
            break;
        case 34:  //SUB
            resultado = regs->reg[instrucao.rs] - regs->reg[instrucao.rt];
            operacao = "sub";
            operador = "-";
            break;
        case 0:   // SLL
            resultado = regs->reg[instrucao.rt] << instrucao.shamt; // Deslocamento à esquerda
            operacao = "sll";
            operador = "<<";
            break;
        default:
            printf("Instrucao do tipo R nao suportada: %d\n", instrucao.funct);
            return;
    }
    // Atribui o resultado ao registrador de destino e imprime o resultado
    regs->reg[instrucao.rd] = resultado;
    // Se for sll não vai usar 3 registradores, somente 2
    if (instrucao.funct == 0) {
        printf("%s %s => %d %s %d = %d\n", 
            operacao, 
            obter_nome_registrador(registradores, instrucao.rt), 
            regs->reg[instrucao.rt], 
            operador, 
            instrucao.shamt, 
            resultado);
    } else {
        printf("%s %s, %s, %s => %d %s %d = %d\n", 
            operacao, 
            obter_nome_registrador(registradores, instrucao.rd), 
            obter_nome_registrador(registradores, instrucao.rs), 
            obter_nome_registrador(registradores, instrucao.rt), 
            regs->reg[instrucao.rs], 
            operador, 
            regs->reg[instrucao.rt], 
            resultado);
    }
}

// Função para executar uma instrução do tipo I
// Instruções do tipo I usam o opcode como ref
void executar_instrucaoI(InstrucaoI instrucao, Registradores *regs, Registrador registradores[]) {
    int32_t resultado;
    const char *operacao;
    const char *operador;

    switch(instrucao.opcode) {
        case 8: // ADDI (Add Immediate)
            resultado = regs->reg[instrucao.rs] + instrucao.immediate;
            operacao = "addi";
            operador = "+";
            break;
        // Continuação das outras 2 intruções do tipo I
        default:
            printf("Instrucao do tipo I nao suportada: %d\n", instrucao.opcode);
            return;
    }
    regs->reg[instrucao.rt] = resultado;
    printf("%s %s, %s, %d => %d %s %d = %d\n", 
        operacao, 
        obter_nome_registrador(registradores, instrucao.rt), 
        obter_nome_registrador(registradores, instrucao.rs), 
        instrucao.immediate, 
        regs->reg[instrucao.rs], 
        operador, 
        instrucao.immediate, 
        resultado);
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

    // Inicializando os registradores
    inicializar_registradores(registradores);
    mostrar_informacoes_registradores(registradores);

    // Definindo os valores dos registradores
    definir_registrador(&regs, registradores, "$t0", 27);  // 8 (correspondente a $t0) = 5
    definir_registrador(&regs, registradores, "$t1", 10); // 9 (correspondente a $t1) = 10
    definir_registrador(&regs, registradores, "$s0", 20); // 18 (corresponde a $s0) = 20


    // Criação das instruções do tipo R
    InstrucaoR instrR1 = {0, 8, 9, 10, 0, 32}; // ADD $t2 = $t0 + $t1
    InstrucaoR instrR2 = {0, 8, 9, 11, 0, 34}; // SUB $t3 = $t0 - $t1
    InstrucaoR instrR3 = {0, 0, 8, 12, 2, 0}; // SLL $t2 = $t0 << 2 (deslocar $t0 2 posições à esquerda)
    //deslocar um número duas posições à esquerda é equivalente a multiplicá-lo por 2² (ou seja, multiplicá-lo por 4)

    // Criação das instruções do tipo R
    InstrucaoI instrI1 = {8, 19, 18, 5}; // ADDI $s1, $s0, 5 (s1 = 00 + 5) - 8 = código de ref do ADDI.


    // Executando as instruções do tipo R
    executar_instrucaoR(instrR1, &regs, registradores);
    executar_instrucaoR(instrR2, &regs, registradores);
    executar_instrucaoR(instrR3, &regs, registradores);

    // Executando as instruções do tipo I
    executar_instrucaoI(instrI1, &regs, registradores);


    // Mostrando os registradores e suas informações
    mostrar_registradores(&regs, registradores);

    return 0;
}
