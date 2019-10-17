#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define PORT 6000
#define IP_SERVER "10.34.59.139"


//send(s0, &buff, sizeof(buff), 0);
//recv(s0, &buff, sizeof(buff), 0);


int s0;
struct sockaddr_in server;

void conecta(){
    int success;
    s0 = socket(AF_INET, SOCK_STREAM, 0);

    if(s0<0){
        perror("opening stream socket");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET ;
    server.sin_port = htons (PORT);
    server.sin_addr.s_addr = inet_addr(IP_SERVER);

    success = connect(s0, (struct sockaddr *) &server, sizeof(server));
    if(success == 0){
        printf("\nConectado com sucesso!\n");
    }else{
        printf("\nAtencao, erro ao conectar!\n");
    }
}

//=========================================================
//            INICIALIZAÇÃO DAS REGRAS DO JOGO
//=========================================================
void regraDoJogo(){
    printf("====================================================================\n");
    printf("=          BEM VINDO AO JOGO SUPER TRUNFO DE ALIMENTOS             =\n");
    printf("====================================================================\n");
    printf("|                     REGRAS DO JOGO                               |\n");    
    printf("| 1- Ganha o alimento com maior valor no atributo Proteína         |\n");    
    printf("| 2- Ganha o alimento com menor valor no atributo Carboidrato      |\n");    
    printf("| 3- Ganha o alimento com menor valor no atributo Valor Energético |\n");    
    printf("| 4- A Carta Super Trunfo é WHEY PROTEIN ISO e esta ganha de todas |\n");
    printf("====================================================================\n");
}

//=========================================================
//            ESCOLHA DE ATRIBUTO
//=========================================================
int escolheAtrib(){
    int op = 0;
    do{
        printf("=======================\n");
        printf("|      ATRIBUTOS      |\n");    
        printf("| 1- Proteína         |\n");    
        printf("| 2- Carboidrato      |\n");    
        printf("| 3- Valor Energético |\n");
        printf("=======================\n");
        scanf("%d", &op);
    }while(op <= 0 || op > 3);
    return op;    
}

void game(){
    char nome[100];
    int op, minha_vez, cartas, fim_jogo = 0, vencedor = 0;
    float proteina, carboidrato, calorias;
    
    do{
        recv(s0, &minha_vez, sizeof(minha_vez), 0);
        recv(s0, &vencedor, sizeof(vencedor), 0);
        recv(s0, &fim_jogo, sizeof(fim_jogo), 0);
        recv(s0, &cartas, sizeof(cartas), 0);
        recv(s0, &nome, sizeof(nome), 0);
        recv(s0, &proteina, sizeof(proteina), 0);
        recv(s0, &carboidrato, sizeof(carboidrato), 0);
        recv(s0, &calorias, sizeof(calorias), 0);
        
        printf("***************************\n");
        printf("= Cartas: %d \n", cartas);
        printf("***************************\n");
        printf("===========================\n");
        printf("= %23s =\n", nome);
        printf("===========================\n");
        printf("= Proteina    -> %8.2f =\n", proteina);
        printf("= Carboidrato -> %8.2f =\n", carboidrato);
        printf("= Calorias    -> %8.2f =\n", calorias);
        printf("===========================\n");

        if(minha_vez == 1){
            if(fim_jogo == 0){
                printf("É a sua vez!\n");
                op = escolheAtrib();
                send(s0, &op, sizeof(op), 0);
            }else{
                if(vencedor == 1){
                    printf("FIM DE JOGO, O VENCEDOR FOI O JOGADOR 1\n\n");
                }else{
                    printf("FIM DE JOGO, O VENCEDOR FOI O JOGADOR 2\n\n");
                }
            }   
        }else{
            if(fim_jogo == 0){
                printf("Aguarde o outro jogador!\n");    
            }else{
                if(vencedor == 1){
                    printf("FIM DE JOGO, O VENCEDOR FOI O JOGADOR 1\n\n");
                }else{
                    printf("FIM DE JOGO, O VENCEDOR FOI O JOGADOR 2\n\n");
                }
            }
        }
    }while(fim_jogo == 0);
}

int main(){
    conecta();
    regraDoJogo();
    game();
}