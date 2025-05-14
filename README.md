# Arquimedes-Dos

## Instalação:
git clone https://github.com/Dragonmodder7/Arquimedes-Dos.git
cd Arquimedes-Dos

## Compilar o código:
clang -o arquimedes Arquimedes-Dos.c -lssl -lcrypto -lpthread

## Rodando o DoS:
./arquimedes -I (IP do site) -p (Porta) -t (threads) 
