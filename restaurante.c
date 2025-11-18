#include <stdio.h>
#include <string.h>
#include "restaurante.h"
#include "arquivo.h"

void cadastrarPrato(Usuario *restaurante) {
    Produto prato; // Cria struct para guardar dados do prato

    printf("\n=== CADASTRAR PRATO ===\n");
    printf("Nome do prato: ");
    getchar(); // Limpa buffer para leitura de string com espaço
    fgets(prato.nome, MAX_NOME, stdin);
    prato.nome[strcspn(prato.nome, "\n")] = 0;

    printf("Descricao: ");
    fgets(prato.descricao, 200, stdin);
    prato.descricao[strcspn(prato.descricao, "\n")] = 0;

    printf("Preco: R$ ");
    scanf("%f", &prato.preco); // Recebe preço do prato

    printf("Quantidade disponivel: ");
    scanf("%d", &prato.quantidade); // Quantidade disponível

    prato.id = gerarProximoId("produtos.dat", sizeof(Produto)); // Gera id automático
    prato.idRestaurante = restaurante->id; // Associa prato ao restaurante logado
    prato.favorito = 0; // Inicia como não favorito

    FILE *arquivo = fopen("produtos.dat", "ab");
    fwrite(&prato, sizeof(Produto), 1, arquivo); // Escreve estrutura
    fclose(arquivo);

    printf("Prato cadastrado com sucesso!\n");
}

void listarProdutosPorRestaurante(int idRestaurante) {
    FILE *arquivo = fopen("produtos.dat", "rb");
    if(arquivo == NULL) {
        printf("\nNenhum prato cadastrado ainda.\n");
        return;
    }

    Produto prato;
    int encontrou = 0;

    printf("\nID   | Nome                          | Preco    | Disponivel\n");
    printf("-------------------------------------------------------------\n");
    while(fread(&prato, sizeof(Produto), 1, arquivo)) {
        if(prato.idRestaurante == idRestaurante) {
            encontrou = 1;
            printf("%-4d | %-28s | R$ %6.2f | %d\n",
                   prato.id,
                   prato.nome,
                   prato.preco,
                   prato.quantidade);
        }
    }

    if(!encontrou) {
        printf("Nenhum prato cadastrado para este restaurante.\n");
    }

    fclose(arquivo);
}

int buscarProdutoPorId(int idProduto, Produto *produto) {
    FILE *arquivo = fopen("produtos.dat", "rb");
    if(arquivo == NULL) {
        return 0;
    }

    Produto temp;
    while(fread(&temp, sizeof(Produto), 1, arquivo)) {
        if(temp.id == idProduto) {
            *produto = temp;
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

int estoqueDisponivel(int idProduto, int quantidade) {
    FILE *arquivo = fopen("produtos.dat", "rb");
    if(arquivo == NULL) {
        return 0;
    }

    Produto temp;
    while(fread(&temp, sizeof(Produto), 1, arquivo)) {
        if(temp.id == idProduto) {
            fclose(arquivo);
            return temp.quantidade >= quantidade;
        }
    }

    fclose(arquivo);
    return 0;
}

int debitarEstoque(int idProduto, int quantidade) {
    FILE *arquivo = fopen("produtos.dat", "rb+");
    if(arquivo == NULL) {
        return 0;
    }

    Produto temp;
    while(fread(&temp, sizeof(Produto), 1, arquivo)) {
        if(temp.id == idProduto) {
            if(temp.quantidade < quantidade) {
                fclose(arquivo);
                return 0;
            }

            temp.quantidade -= quantidade;
            fseek(arquivo, -((long)sizeof(Produto)), SEEK_CUR);
            fwrite(&temp, sizeof(Produto), 1, arquivo);
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}