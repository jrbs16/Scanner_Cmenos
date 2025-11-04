#############
Scanner para C-
#############

O programa implementa um analisador léxico para a linguagem C-. O scanner realiza a varredura de um código e identifica  tokens e símbolos especiais. Os comentários são reconhecidos e ignorados.

########
Aluno: Jarbas Ribeiro  
RA 189331

Funcionalidades Principais
#######
- Reconhecimento de palavras: `if`, `else`, `int`, `return`, `void`, `while`
- Identificação de identificadores e número
- Processamento de operadores e delimitadores: `+`, `-`, `*`, `/`, `==`, `<=`, `!=`, `{}`, `[]`, `()`
- Ignora comentário do tipo `/* ... */`
- Detecta erros (caracteres inválidos, comentários não terminados etc.)

Requisitos
#######
É necessário o compilador GCC. O programa foi testado com o GCC versão 13.3.0.
O programa foi testado no linux ubuntu.

Rodando o programa
##########
1. Compilar o arquivo "gcc -std=c11 -O0 -g -Wall -Wextra scanner_cminus.c -o scanner.exe"
2. Executar o arquivo .exe, informando qual arquivo de entrada será escaneado.
"./scanner.exe arquivo_de_entrada.cm"

