#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include "structs.h"

void cadastrarPrato(Usuario *restaurante);
void listarProdutosPorRestaurante(int idRestaurante);
int buscarProdutoPorId(int idProduto, Produto *produto);
int estoqueDisponivel(int idProduto, int quantidade);
int debitarEstoque(int idProduto, int quantidade);

#endif
