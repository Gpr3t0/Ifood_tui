# 🍔 iFood TUI

Interface de terminal feita em C para simular o fluxo do iFood entre clientes e restaurantes. Tudo acontece no console, com arquivos `.dat` guardando usuários, pratos, horários, favoritos, endereços e pedidos.

---

## 🧱 Estrutura Principal

| Arquivo           | Papel                                                                 |
|-------------------|-----------------------------------------------------------------------|
| `main.c`          | Menu inicial (login/cadastro de cliente e restaurante).               |
| `usuario.c/h`     | Toda a experiência do usuário: login, menus, carrinho, pedidos etc.    |
| `restaurante.c/h` | Cadastro e gerenciamento de pratos + estoque.                         |
| `horario.c/h`     | Cadastro rápido de horários por semana e exibição.                    |
| `arquivo.c/h`     | Funções utilitárias (por exemplo, gerar ID incremental).              |
| `structs.h`       | Todas as structs compartilhadas (Usuário, Produto, Pedido...).        |
| `Makefile.win`    | Script usado pelo Dev-C++/MinGW para compilar tudo.                   |

Arquivos `.dat` são criados automaticamente na raiz para armazenar os dados (ex.: `usuarios.dat`, `produtos.dat`, `pedidos.dat`, `horarios_<id>.dat`, `enderecos_<id>.dat`, `favoritos.dat`).

---

## 🧑‍🍳 Fluxo do Restaurante

1. **Login / Cadastro:** restaurante cria conta com localidade fixa (Taguatinga, Guará etc.).
2. **Menu interno:**
   - ➕ Cadastrar pratos (nome, descrição, preço, quantidade).
   - 📋 Listar pratos existentes.
   - 🕒 Definir horários com preenchimento rápido (dias em lote + ajustes finos).
   - 📅 Ver horários salvos.
   - 🧾 Acompanhar pedidos recebidos e atualizar status (ex.: “Em preparo”, “Saiu para entrega”).

Quantidade dos pratos é debitada automaticamente quando um pedido é confirmado.

---

## 🛒 Fluxo do Cliente

1. **Login / Cadastro:** cliente cria conta e depois gere seus próprios endereços (apelidos + regiões).
2. **Menu interno:**
   - 🔍 Explorar restaurantes, ver pratos, detalhes e horários.
   - ❤️ Favoritar pratos e consultar a lista depois.
   - 🛍️ Montar carrinho (somente itens de um restaurante por vez).
   - 📦 Fechar pedido (escolhe endereço salvo, forma de pagamento, confere total e salva em `pedidos.dat`).
   - 📫 Consultar histórico de pedidos com status atualizado pelo restaurante.
   - 📍 Ver os restaurantes mais próximos com base em qualquer endereço salvo (usa lat/long das regiões oferecidas).

Endereços e restaurantes sempre usam as mesmas regiões pré-listadas, facilitando a ordenação por distância.

---

## ⚙️ Como Rodar (Dev-C++ / MinGW)

1. Verifique se o Dev-C++ 5.11 (ou MinGW equivalente) está instalado com GCC.
2. Abra o projeto na pasta `ifood_tui` ou rode no terminal do MinGW:
   ```bash
   mingw32-make -f Makefile.win
   ```
3. O executável `ifood.exe` será gerado na raiz. Basta executar no terminal (`./ifood.exe`) para iniciar os menus.

> 💡 Se aparecer erro de formatação dos laços `for`, garanta que está usando o padrão C90 (já tratado no código, mas é bom conferir as configurações do compilador).

---

## 📁 Dicas e Arquivos Úteis

- Apague os `.dat` se quiser “resetar” os dados (o sistema recria tudo).
- Para testes rápidos:
  - Cadastre um restaurante, inclua alguns pratos e horários.
  - Cadastre um cliente, crie endereços e monte um pedido para ver o fluxo completo.
- Os horários ficam em arquivos por restaurante (`horarios_<id>.dat`), então dá para inspecionar com um editor se necessário.

---

Qualquer ajuste de texto, novas regiões ou regras diferentes podem ser feitos direto nos arquivos `.c` e `.h`.😄

