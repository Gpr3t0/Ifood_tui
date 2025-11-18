#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "horario.h"

#define TOTAL_DIAS 7

const char* diasSemana[TOTAL_DIAS] = {
    "segunda-feira", "terca-feira", "quarta-feira",
    "quinta-feira",  "sexta-feira", "sabado", "domingo"
};

void cadastrarHorariosFuncionamento(int idRestaurante) {
    Horario horarios[TOTAL_DIAS];
    int diasAbertos[TOTAL_DIAS] = {0};
    char entradaDias[128];
    char horaInicioPadrao[6];
    char horaFimPadrao[6];
    int i;

    printf("\n--- Cadastro rapido de Horario ---\n");
    printf("Mapa dos dias:\n");
    for(i = 0; i < TOTAL_DIAS; i++) {
        strcpy(horarios[i].diaSemana, diasSemana[i]);
        horarios[i].aberto = 0;
        strcpy(horarios[i].horaInicio, "--:--");
        strcpy(horarios[i].horaFim, "--:--");
        printf("%d - %s\n", i + 1, diasSemana[i]);
    }

    printf("\nDigite os dias abertos separados por virgula (ex: 1,2,3,5): ");
    scanf(" %127[^\n]", entradaDias);

    char *token = strtok(entradaDias, ", ");
    int marcou = 0;
    while(token != NULL) {
        int dia = atoi(token);
        if(dia >= 1 && dia <= TOTAL_DIAS) {
            diasAbertos[dia - 1] = 1;
            horarios[dia - 1].aberto = 1;
            marcou = 1;
        }
        token = strtok(NULL, ", ");
    }

    if(marcou) {
        printf("Horario padrao de abertura (HH:MM): ");
        scanf("%5s", horaInicioPadrao);
        printf("Horario padrao de fechamento (HH:MM): ");
        scanf("%5s", horaFimPadrao);

        for(i = 0; i < TOTAL_DIAS; i++) {
            if(horarios[i].aberto) {
                strcpy(horarios[i].horaInicio, horaInicioPadrao);
                strcpy(horarios[i].horaFim, horaFimPadrao);
            }
        }
    } else {
        printf("Nenhum dia selecionado. Todos permanecerao fechados.\n");
    }

    int personalizar;
    printf("\nDeseja personalizar algum dia especifico? (1-Sim / 0-Nao): ");
    scanf("%d", &personalizar);

    while(personalizar == 1) {
        int diaEscolhido;
        printf("Informe o dia para personalizar (1-7) ou 0 para encerrar: ");
        scanf("%d", &diaEscolhido);

        if(diaEscolhido == 0) {
            break;
        }
        if(diaEscolhido < 1 || diaEscolhido > TOTAL_DIAS) {
            printf("Dia invalido!\n");
            continue;
        }

        diaEscolhido -= 1;
        printf("%s deve ficar aberto? (1-Sim / 0-Nao): ", diasSemana[diaEscolhido]);
        scanf("%d", &horarios[diaEscolhido].aberto);

        if(horarios[diaEscolhido].aberto) {
            printf("Horario de abertura (HH:MM): ");
            scanf("%5s", horarios[diaEscolhido].horaInicio);
            printf("Horario de fechamento (HH:MM): ");
            scanf("%5s", horarios[diaEscolhido].horaFim);
        } else {
            strcpy(horarios[diaEscolhido].horaInicio, "--:--");
            strcpy(horarios[diaEscolhido].horaFim, "--:--");
        }
    }

    char nomeArquivo[50];
    sprintf(nomeArquivo, "horarios_%d.dat", idRestaurante);

    FILE *arq = fopen(nomeArquivo, "wb");
    if(arq != NULL) {
        fwrite(horarios, sizeof(Horario), TOTAL_DIAS, arq);
        fclose(arq);
        printf("\nHorários cadastrados com sucesso!\n");
    } else {
        printf("\nErro ao salvar horários!\n");
    }
}

void exibirHorariosFuncionamento(int idRestaurante) {
    Horario horarios[TOTAL_DIAS];

    char nomeArquivo[50];
    sprintf(nomeArquivo, "horarios_%d.dat", idRestaurante);

    FILE *arq = fopen(nomeArquivo, "rb");
    if(arq == NULL) {
        printf("\nHorários ainda não cadastrados para este restaurante.\n");
        return;
    }
    fread(horarios, sizeof(Horario), TOTAL_DIAS, arq);
    fclose(arq);

    printf("\n--- Horários de Funcionamento ---\n");
    int i;
    for(i = 0; i < TOTAL_DIAS; i++) {
        printf("%-13s: ", horarios[i].diaSemana);
        if(horarios[i].aberto)
            printf("%s às %s\n", horarios[i].horaInicio, horarios[i].horaFim);
        else
            printf("Fechado\n");
    }
}
