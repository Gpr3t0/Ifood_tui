#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "usuario.h"
#include "arquivo.h"
#include "restaurante.h"
#include "horario.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define FAVORITOS_FILE "favoritos.dat"

void limparTela();
void pausar();

typedef struct {
    const char *nome;
    double latitude;
    double longitude;
} LocalidadeInfo;

static const LocalidadeInfo LOCALIDADES[] = {
    {"TAGUATINGA",   -15.833528, -48.056833},
    {"AGUAS CLARAS", -15.838139, -48.022945},
    {"CEILANDIA",    -15.821639, -48.113472},
    {"VICENTE PIRES",-15.800111, -48.026195},
    {"26 DE SETEMBRO",-15.762889, -48.025500},
    {"GUARA",        -15.826195, -47.978389},
    {"SAMAMBAIA",    -15.878695, -48.101083}
};

static const int TOTAL_LOCALIDADES = (int)(sizeof(LOCALIDADES) / sizeof(LOCALIDADES[0]));

static void limparBufferEntrada(void);
static void lerString(char *destino, size_t tamanho);
static void listarRestaurantes(void);
static int buscarRestaurantePorId(int id, Usuario *restaurante);
static void mostrarDetalhesProduto(const Produto *produto);
static void adicionarItemCarrinho(ItemCarrinho *carrinho, int *totalItens, Produto produto, int quantidade, int *idRestauranteAtual);
static void explorarRestaurantes(Usuario *cliente, ItemCarrinho *carrinho, int *totalItens, int *idRestauranteAtual);
static void exibirCarrinho(const ItemCarrinho *carrinho, int totalItens);
static void removerDoCarrinho(ItemCarrinho *carrinho, int *totalItens);
static void limparCarrinho(ItemCarrinho *carrinho, int *totalItens, int *idRestauranteAtual);
static void gerenciarCarrinho(ItemCarrinho *carrinho, int *totalItens, int *idRestauranteAtual);
static int favoritoExiste(int idCliente, int idProduto);
static void adicionarFavorito(int idCliente, int idProduto);
static void removerFavorito(int idCliente, int idProduto);
static void alternarFavorito(int idCliente, int idProduto);
static void listarFavoritosCliente(int idCliente);
static void menuFavoritos(int idCliente);
static void montarNomeArquivoEnderecos(int idCliente, char *destino, size_t tamanho);
static int carregarEnderecoPorId(int idCliente, int idEndereco, Endereco *endereco);
static void listarEnderecosCliente(int idCliente);
static void cadastrarEnderecoCliente(int idCliente);
static void removerEnderecoCliente(int idCliente);
static void menuEnderecosCliente(int idCliente);
static int selecionarEnderecoCliente(int idCliente, Endereco *endereco, const char *titulo);
static void listarRestaurantesProximos(Usuario *cliente);
static void listarLocalidadesDisponiveis(void);
static int escolherIndiceLocalidade(int permitirCancelar);
static void definirEnderecoPadrao(Endereco *destino, const char *numero, const char *nomeLocal);
static const char* obterNomeLocalidade(const Endereco *endereco);
static int buscarIndiceLocalidade(const char *nome);
static double calcularDistanciaEnderecos(const Endereco *cliente, const Endereco *restaurante);
static float calcularTotalCarrinho(const ItemCarrinho *carrinho, int totalItens);
static void salvarPedido(const Pedido *pedido);
static void finalizarCompra(Usuario *cliente, ItemCarrinho *carrinho, int *totalItens, int *idRestauranteAtual);
static void listarPedidosCliente(int idCliente);
static void menuPedidosCliente(int idCliente);
static void listarPedidosRestaurante(int idRestaurante);
static int atualizarStatusPedidoArquivo(int idPedido, int idRestaurante, const char *novoStatus);
static void gerenciarPedidosRestaurante(int idRestaurante);

int fazerLogin(TipoUsuario tipo, Usuario *usuario) {
    char email[MAX_NOME], senha[MAX_SENHA];
    printf("\n====================  LOGIN  ====================\n");
    printf("Email: ");
    scanf("%s", email);
    printf("Senha: ");
    scanf("%s", senha);

    FILE *arquivo = fopen("usuarios.dat", "rb");
    if(arquivo == NULL){
      printf("Erro ao abrir arquivo de usuarios!\n");
      return 0;
    }

    Usuario temp;
    while(fread(&temp, sizeof(Usuario), 1, arquivo)){
      if(strcmp(temp.email, email) == 0 &&
        strcmp(temp.senha, senha) == 0 &&
        temp.tipo == tipo){
        *usuario = temp;
        fclose(arquivo);
        printf("Login realizado com sucesso!\n");
        return 1;
      }
    }

    fclose(arquivo);
    printf("Usuario ou senha invalidos!\n");
    return 0;
}

void cadastrarUsuario(TipoUsuario tipo){
  Usuario novo;

  printf("====================  CADASTRO  ====================\n");
  printf("Nome: ");
  limparBufferEntrada();
  lerString(novo.nome, MAX_NOME);

  printf("Email: ");
  scanf("%s", novo.email);
  printf("Senha: ");
  scanf("%s", novo.senha);

  novo.tipo = tipo;
  novo.id = gerarProximoId("usuarios.dat", sizeof(Usuario));

  if(tipo == RESTAURANTE) {
      printf("\nSelecione a localidade do restaurante:\n");
      int indice = escolherIndiceLocalidade(0);
      definirEnderecoPadrao(&novo.enderecoPrincipal, "S/N", LOCALIDADES[indice].nome);
  } else {
      novo.enderecoPrincipal.rua[0] = '\0';
      novo.enderecoPrincipal.numero[0] = '\0';
      novo.enderecoPrincipal.bairro[0] = '\0';
      novo.enderecoPrincipal.cidade[0] = '\0';
  }

  FILE *arquivo = fopen("usuarios.dat", "ab");
  if(arquivo == NULL){
    printf("Erro ao salvar usuario!\n");
    return;
  }

  fwrite(&novo, sizeof(Usuario), 1, arquivo);
  fclose(arquivo);

  printf("Usuario cadastrado com sucesso!\n");
}

static void limparBufferEntrada(void) {
    int c;
    while((c = getchar()) != '\n' && c != EOF) {}
}

static void lerString(char *destino, size_t tamanho) {
    if(fgets(destino, (int)tamanho, stdin)) {
        destino[strcspn(destino, "\n")] = 0;
    }
}

static void listarLocalidadesDisponiveis(void) {
    int i;
    printf("\nLocais disponiveis:\n");
    for(i = 0; i < TOTAL_LOCALIDADES; i++) {
        printf("%d. %s\n", i + 1, LOCALIDADES[i].nome);
    }
}

static int escolherIndiceLocalidade(int permitirCancelar) {
    int opcao;
    while(1) {
        listarLocalidadesDisponiveis();
        printf("Escolha uma opcao%s: ", permitirCancelar ? " (0 para cancelar)" : "");
        scanf("%d", &opcao);

        if(permitirCancelar && opcao == 0) {
            return -1;
        }

        if(opcao >= 1 && opcao <= TOTAL_LOCALIDADES) {
            return opcao - 1;
        }

        printf("Opcao invalida!\n");
    }
}

static void definirEnderecoPadrao(Endereco *destino, const char *numero, const char *nomeLocal) {
    strncpy(destino->rua, nomeLocal, MAX_NOME);
    destino->rua[MAX_NOME - 1] = '\0';
    strncpy(destino->bairro, nomeLocal, sizeof(destino->bairro));
    destino->bairro[sizeof(destino->bairro) - 1] = '\0';
    strncpy(destino->cidade, "DF", sizeof(destino->cidade));
    destino->cidade[sizeof(destino->cidade) - 1] = '\0';

    if(numero != NULL && numero[0] != '\0') {
        strncpy(destino->numero, numero, sizeof(destino->numero));
    } else {
        strncpy(destino->numero, "S/N", sizeof(destino->numero));
    }
    destino->numero[sizeof(destino->numero) - 1] = '\0';
}

static const char* obterNomeLocalidade(const Endereco *endereco) {
    if(endereco == NULL || endereco->bairro[0] == '\0') {
        return NULL;
    }
    return endereco->bairro;
}

static int buscarIndiceLocalidade(const char *nome) {
    int i;
    if(nome == NULL) return -1;
    for(i = 0; i < TOTAL_LOCALIDADES; i++) {
        if(strcmp(LOCALIDADES[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

static double calcularDistanciaEnderecos(const Endereco *cliente, const Endereco *restaurante) {
    const char *nomeCliente = obterNomeLocalidade(cliente);
    const char *nomeRestaurante = obterNomeLocalidade(restaurante);

    int idxCliente = buscarIndiceLocalidade(nomeCliente);
    int idxRestaurante = buscarIndiceLocalidade(nomeRestaurante);

    if(idxCliente < 0 || idxRestaurante < 0) {
        return 1e9;
    }

    double latCliente = LOCALIDADES[idxCliente].latitude;
    double latRestaurante = LOCALIDADES[idxRestaurante].latitude;
    double lonCliente = LOCALIDADES[idxCliente].longitude;
    double lonRestaurante = LOCALIDADES[idxRestaurante].longitude;

    double deltaLat = (latCliente - latRestaurante) * 111.0;
    double mediaLatRad = ((latCliente + latRestaurante) / 2.0) * (M_PI / 180.0);
    double deltaLon = (lonCliente - lonRestaurante) * 111.0 * cos(mediaLatRad);

    return sqrt(deltaLat * deltaLat + deltaLon * deltaLon);
}

static void listarRestaurantes(void) {
    FILE *arquivo = fopen("usuarios.dat", "rb");
    if(arquivo == NULL) {
        printf("Nenhum restaurante cadastrado.\n");
        return;
    }

    Usuario temp;
    int encontrou = 0;
    printf("ID   | Nome do Restaurante\n");
    printf("-------------------------------\n");
    while(fread(&temp, sizeof(Usuario), 1, arquivo)) {
        if(temp.tipo == RESTAURANTE) {
            encontrou = 1;
            printf("%-4d | %s\n", temp.id, temp.nome);
        }
    }

    if(!encontrou) {
        printf("Nenhum restaurante cadastrado.\n");
    }

    fclose(arquivo);
}

static int buscarRestaurantePorId(int id, Usuario *restaurante) {
    FILE *arquivo = fopen("usuarios.dat", "rb");
    if(arquivo == NULL) {
        return 0;
    }

    Usuario temp;
    while(fread(&temp, sizeof(Usuario), 1, arquivo)) {
        if(temp.id == id && temp.tipo == RESTAURANTE) {
            *restaurante = temp;
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

static void mostrarDetalhesProduto(const Produto *produto) {
    printf("=== Detalhes do Produto ===\n");
    printf("ID: %d\n", produto->id);
    printf("Nome: %s\n", produto->nome);
    printf("Descricao: %s\n", produto->descricao);
    printf("Preco: R$ %.2f\n", produto->preco);
    printf("Disponivel: %d unidade(s)\n", produto->quantidade);
}

static void adicionarItemCarrinho(ItemCarrinho *carrinho, int *totalItens, Produto produto, int quantidade, int *idRestauranteAtual) {
    if(quantidade <= 0) {
        printf("Quantidade invalida.\n");
        return;
    }

    if(produto.quantidade == 0) {
        printf("Produto sem estoque no momento.\n");
        return;
    }

    if(quantidade > produto.quantidade) {
        printf("Quantidade solicitada acima do estoque disponivel.\n");
        return;
    }

    if(*totalItens >= MAX_CARRINHO) {
        printf("Carrinho cheio! Finalize a compra ou remova itens.\n");
        return;
    }

    if(*totalItens == 0) {
        *idRestauranteAtual = produto.idRestaurante;
    } else if(*idRestauranteAtual != produto.idRestaurante) {
        printf("O carrinho so pode conter itens do mesmo restaurante. Finalize ou esvazie antes de escolher outro.\n");
        return;
    }

    int i;
    for(i = 0; i < *totalItens; i++) {
        if(carrinho[i].produto.id == produto.id) {
            carrinho[i].quantidade += quantidade;
            printf("Quantidade atualizada no carrinho!\n");
            return;
        }
    }

    carrinho[*totalItens].produto = produto;
    carrinho[*totalItens].quantidade = quantidade;
    (*totalItens)++;
    printf("Produto adicionado ao carrinho!\n");
}

static void explorarRestaurantes(Usuario *cliente, ItemCarrinho *carrinho, int *totalItens, int *idRestauranteAtual) {
    int idRestaurante;
    while(1) {
        limparTela();
        printf("=== Restaurantes Disponiveis ===\n");
        listarRestaurantes();
        printf("\nDigite o ID do restaurante para ver os pratos (0 para voltar): ");
        scanf("%d", &idRestaurante);
        if(idRestaurante == 0) {
            break;
        }

        Usuario restauranteSelecionado;
        if(!buscarRestaurantePorId(idRestaurante, &restauranteSelecionado)) {
            printf("Restaurante nao encontrado!\n");
            pausar();
            continue;
        }

        int continuar = 1;
        while(continuar) {
            limparTela();
            printf("=== %s ===\n", restauranteSelecionado.nome);
            exibirHorariosFuncionamento(restauranteSelecionado.id);
            listarProdutosPorRestaurante(restauranteSelecionado.id);
            printf("\nDigite o ID do prato para ver detalhes (0 para outro restaurante): ");
            int idProduto;
            scanf("%d", &idProduto);
            if(idProduto == 0) {
                break;
            }

            Produto produto;
            if(!buscarProdutoPorId(idProduto, &produto) || produto.idRestaurante != restauranteSelecionado.id) {
                printf("Produto invalido para este restaurante.\n");
                pausar();
                continue;
            }

            limparTela();
            mostrarDetalhesProduto(&produto);
            printf("\n1. Adicionar ao carrinho\n");
            printf("2. Favoritar/Desfavoritar\n");
            printf("0. Voltar\n");
            printf("Escolha: ");
            int opcao;
            scanf("%d", &opcao);

            if(opcao == 1) {
                printf("Quantidade desejada: ");
                int quantidade;
                scanf("%d", &quantidade);
                adicionarItemCarrinho(carrinho, totalItens, produto, quantidade, idRestauranteAtual);
                pausar();
            } else if(opcao == 2) {
                alternarFavorito(cliente->id, produto.id);
                pausar();
            }
        }
    }
}

static void exibirCarrinho(const ItemCarrinho *carrinho, int totalItens) {
    if(totalItens == 0) {
        printf("Carrinho vazio.\n");
        return;
    }

    printf("ID   | Produto                      | Qtde | Unitario | Subtotal\n");
    printf("-----------------------------------------------------------------\n");
    int i;
    for(i = 0; i < totalItens; i++) {
        float subtotal = carrinho[i].quantidade * carrinho[i].produto.preco;
        printf("%-4d | %-27s | %-4d | R$ %6.2f | R$ %7.2f\n",
               carrinho[i].produto.id,
               carrinho[i].produto.nome,
               carrinho[i].quantidade,
               carrinho[i].produto.preco,
               subtotal);
    }
    printf("-----------------------------------------------------------------\n");
    printf("Total: R$ %.2f\n", calcularTotalCarrinho(carrinho, totalItens));
}

static void removerDoCarrinho(ItemCarrinho *carrinho, int *totalItens) {
    if(*totalItens == 0) {
        printf("Carrinho vazio.\n");
        return;
    }

    int idProduto;
    printf("Digite o ID do produto que deseja remover: ");
    scanf("%d", &idProduto);

    int encontrado = 0;
    int i;
    for(i = 0; i < *totalItens; i++) {
        if(carrinho[i].produto.id == idProduto) {
            encontrado = 1;
            int j;
            for(j = i; j < *totalItens - 1; j++) {
                carrinho[j] = carrinho[j + 1];
            }
            (*totalItens)--;
            printf("Item removido do carrinho.\n");
            break;
        }
    }

    if(!encontrado) {
        printf("Produto nao encontrado no carrinho.\n");
    }
}

static void limparCarrinho(ItemCarrinho *carrinho, int *totalItens, int *idRestauranteAtual) {
    (void)carrinho;
    *totalItens = 0;
    *idRestauranteAtual = 0;
    printf("Carrinho esvaziado.\n");
}

static void gerenciarCarrinho(ItemCarrinho *carrinho, int *totalItens, int *idRestauranteAtual) {
    int opcao;
    do {
        limparTela();
        printf("=== Meu Carrinho ===\n");
        exibirCarrinho(carrinho, *totalItens);
        printf("\n1. Remover item\n");
        printf("2. Esvaziar carrinho\n");
        printf("0. Voltar\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                removerDoCarrinho(carrinho, totalItens);
                pausar();
                break;
            case 2:
                limparCarrinho(carrinho, totalItens, idRestauranteAtual);
                pausar();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida!\n");
                pausar();
        }
    } while(opcao != 0);
}

static int favoritoExiste(int idCliente, int idProduto) {
    FILE *arquivo = fopen(FAVORITOS_FILE, "rb");
    if(arquivo == NULL) {
        return 0;
    }

    Favorito fav;
    while(fread(&fav, sizeof(Favorito), 1, arquivo)) {
        if(fav.idCliente == idCliente && fav.idProduto == idProduto) {
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

static void adicionarFavorito(int idCliente, int idProduto) {
    Favorito fav;
    fav.idCliente = idCliente;
    fav.idProduto = idProduto;

    FILE *arquivo = fopen(FAVORITOS_FILE, "ab");
    if(arquivo == NULL) {
        printf("Nao foi possivel salvar o favorito.\n");
        return;
    }

    fwrite(&fav, sizeof(Favorito), 1, arquivo);
    fclose(arquivo);
    printf("Produto adicionado aos favoritos!\n");
}

static void removerFavorito(int idCliente, int idProduto) {
    FILE *arquivo = fopen(FAVORITOS_FILE, "rb");
    if(arquivo == NULL) {
        return;
    }

    FILE *temp = fopen("favoritos.tmp", "wb");
    if(temp == NULL) {
        fclose(arquivo);
        return;
    }

    Favorito fav;
    while(fread(&fav, sizeof(Favorito), 1, arquivo)) {
        if(!(fav.idCliente == idCliente && fav.idProduto == idProduto)) {
            fwrite(&fav, sizeof(Favorito), 1, temp);
        }
    }

    fclose(arquivo);
    fclose(temp);
    remove(FAVORITOS_FILE);
    rename("favoritos.tmp", FAVORITOS_FILE);
    printf("Produto removido dos favoritos.\n");
}

static void alternarFavorito(int idCliente, int idProduto) {
    if(favoritoExiste(idCliente, idProduto)) {
        removerFavorito(idCliente, idProduto);
    } else {
        adicionarFavorito(idCliente, idProduto);
    }
}

static void listarFavoritosCliente(int idCliente) {
    FILE *arquivo = fopen(FAVORITOS_FILE, "rb");
    if(arquivo == NULL) {
        printf("Nenhum favorito cadastrado.\n");
        return;
    }

    Favorito fav;
    int encontrou = 0;
    while(fread(&fav, sizeof(Favorito), 1, arquivo)) {
        if(fav.idCliente == idCliente) {
            Produto produto;
            if(buscarProdutoPorId(fav.idProduto, &produto)) {
                if(!encontrou) {
                    printf("Meus favoritos:\n");
                    printf("-------------------------------\n");
                }
                encontrou = 1;
                printf("[%d] %s - R$ %.2f\n", produto.id, produto.nome, produto.preco);
            }
        }
    }

    if(!encontrou) {
        printf("Nenhum favorito cadastrado.\n");
    }

    fclose(arquivo);
}

static void menuFavoritos(int idCliente) {
    int opcao;
    do {
        limparTela();
        printf("=== Favoritos ===\n");
        listarFavoritosCliente(idCliente);
        printf("\n1. Adicionar/Remover favorito\n");
        printf("0. Voltar\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        if(opcao == 1) {
            printf("Informe o ID do produto: ");
            int idProduto;
            scanf("%d", &idProduto);
            Produto produto;
            if(!buscarProdutoPorId(idProduto, &produto)) {
                printf("Produto nao encontrado.\n");
            } else {
                alternarFavorito(idCliente, idProduto);
            }
            pausar();
        } else if(opcao != 0) {
            printf("Opcao invalida!\n");
            pausar();
        }
    } while(opcao != 0);
}

static void montarNomeArquivoEnderecos(int idCliente, char *destino, size_t tamanho) {
    snprintf(destino, tamanho, "enderecos_%d.dat", idCliente);
}

static int carregarEnderecoPorId(int idCliente, int idEndereco, Endereco *endereco) {
    char nomeArquivo[40];
    montarNomeArquivoEnderecos(idCliente, nomeArquivo, sizeof(nomeArquivo));

    FILE *arquivo = fopen(nomeArquivo, "rb");
    if(arquivo == NULL) {
        return 0;
    }

    EnderecoSalvo salvo;
    while(fread(&salvo, sizeof(EnderecoSalvo), 1, arquivo)) {
        if(salvo.id == idEndereco) {
            *endereco = salvo.endereco;
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

static void listarEnderecosCliente(int idCliente) {
    char nomeArquivo[40];
    montarNomeArquivoEnderecos(idCliente, nomeArquivo, sizeof(nomeArquivo));

    FILE *arquivo = fopen(nomeArquivo, "rb");
    if(arquivo == NULL) {
        printf("Nenhum endereco cadastrado.\n");
        return;
    }

    EnderecoSalvo salvo;
    int encontrou = 0;
    printf("ID | Apelido | Rua, Numero - Bairro, Cidade\n");
    printf("------------------------------------------------------\n");
    while(fread(&salvo, sizeof(EnderecoSalvo), 1, arquivo)) {
        encontrou = 1;
        printf("%-2d | %-7s | %s, %s - %s, %s\n",
               salvo.id,
               salvo.apelido,
               salvo.endereco.rua,
               salvo.endereco.numero,
               salvo.endereco.bairro,
               salvo.endereco.cidade);
    }

    if(!encontrou) {
        printf("Nenhum endereco cadastrado.\n");
    }

    fclose(arquivo);
}

static void cadastrarEnderecoCliente(int idCliente) {
    char nomeArquivo[40];
    montarNomeArquivoEnderecos(idCliente, nomeArquivo, sizeof(nomeArquivo));

    EnderecoSalvo novo;
    novo.idCliente = idCliente;
    novo.id = gerarProximoId(nomeArquivo, sizeof(EnderecoSalvo));

    printf("Apelido (ex: Casa, Trabalho): ");
    limparBufferEntrada();
    lerString(novo.apelido, MAX_APELIDO);

    printf("\nSelecione uma das regioes disponiveis:\n");
    int indice = escolherIndiceLocalidade(1);
    if(indice < 0) {
        printf("Cadastro de endereco cancelado.\n");
        return;
    }

    char numero[sizeof(novo.endereco.numero)];
    printf("Numero ou complemento: ");
    limparBufferEntrada();
    lerString(numero, sizeof(numero));
    definirEnderecoPadrao(&novo.endereco, numero, LOCALIDADES[indice].nome);

    FILE *arquivo = fopen(nomeArquivo, "ab");
    if(arquivo == NULL) {
        printf("Nao foi possivel salvar o endereco.\n");
        return;
    }

    fwrite(&novo, sizeof(EnderecoSalvo), 1, arquivo);
    fclose(arquivo);
    printf("Endereco salvo!\n");
}

static void removerEnderecoCliente(int idCliente) {
    char nomeArquivo[40];
    montarNomeArquivoEnderecos(idCliente, nomeArquivo, sizeof(nomeArquivo));

    FILE *arquivo = fopen(nomeArquivo, "rb");
    if(arquivo == NULL) {
        printf("Nenhum endereco para remover.\n");
        return;
    }

    FILE *temp = fopen("enderecos.tmp", "wb");
    if(temp == NULL) {
        fclose(arquivo);
        return;
    }

    int idRemover;
    printf("Digite o ID do endereco que deseja remover: ");
    scanf("%d", &idRemover);

    EnderecoSalvo salvo;
    int removido = 0;
    while(fread(&salvo, sizeof(EnderecoSalvo), 1, arquivo)) {
        if(salvo.id == idRemover) {
            removido = 1;
            continue;
        }
        fwrite(&salvo, sizeof(EnderecoSalvo), 1, temp);
    }

    fclose(arquivo);
    fclose(temp);
    remove(nomeArquivo);
    rename("enderecos.tmp", nomeArquivo);

    if(removido) {
        printf("Endereco removido.\n");
    } else {
        printf("Endereco nao encontrado.\n");
    }
}

static void menuEnderecosCliente(int idCliente) {
    int opcao;
    do {
        limparTela();
        printf("=== Meus Enderecos ===\n");
        listarEnderecosCliente(idCliente);
        printf("\n1. Cadastrar novo endereco\n");
        printf("2. Remover endereco\n");
        printf("0. Voltar\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                cadastrarEnderecoCliente(idCliente);
                pausar();
                break;
            case 2:
                removerEnderecoCliente(idCliente);
                pausar();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida!\n");
                pausar();
        }
    } while(opcao != 0);
}

static int selecionarEnderecoCliente(int idCliente, Endereco *endereco, const char *titulo) {
    while(1) {
        limparTela();
        printf("=== %s ===\n", titulo);
        listarEnderecosCliente(idCliente);
        printf("\nDigite o ID do endereco desejado\n");
        printf("(digite -1 para cadastrar um novo ou 0 para voltar): ");
        int idEscolhido;
        scanf("%d", &idEscolhido);

        if(idEscolhido == 0) {
            return 0;
        }

        if(idEscolhido == -1) {
            cadastrarEnderecoCliente(idCliente);
            pausar();
            continue;
        }

        if(carregarEnderecoPorId(idCliente, idEscolhido, endereco)) {
            return 1;
        }

        printf("Endereco nao encontrado.\n");
        pausar();
    }
}

static void listarRestaurantesProximos(Usuario *cliente) {
    Endereco referencia;
    if(!selecionarEnderecoCliente(cliente->id, &referencia, "Escolha o endereco de referencia")) {
        printf("Tudo certo, voltamos ao menu.\n");
        pausar();
        return;
    }

    FILE *arquivo = fopen("usuarios.dat", "rb");
    if(arquivo == NULL) {
        printf("Nenhum restaurante encontrado.\n");
        pausar();
        return;
    }

    typedef struct {
        Usuario restaurante;
        double distanciaKm;
    } RestauranteComDistancia;

    RestauranteComDistancia lista[MAX_PRODUTOS];
    int total = 0;

    Usuario temp;
    while(fread(&temp, sizeof(Usuario), 1, arquivo)) {
        if(temp.tipo == RESTAURANTE && temp.enderecoPrincipal.cidade[0] != '\0') {
            if(total < (int)(sizeof(lista) / sizeof(lista[0]))) {
                lista[total].restaurante = temp;
                lista[total].distanciaKm = calcularDistanciaEnderecos(&referencia, &temp.enderecoPrincipal);
                total++;
            }
        }
    }

    fclose(arquivo);

    if(total == 0) {
        printf("Nenhum restaurante com endereco cadastrado.\n");
        pausar();
        return;
    }

    int i;
    for(i = 0; i < total - 1; i++) {
        int j;
        for(j = i + 1; j < total; j++) {
            if(lista[j].distanciaKm < lista[i].distanciaKm) {
                RestauranteComDistancia aux = lista[i];
                lista[i] = lista[j];
                lista[j] = aux;
            }
        }
    }

    printf("\nRestaurantes proximos ao endereco selecionado:\n");
    printf("Base: %s, %s - %s, %s\n",
           referencia.rua, referencia.numero, referencia.bairro, referencia.cidade);
    printf("\nID | Nome                         | Localidade    | Dist (km)\n");
    printf("-------------------------------------------------------------\n");

    for(i = 0; i < total; i++) {
        printf("%-2d | %-27s | %-13s | %6.2f\n",
               lista[i].restaurante.id,
               lista[i].restaurante.nome,
               lista[i].restaurante.enderecoPrincipal.bairro,
               lista[i].distanciaKm);
    }

    pausar();
}

static float calcularTotalCarrinho(const ItemCarrinho *carrinho, int totalItens) {
    float total = 0;
    int i;
    for(i = 0; i < totalItens; i++) {
        total += carrinho[i].produto.preco * carrinho[i].quantidade;
    }
    return total;
}

static void salvarPedido(const Pedido *pedido) {
    FILE *arquivo = fopen("pedidos.dat", "ab");
    if(arquivo == NULL) {
        printf("Nao foi possivel registrar o pedido.\n");
        return;
    }

    fwrite(pedido, sizeof(Pedido), 1, arquivo);
    fclose(arquivo);
}

static void finalizarCompra(Usuario *cliente, ItemCarrinho *carrinho, int *totalItens, int *idRestauranteAtual) {
    if(*totalItens == 0) {
        printf("Carrinho vazio!\n");
        pausar();
        return;
    }

    Endereco enderecoEntrega;
    if(!selecionarEnderecoCliente(cliente->id, &enderecoEntrega, "Escolha o Endereco de Entrega")) {
        printf("Compra cancelada.\n");
        pausar();
        return;
    }

    int i;
    for(i = 0; i < *totalItens; i++) {
        if(!estoqueDisponivel(carrinho[i].produto.id, carrinho[i].quantidade)) {
            printf("Estoque insuficiente para o produto %s.\n", carrinho[i].produto.nome);
            pausar();
            return;
        }
    }

    printf("Forma de pagamento (ex: Cartao, Pix, Dinheiro): ");
    limparBufferEntrada();
    char formaPagamento[30];
    lerString(formaPagamento, sizeof(formaPagamento));

    for(i = 0; i < *totalItens; i++) {
        debitarEstoque(carrinho[i].produto.id, carrinho[i].quantidade);
    }

    Pedido pedido;
    pedido.id = gerarProximoId("pedidos.dat", sizeof(Pedido));
    pedido.idCliente = cliente->id;
    pedido.idRestaurante = *idRestauranteAtual;
    pedido.totalItens = *totalItens;
    pedido.endereco = enderecoEntrega;
    pedido.valorTotal = calcularTotalCarrinho(carrinho, *totalItens);
    strncpy(pedido.formaPagamento, formaPagamento, sizeof(pedido.formaPagamento));
    pedido.formaPagamento[sizeof(pedido.formaPagamento) - 1] = '\0';
    strcpy(pedido.status, "Recebido");

    for(i = 0; i < *totalItens; i++) {
        pedido.itens[i] = carrinho[i];
    }

    salvarPedido(&pedido);
    printf("Pedido #%d registrado com sucesso! Total R$ %.2f\n", pedido.id, pedido.valorTotal);
    limparCarrinho(carrinho, totalItens, idRestauranteAtual);
    pausar();
}

static void listarPedidosCliente(int idCliente) {
    FILE *arquivo = fopen("pedidos.dat", "rb");
    if(arquivo == NULL) {
        printf("Nenhum pedido encontrado.\n");
        return;
    }

    Pedido pedido;
    int encontrou = 0;
    while(fread(&pedido, sizeof(Pedido), 1, arquivo)) {
        if(pedido.idCliente == idCliente) {
            encontrou = 1;
            Usuario restaurante;
            char nomeRestaurante[MAX_NOME] = "Restaurante removido";
            if(buscarRestaurantePorId(pedido.idRestaurante, &restaurante)) {
                strncpy(nomeRestaurante, restaurante.nome, sizeof(nomeRestaurante));
                nomeRestaurante[sizeof(nomeRestaurante) - 1] = '\0';
            }
            printf("\nPedido #%d - Restaurante: %s\n", pedido.id, nomeRestaurante);
            printf("Status: %s | Pagamento: %s | Total: R$ %.2f\n",
                   pedido.status, pedido.formaPagamento, pedido.valorTotal);
            printf("Entrega: %s, %s - %s, %s\n",
                   pedido.endereco.rua,
                   pedido.endereco.numero,
                   pedido.endereco.bairro,
                   pedido.endereco.cidade);
            printf("Itens:\n");
            {
                int i;
                for(i = 0; i < pedido.totalItens; i++) {
                printf(" - %s x%d (R$ %.2f)\n",
                       pedido.itens[i].produto.nome,
                       pedido.itens[i].quantidade,
                       pedido.itens[i].produto.preco);
                }
            }
        }
    }

    if(!encontrou) {
        printf("Nenhum pedido registrado ainda.\n");
    }

    fclose(arquivo);
}

static void menuPedidosCliente(int idCliente) {
    limparTela();
    printf("=== Meus Pedidos ===\n");
    listarPedidosCliente(idCliente);
    pausar();
}

static void listarPedidosRestaurante(int idRestaurante) {
    FILE *arquivo = fopen("pedidos.dat", "rb");
    if(arquivo == NULL) {
        printf("Nenhum pedido encontrado.\n");
        return;
    }

    Pedido pedido;
    int encontrou = 0;
    while(fread(&pedido, sizeof(Pedido), 1, arquivo)) {
        if(pedido.idRestaurante == idRestaurante) {
            encontrou = 1;
            printf("\nPedido #%d - Cliente %d\n", pedido.id, pedido.idCliente);
            printf("Status: %s | Total: R$ %.2f\n", pedido.status, pedido.valorTotal);
            printf("Itens:\n");
            {
                int i;
                for(i = 0; i < pedido.totalItens; i++) {
                printf(" - %s x%d\n",
                       pedido.itens[i].produto.nome,
                       pedido.itens[i].quantidade);
                }
            }
        }
    }

    if(!encontrou) {
        printf("Nenhum pedido recebido ainda.\n");
    }

    fclose(arquivo);
}

static int atualizarStatusPedidoArquivo(int idPedido, int idRestaurante, const char *novoStatus) {
    FILE *arquivo = fopen("pedidos.dat", "rb+");
    if(arquivo == NULL) {
        return 0;
    }

    Pedido pedido;
    while(fread(&pedido, sizeof(Pedido), 1, arquivo)) {
        if(pedido.id == idPedido && pedido.idRestaurante == idRestaurante) {
            strncpy(pedido.status, novoStatus, sizeof(pedido.status));
            pedido.status[sizeof(pedido.status) - 1] = '\0';
            fseek(arquivo, -((long)sizeof(Pedido)), SEEK_CUR);
            fwrite(&pedido, sizeof(Pedido), 1, arquivo);
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

static void gerenciarPedidosRestaurante(int idRestaurante) {
    int opcao;
    do {
        limparTela();
        printf("=== Pedidos Recebidos ===\n");
        listarPedidosRestaurante(idRestaurante);
        printf("\n1. Atualizar status de um pedido\n");
        printf("0. Voltar\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        if(opcao == 1) {
            int idPedido;
            char novoStatus[20];
            printf("ID do pedido: ");
            scanf("%d", &idPedido);
            printf("Novo status (ex: Em preparo, Saiu, Entregue): ");
            limparBufferEntrada();
            lerString(novoStatus, sizeof(novoStatus));
            if(atualizarStatusPedidoArquivo(idPedido, idRestaurante, novoStatus)) {
                printf("Status atualizado!\n");
            } else {
                printf("Pedido nao encontrado.\n");
            }
            pausar();
        } else if(opcao != 0) {
            printf("Opcao invalida!\n");
            pausar();
        }
    } while(opcao != 0);
}

void menuCliente(Usuario *usuario) {
    ItemCarrinho carrinho[MAX_CARRINHO];
    int totalItens = 0;
    int idRestauranteAtual = 0;
    int opcao;

    do {
        limparTela();
        printf("=== Area do Cliente %s ===\n\n", usuario->nome);
        printf("1. Explorar restaurantes e pratos\n");
        printf("2. Ver carrinho\n");
        printf("3. Finalizar compra\n");
        printf("4. Favoritos\n");
        printf("5. Enderecos de entrega\n");
        printf("6. Meus pedidos\n");
        printf("7. Restaurantes proximos\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                explorarRestaurantes(usuario, carrinho, &totalItens, &idRestauranteAtual);
                break;
            case 2:
                gerenciarCarrinho(carrinho, &totalItens, &idRestauranteAtual);
                break;
            case 3:
                finalizarCompra(usuario, carrinho, &totalItens, &idRestauranteAtual);
                break;
            case 4:
                menuFavoritos(usuario->id);
                break;
            case 5:
                menuEnderecosCliente(usuario->id);
                break;
            case 6:
                menuPedidosCliente(usuario->id);
                break;
            case 7:
                listarRestaurantesProximos(usuario);
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida!\n");
                pausar();
        }
    } while(opcao != 0);
}

void menuRestaurante(Usuario *usuario) {
    int opcao;
    do {
        limparTela();
        printf("=== Area do Restaurante %s ===\n\n", usuario->nome);
        printf("1. Cadastrar novo prato\n");
        printf("2. Ver meus pratos\n");
        printf("3. Cadastrar horarios de funcionamento\n");
        printf("4. Ver horarios cadastrados\n");
        printf("5. Pedidos recebidos\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                cadastrarPrato(usuario);
                pausar();
                break;
            case 2:
                listarProdutosPorRestaurante(usuario->id);
                pausar();
                break;
            case 3:
                cadastrarHorariosFuncionamento(usuario->id);
                pausar();
                break;
            case 4:
                exibirHorariosFuncionamento(usuario->id);
                pausar();
                break;
            case 5:
                gerenciarPedidosRestaurante(usuario->id);
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida!\n");
                pausar();
        }
    } while(opcao != 0);
}
