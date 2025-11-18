#ifndef STRUCTS_H
#define STRUCTS_H


#define MAX_NOME 100
#define MAX_SENHA 50
#define MAX_PRODUTOS 200
#define MAX_CARRINHO 50
#define MAX_APELIDO 50


typedef enum {
    CLIENTE = 1,
    RESTAURANTE = 2

} TipoUsuario;


typedef struct {
    char rua[MAX_NOME];
    char numero[10];
    char bairro[50];
    char cidade[50];

} Endereco;


typedef struct{
    int id;
    char nome[MAX_NOME];
    char email[MAX_NOME];
    char senha[MAX_SENHA];
    TipoUsuario tipo;
    Endereco enderecoPrincipal;
} Usuario;


typedef struct {
    char diaSemana[20];
    char horaInicio[6];
    char horaFim[6];
    int aberto;
} Horario;


typedef struct {
    int id;
    int idRestaurante;
    char nome[MAX_NOME];
    char descricao[200];
    float preco;
    int quantidade;
    int favorito;

} Produto;


typedef struct {
    Produto produto; 
    int quantidade;

} ItemCarrinho;

typedef struct {
    int id;
    int idCliente;
    char apelido[MAX_APELIDO];
    Endereco endereco;
} EnderecoSalvo;

typedef struct {
    int idCliente;
    int idProduto;
} Favorito;


typedef struct {
    int id;
    int idCliente;
    int idRestaurante;
    ItemCarrinho itens[MAX_CARRINHO];
    int totalItens;
    Endereco endereco;
    float valorTotal;
    char formaPagamento[30];
    char status[20];

} Pedido;

#endif // STRUCTS_H