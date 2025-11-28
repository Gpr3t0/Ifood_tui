# ifood_tui

Projeto TUI (Text User Interface) em C para simular funcionalidades básicas de um serviço de pedidos (estilo iFood) com persistência simples em arquivos binários. Código organizado em módulos para usuário, restaurante, produtos, horários e operações de arquivo.

Sumário
- Visão geral
- Estrutura do repositório (arquivos)
- Principais funcionalidades (fluxos do cliente e do restaurante)
- Arquivos de dados gerados / persistência
- Como compilar
- Como executar
- Bibliotecas e headers usados
- Nomes dos integrantes
- Observações e recomendações

Visão geral
Este projeto foi desenvolvido para ser compilado e executado como um projeto em um IDE que agrupe múltiplos arquivos fonte. Não é fornecido um instalador; para executar você deve criar um novo projeto no Dev-C++, adicionar todos os arquivos .c e .h do repositório ao projeto (Project -> Add to Project) e então compilar a partir do IDE. Depois de compilado no Dev-C++, o executável pode ser executado a partir do próprio IDE ou a partir do console.

Funcionalidades principais
- Cadastro e login de clientes e restaurantes;
- Restaurantes cadastrarem pratos (produtos) e horários de funcionamento;
- Clientes explorarem restaurantes, adicionar pratos ao carrinho, favoritar itens, gerenciar endereços e finalizar pedidos;
- Persistência dos dados em arquivos binários simples (sem banco de dados).

Estrutura do repositório (arquivos principais)
- main.c
  - Menu principal: login (cliente/restaurante), cadastro de cliente e cadastro de restaurante.
- structs.h
  - Definições de estruturas (Usuario, Produto, Horario, Pedido, Endereco, etc.) e constantes.
- usuario.h / usuario.c
  - Login, cadastro, menu do cliente, fluxo de exploração de restaurantes, carrinho, favoritos, endereços, pedidos do cliente.
  - Funções auxiliares: cálculo de distância entre localidades, seleção de endereço, finalizar compra, salvar pedidos.
- restaurante.h / restaurante.c
  - Cadastro de pratos (produtos), listagem de produtos por restaurante, busca por produto, controle de estoque.
- horario.h / horario.c
  - Cadastro rápido e personalização de horários de funcionamento por restaurante e exibição dos horários.
- arquivo.h / arquivo.c
  - Função utilitária gerarProximoId para gerar IDs automáticos com base no tamanho do arquivo.
- Makefile.win
  - Arquivo de build para Windows (ex.: mingw32-make -f Makefile.win).
- ifood.exe (binários gerados / versão compilada)
- ifood.dev, ifood.layout (artefatos/metadata)
- output/main.exe (outro binário gerado, quando aplicável)
- arquivos objeto (.o) gerados pelo build:
  - main.o, horario.o, restaurante.o, usuario.o, arquivo.o
- README.md
  - Este arquivo.

Principais funcionalidades (como acessá-las dentro do aplicativo)
- Tela inicial (menu principal)
  - 1 — Login Cliente: Efetua login como cliente; após o login você acessa o Menu do Cliente.
  - 2 — Login Restaurante: Efetua login como restaurante; após o login você acessa o Menu do Restaurante.
  - 3 — Cadastrar Cliente: Cria uma nova conta de cliente (nome, email, senha e endereço opcional).
  - 4 — Cadastrar Restaurante: Cria uma nova conta de restaurante. Durante o cadastro o restaurante seleciona uma localidade pré-definida (ex.: TAGUATINGA, AGUAS CLARAS, CEILANDIA, etc.). Essa localidade será usada como o endereço principal do restaurante e será considerada pela funcionalidade de listar restaurantes por proximidade.
  - 0 — Sair: Encerra o programa.

- Menu do Cliente (após login como cliente)
  - 1 — Explorar restaurantes e pratos
    - Lista restaurantes disponíveis. Selecione o ID do restaurante para ver a lista de pratos.
    - Ao selecionar um prato, veja detalhes (nome, descrição, preço, disponibilidade) e escolha:
      - Adicionar ao carrinho (informar quantidade).
      - Favoritar / desfavoritar o produto.
  - 2 — Ver carrinho
    - Visualiza os itens no carrinho com subtotais.
    - Opções: remover item, esvaziar carrinho ou voltar.
  - 3 — Finalizar compra
    - Escolha um endereço salvo ou cadastre um novo.
    - Informe a forma de pagamento (ex: Cartão, Pix, Dinheiro).
    - Confirme o pedido — o sistema verifica estoque, debita itens e registra o pedido.
  - 4 — Favoritos
    - Lista seus produtos favoritos.
    - Permite adicionar ou remover favoritos informando o ID do produto.
  - 5 — Endereços de entrega
    - Lista endereços salvos associados ao cliente.
    - Permite cadastrar novo endereço (informando apelido, região/localidade e número/complemento) ou remover endereços.
    - Endereços salvos podem ser usados como endereço de entrega ao finalizar compra e também como referência para listar restaurantes por proximidade.
  - 6 — Meus pedidos
    - Mostra os pedidos já realizados pelo cliente com status e detalhes.
  - 7 — Restaurantes próximos
    - Escolha um endereço de referência (um dos seus endereços salvos) e veja os restaurantes ordenados por proximidade ao endereço escolhido.
    - A lista de proximidade utiliza as localizações selecionadas pelos restaurantes no momento do cadastro e as regiões dos seus endereços salvos para calcular a distância aproximada.
  - 0 — Voltar / Logout

- Menu do Restaurante (após login como restaurante)
  - Nota: o cadastro do restaurante (feito na tela inicial) inclui a seleção de localidade que define o endereço principal — essa localidade impacta a listagem por proximidade.
  - 1 — Cadastrar novo prato
    - Informe nome, descrição, preço e quantidade para registrar um prato disponível para venda.
  - 2 — Ver meus pratos
    - Lista todos os pratos cadastrados pelo restaurante com preço e estoque disponível.
  - 3 — Cadastrar horários de funcionamento
    - Informe os dias e horários em que o restaurante ficará aberto.
  - 4 — Ver horários cadastrados
    - Exibe os horários de funcionamento já cadastrados.
  - 5 — Pedidos recebidos
    - Lista pedidos feitos ao restaurante com detalhes e permite atualizar o status do pedido (ex: Em preparo, Saiu, Entregue).
  - 0 — Voltar / Logout

Observação: as instruções acima descrevem a experiência do usuário ao navegar pela aplicação (menus e opções). Elas não listam chamadas de código ou arquivos fonte; concentram-se apenas nas funcionalidades disponíveis e como acessá-las dentro do app.

Arquivos de dados gerados / persistência
- usuarios.dat         — armazenamento binário de estruturas Usuario
- produtos.dat         — armazenamento binário de estruturas Produto (pratos)
- pedidos.dat          — armazenamento binário de estruturas Pedido
- favoritos.dat        — lista binária de relações Favorito (idCliente, idProduto)
- horarios_<id>.dat    — arquivos binários por restaurante contendo array de Horario (ex.: horarios_3.dat)
- enderecos_<id>.dat   — arquivos binários por cliente contendo EnderecoSalvo (ex.: enderecos_5.dat)
- Observação: os arquivos são escritos em formato binário usando fwrite/fread. Fazer backup antes de testes que possam sobrescrever dados.

Como compilar
- Recomendado (Dev-C++):
  - Abra o Dev-C++ e crie um novo projeto (Console Application -> C).
  - Adicione todos os arquivos .c e .h do repositório ao projeto (Project -> Add to Project).
  - Compile/build a partir do IDE (Compile & Run / Build). O Dev-C++ agrupará os arquivos e gerará o executável.
- Alternativa (linha de comando / Make):
  - Se preferir compilar via linha de comando, certifique-se de compilar/ligar todos os arquivos fonte juntos. Exemplo:
    gcc main.c usuario.c restaurante.c horario.c arquivo.c -o ifood.exe
  - Se houver um Makefile compatível com seu ambiente, você pode tentar:
    mingw32-make -f Makefile.win
  - Observação: alguns ambientes exigem que todos os arquivos fontes sejam explicitamente incluídos no build; por isso o fluxo via IDE (Dev-C++) é o método recomendado para reproduzir o comportamento esperado do projeto.

Como executar
- Após compilar, execute:
  - ifood.exe
  - ou ./output/main.exe (se você tiver o executável dentro da pasta output)
- O programa é baseado em terminal/console (Windows). Use as teclas conforme instruções exibidas nos menus.

Bibliotecas e headers utilizados
- Headers padrão C usados no projeto:
  - stdio.h
  - stdlib.h
  - string.h
  - math.h
  - conio.h (apenas em main.c para getch/limpar tela; pode depender do compilador)
- Headers do projeto (módulos):
  - structs.h, usuario.h, restaurante.h, horario.h, arquivo.h
- Observação: o uso de conio.h e system("cls") torna a aplicação mais orientada a Windows; ao portar para Linux/macOS, altere chamadas de limpeza de tela e leitura de tecla.

Nomes dos integrantes
- JOÃO GABRIEL DA CRUZ VIANA
- HELENNA SALES TEIXEIRA ALVES
- IGOR VENDRAMINI PERINI
- HIGOR SAMUEL FERREIRA SILVA


Referências de implementação (onde procurar no código)
- Login / menus: main.c, usuario.c
- Cadastro de usuário / restaurante: usuario.c
- Produtos (pratos) e estoque: restaurante.c
- Horários: horario.c
- Geração de IDs automática: arquivo.c (gerarProximoId)
- Persistência de pedidos / favoritos / endereços: usuario.c, restaurante.c