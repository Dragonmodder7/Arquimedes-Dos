# Arquimedes-Dos 2.0

## Instalação:
git clone https://github.com/Dragonmodder7/Arquimedes-Dos.git
cd Arquimedes-Dos

## Compilar o código:
clang -o arquimedes Arquimedes-Dos.c -lssl -lcrypto -lpthread

## Rodando o DoS:
./arquimedes -I (ip) -p (porta) -t (threads) -l (IP de origem pro ataque, pode ser um ip de proxy list) -lp (porta do Ip origem do ataque) 
