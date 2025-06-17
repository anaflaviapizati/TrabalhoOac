# Montador RISC-V Simplificado

Este projeto implementa um **montador (assembler) básico da arquitetura RISC-V**, responsável por converter instruções em linguagem de montagem para código de máquina (binário de 32 bits). Ele foi desenvolvido com fins **didáticos**, sendo parte de um trabalho de Organização e Arquitetura de Computadores (OAC).

---

## ✅ Instruções Suportadas

O montador reconhece e converte as seguintes instruções do conjunto RISC-V:

| Instrução | Descrição                         |
|----------|----------------------------------|
| `lw`     | Carrega uma palavra da memória    |
| `sw`     | Armazena uma palavra na memória   |
| `sub`    | Subtração entre registradores     |
| `xor`    | OU exclusivo entre registradores  |
| `addi`   | Adição com valor imediato         |
| `srl`    | Deslocamento lógico à direita     |
| `beq`    | Desvio condicional se igual       |

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
