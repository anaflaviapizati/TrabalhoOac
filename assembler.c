#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAMANHO_LINHA 256
#define MAX_ROTULOS 100
#define MAX_INSTRUCOES 1000

#define OPCODE_LW  0x03
#define OPCODE_SW  0x23
#define OPCODE_SUB 0x33
#define OPCODE_XOR 0x33
#define OPCODE_ADDI 0x13
#define OPCODE_SRL 0x33
#define OPCODE_BEQ 0x63
#define OPCODE_ADD 0x33
#define OPCODE_AND 0x33
#define OPCODE_ORI 0x13

#define FUNCT3_LW  0x2
#define FUNCT3_SW  0x2
#define FUNCT3_SUB 0x0
#define FUNCT3_XOR 0x4
#define FUNCT3_ADDI 0x0
#define FUNCT3_SRL 0x5
#define FUNCT3_BEQ 0x0
#define FUNCT3_ADD 0x0
#define FUNCT3_AND 0x7
#define FUNCT3_ORI 0x6

#define FUNCT7_SUB 0x20
#define FUNCT7_XOR 0x00
#define FUNCT7_SRL 0x00
#define FUNCT7_ADD 0x00
#define FUNCT7_AND 0x00

typedef struct {
    char nome[50];
    int endereco;
} Rotulo;

typedef struct {
    Rotulo rotulos[MAX_ROTULOS];
    int total_rotulos;
    int endereco_atual;
    int instrucoes[MAX_INSTRUCOES];
    int total_instrucoes;
} Montador;

void iniciar_montador(Montador *m) {
    m->total_rotulos = 0;
    m->endereco_atual = 0;
    m->total_instrucoes = 0;
}

int extrair_registrador(const char *reg) {
    if (reg[0] != 'x') return -1;
    int numero = atoi(reg + 1);
    if (numero < 0 || numero > 31) return -1;
    return numero;
}

int extrair_imediato(const char *valor) {
    while (isspace(*valor)) valor++;
    
    if (strncmp(valor, "0b", 2) == 0) {
        return (int)strtol(valor + 2, NULL, 2);
    } else if (strncmp(valor, "0x", 2) == 0) {
        return (int)strtol(valor + 2, NULL, 16);
    } else if (strncmp(valor, "0", 1) == 0 && strlen(valor) > 1) {
        return (int)strtol(valor + 1, NULL, 8);
    } else {
        return atoi(valor);
    }
}

unsigned int codificar_tipo_r(int opcode, int rd, int rs1, int rs2, int funct3, int funct7) {
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int codificar_tipo_i(int opcode, int rd, int rs1, int imed, int funct3) {
    return (imed << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int codificar_tipo_s(int opcode, int rs1, int rs2, int imed, int funct3) {
    int imed_11_5 = (imed >> 5) & 0x7f;
    int imed_4_0 = imed & 0x1f;
    return (imed_11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imed_4_0 << 7) | opcode;
}

unsigned int codificar_tipo_b(int opcode, int rs1, int rs2, int imed, int funct3) {
    int imed_12 = (imed >> 12) & 0x1;
    int imed_11 = (imed >> 11) & 0x1;
    int imed_10_5 = (imed >> 5) & 0x3f;
    int imed_4_1 = (imed >> 1) & 0xf;
    return (imed_12 << 31) | (imed_10_5 << 25) | (rs2 << 20) | (rs1 << 15) |
           (funct3 << 12) | (imed_4_1 << 8) | (imed_11 << 7) | opcode;
}

int buscar_endereco_rotulo(Montador *m, const char *rotulo) {
    for (int i = 0; i < m->total_rotulos; i++) {
        if (strcmp(m->rotulos[i].nome, rotulo) == 0) {
            return m->rotulos[i].endereco;
        }
    }
    return -1;
}

void primeira_passagem(Montador *m, FILE *arquivo) {
    char linha[TAMANHO_LINHA];
    m->endereco_atual = 0;

    while (fgets(linha, TAMANHO_LINHA, arquivo)) {
        linha[strcspn(linha, "\n")] = 0;
        if (linha[0] == '\0' || linha[0] == '#') continue;

        char *dois_pontos = strchr(linha, ':');
        if (dois_pontos) {
            *dois_pontos = '\0';
            strcpy(m->rotulos[m->total_rotulos].nome, linha);
            m->rotulos[m->total_rotulos].endereco = m->endereco_atual;
            m->total_rotulos++;
        } else {
            m->endereco_atual += 4;
        }
    }
}

void segunda_passagem(Montador *m, FILE *arquivo) {
    char linha[TAMANHO_LINHA];
    m->endereco_atual = 0;
    rewind(arquivo);

    while (fgets(linha, TAMANHO_LINHA, arquivo)) {
        linha[strcspn(linha, "\n")] = 0;
        if (linha[0] == '\0' || linha[0] == '#') continue;
        if (strchr(linha, ':')) continue;

        char instrucao[50], rd[10], rs1[10], rs2[10], imed[20];
        int rd_num, rs1_num, rs2_num, imed_num;

        if (sscanf(linha, "%s", instrucao) == 1) {
            if (strcmp(instrucao, "lw") == 0) {
                sscanf(linha, "lw %[^,], %[^(](%[^)])", rd, imed, rs1);
                rd_num = extrair_registrador(rd);
                rs1_num = extrair_registrador(rs1);
                imed_num = extrair_imediato(imed);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_i(OPCODE_LW, rd_num, rs1_num, imed_num, FUNCT3_LW);
            } else if (strcmp(instrucao, "sw") == 0) {
                sscanf(linha, "sw %[^,], %[^(](%[^)])", rs2, imed, rs1);
                rs2_num = extrair_registrador(rs2);
                rs1_num = extrair_registrador(rs1);
                imed_num = extrair_imediato(imed);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_s(OPCODE_SW, rs1_num, rs2_num, imed_num, FUNCT3_SW);
            } else if (strcmp(instrucao, "sub") == 0) {
                sscanf(linha, "sub %[^,], %[^,], %s", rd, rs1, rs2);
                rd_num = extrair_registrador(rd);
                rs1_num = extrair_registrador(rs1);
                rs2_num = extrair_registrador(rs2);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_r(OPCODE_SUB, rd_num, rs1_num, rs2_num, FUNCT3_SUB, FUNCT7_SUB);
            } else if (strcmp(instrucao, "xor") == 0) {
                sscanf(linha, "xor %[^,], %[^,], %s", rd, rs1, rs2);
                rd_num = extrair_registrador(rd);
                rs1_num = extrair_registrador(rs1);
                rs2_num = extrair_registrador(rs2);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_r(OPCODE_XOR, rd_num, rs1_num, rs2_num, FUNCT3_XOR, FUNCT7_XOR);
            } else if (strcmp(instrucao, "addi") == 0) {
                sscanf(linha, "addi %[^,], %[^,], %s", rd, rs1, imed);
                rd_num = extrair_registrador(rd);
                rs1_num = extrair_registrador(rs1);
                imed_num = extrair_imediato(imed);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_i(OPCODE_ADDI, rd_num, rs1_num, imed_num, FUNCT3_ADDI);
            } else if (strcmp(instrucao, "srl") == 0) {
                sscanf(linha, "srl %[^,], %[^,], %s", rd, rs1, rs2);
                rd_num = extrair_registrador(rd);
                rs1_num = extrair_registrador(rs1);
                rs2_num = extrair_registrador(rs2);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_r(OPCODE_SRL, rd_num, rs1_num, rs2_num, FUNCT3_SRL, FUNCT7_SRL);
            } else if (strcmp(instrucao, "beq") == 0) {
                sscanf(linha, "beq %[^,], %[^,], %s", rs1, rs2, imed);
                rs1_num = extrair_registrador(rs1);
                rs2_num = extrair_registrador(rs2);
                int endereco_rotulo = buscar_endereco_rotulo(m, imed);
                if (endereco_rotulo == -1) {
                    fprintf(stderr, "Erro: Rótulo indefinido %s\n", imed);
                    exit(1);
                }
                imed_num = endereco_rotulo - m->endereco_atual;
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_b(OPCODE_BEQ, rs1_num, rs2_num, imed_num, FUNCT3_BEQ);
            } else if (strcmp(instrucao, "add") == 0) {
                sscanf(linha, "add %[^,], %[^,], %s", rd, rs1, rs2);
                rd_num = extrair_registrador(rd);
                rs1_num = extrair_registrador(rs1);
                rs2_num = extrair_registrador(rs2);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_r(OPCODE_ADD, rd_num, rs1_num, rs2_num, FUNCT3_ADD, FUNCT7_ADD);
            } else if (strcmp(instrucao, "and") == 0) {
                sscanf(linha, "and %[^,], %[^,], %s", rd, rs1, rs2);
                rd_num = extrair_registrador(rd);
                rs1_num = extrair_registrador(rs1);
                rs2_num = extrair_registrador(rs2);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_r(OPCODE_AND, rd_num, rs1_num, rs2_num, FUNCT3_AND, FUNCT7_AND);
            } else if (strcmp(instrucao, "ori") == 0) {
                sscanf(linha, "ori %[^,], %[^,], %s", rd, rs1, imed);
                rd_num = extrair_registrador(rd);
                rs1_num = extrair_registrador(rs1);
                imed_num = extrair_imediato(imed);
                m->instrucoes[m->total_instrucoes++] = codificar_tipo_i(OPCODE_ORI, rd_num, rs1_num, imed_num, FUNCT3_ORI);
            }
        }
        m->endereco_atual += 4;
    }
}

void escrever_saida(Montador *m, const char *arquivo_saida) {
    FILE *saida = fopen(arquivo_saida, "w");
    if (!saida) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo de saída %s\n", arquivo_saida);
        exit(1);
    }

    printf("\nInstruções em binário:\n");
    for (int i = 0; i < m->total_instrucoes; i++) {
        unsigned int instrucao = m->instrucoes[i];
        for (int bit = 31; bit >= 0; bit--) {
            int bit_value = (instrucao >> bit) & 1;
            fprintf(saida, "%d", bit_value);
            printf("%d", bit_value);
        }
        fprintf(saida, "\n");
        printf("\n");
    }
    fclose(saida);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s entrada.asm saida.hex\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    if (!entrada) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    Montador montador;
    iniciar_montador(&montador);

    primeira_passagem(&montador, entrada);
    segunda_passagem(&montador, entrada);
    escrever_saida(&montador, argv[2]);

    fclose(entrada);
    printf("Montagem concluída com sucesso. Saída salva em %s\n", argv[2]);
    return 0;
}
