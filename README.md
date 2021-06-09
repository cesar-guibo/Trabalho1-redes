# TicTacToe

## Membros
- Bruno de Souza Menzzano - 9790795

- César Henrique de Araujo Guibo - 11218705

- Frederico Bulhões Ribeiro de Souza - 11202440

- Leonardo Fonseca Pinheiro - 11219241

- Maria Eduarda Kawakami Moreira - 11218751

## Instruções de Instalação
    mkdir build && cd build
    cmake ..
    make

Serão gerados dois executáveis dentro da pasta build.  

## Para executar:

    ./server
e

    ./client

## Opções

Caso queira especificar as portas e/ou endereço:

    ./server -p [porta] -a [endereco]
    ./client -p [porta] -a [endereco]

A porta padrão é 12345, e o endereço padrão 127.0.0.1

## Sistema de testes
- ArchLinux

- Versão do kernel: 5.10.39-1-lts

- Gcc 11.1.0

- CMake 3.14+