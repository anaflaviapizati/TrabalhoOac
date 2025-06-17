# Montador RISC-V Simplificado

Este projeto implementa um **montador (assembler) básico da arquitetura RISC-V**, responsável por converter instruções em linguagem de montagem para código de máquina (binário de 32 bits). Ele foi desenvolvido com fins **didáticos**, sendo parte de um trabalho de Organização e Arquitetura de Computadores (OAC).

---

## 📄 Documentação

Link do PDF da documentação desenvolvido no LaTeX.
https://drive.google.com/file/d/1WrcKH7l2PN5p1Q9A3GgCuM1T1srLfdbI/view?usp=sharing

## ✅ Instruções Suportadas

O montador reconhece e converte as seguintes instruções do conjunto RISC-V:

| Instrução | Descrição                         |
|-----------|----------------------------------|
| `lw`      | Carrega uma palavra da memória    |
| `sw`      | Armazena uma palavra na memória   |
| `sub`     | Subtração entre registradores     |
| `xor`     | OU exclusivo entre registradores  |
| `addi`    | Adição com valor imediato         |
| `srl`     | Deslocamento lógico à direita     |
| `beq`     | Desvio condicional se igual       |
| `add`     | Adição entre registradores        |
| `and`     | Operação AND bit a bit             |
| `ori`     | Operação OR com valor imediato    |

---

## 📚 Pseudocódigos e Lógica

A implementação inclui pseudocódigos detalhados para as etapas principais do montador: análise léxica, resolução de rótulos e geração do código binário. Esses pseudocódigos facilitam o entendimento do processo de tradução das instruções em linguagem assembly para suas representações binárias.

---

## 🔢 Bases Numéricas Suportadas

O montador suporta a interpretação de operandos em diferentes bases numéricas, permitindo maior flexibilidade na escrita do código assembly:

- Decimal (ex: `42`)
- Hexadecimal (prefixo `0x`, ex: `0x2A`)
- Binário (prefixo `0b`, ex: `0b101010`)

---

## 📦 Requisitos

- Sistema Linux (ou WSL no Windows)
- Compilador C (ex: `gcc`)
- Editor de texto para escrever os arquivos `.asm`

---

## ⚙️ Como Compilar e Executar

No terminal, execute os seguintes comandos:

```bash
gcc -o assembler assembler.c      # Compila o código em C
./assembler test.asm saida.hex   # Executa o montador com entrada e saída
cat saida.hex                    # Mostra o resultado final em binário


---

