#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "structs.h"
#include "usuario.h"
#include "restaurante.h"

void limparTela() {
    system("cls");
}

void pausar() {
    printf("\n\nPressione qualquer tecla para continuar...");
    getch();
}

int main() {
    int opcao;  
    Usuario usuarioLogado;
    int logado = 0;       

    while(1) { // Loop infinito at� sair
        limparTela();
        printf("========================================\n");
        printf("       BEM-VINDO AO IFOOD       \n");
        printf("========================================\n\n");

        if(!logado) { // S� aparece menu de login/cadastro se n�o estiver logado
            printf("1. Login Cliente\n");
            printf("2. Login Restaurante\n");
            printf("3. Cadastrar Cliente\n");
            printf("4. Cadastrar Restaurante\n");
            printf("0. Sair\n");
            printf("\nEscolha uma opcao: ");
            scanf("%d", &opcao); 

            switch(opcao) {
                case 1:
                    if(fazerLogin(CLIENTE, &usuarioLogado)) {
                        logado = 1; // Marca como logado
                        menuCliente(&usuarioLogado);
                        logado = 0;
                    }
                    break;
                case 2:
                    if(fazerLogin(RESTAURANTE, &usuarioLogado)) {
                        logado = 1;
                        menuRestaurante(&usuarioLogado);
                        logado = 0;
                    }
                    break;
                case 3:
                    cadastrarUsuario(CLIENTE); // Cadastro cliente
                    pausar();
                    break;
                case 4:
                    cadastrarUsuario(RESTAURANTE); // Cadastro restaurante
                    pausar();
                    break;
                case 0:
                    return 0; // Sai do programa
                default:
                    printf("Opcao invalida!");
                    pausar();
            }
        }
    }
    return 0;
}

