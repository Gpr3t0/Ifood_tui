#ifndef USUARIO_H
#define USUARIO_H

#include "structs.h"


int fazerLogin(TipoUsuario tipo, Usuario *usuario);     // Login
void cadastrarUsuario(TipoUsuario tipo);               // Cadastro
void menuCliente(Usuario *usuario);                    // Menu Cliente
void menuRestaurante(Usuario *usuario);                // Menu Restaurante

#endif