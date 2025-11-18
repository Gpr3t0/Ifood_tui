#include <stdio.h>
#include "arquivo.h"


int gerarProximoId(const char *nomeArquivo, size_t tamanhoRegistro) {
  FILE *arquivo = fopen(nomeArquivo, "rb");
  if(arquivo == NULL) return 1;

  fseek(arquivo, 0, SEEK_END);
  long tamanhoArquivo = ftell(arquivo);
  fclose(arquivo);

  return (tamanhoArquivo / tamanhoRegistro) + 1;

} 