#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 6000

int sock, length, s[1], i = 0;
struct sockaddr_in server;

void conecta(){
    sock = socket(AF_INET, SOCK_STREAM, 0);
   
    if(sock < 0){
        perror("opening stream socket");
        exit(0);
    }
   
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET ;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons (PORT);
    
    if(bind(sock, (struct sockaddr *)&server, sizeof (server)) < 0){
        perror("binding stream socket");
        exit(0);
    }

    listen(sock,2);

    printf("Servidor: Aguardando conexao!\n");
    
    for(i = 0; i < 2; i++){
        s[i] = accept(sock,(struct sockaddr *)0,(socklen_t *)0);
        
        if(s[i] == -1){
            perror("Accept");
            exit(1);
        }
        else{
            printf("Accept[%d] ok!\n", i);
        }    
    } 
}


int cartasJog1= 0, cartasJog2= 0, ganhou= 0;

//=========================================================
//             STRUCT ATRIBUTOS DAS CARTAS
//=========================================================
struct Atributos{
    char nome[100];
    float carboidrato, proteina, valorEnergetico;
};

struct Atributos atributos[32];

//=========================================================
//                     STRUCT CARTAS
//=========================================================
struct Carta{
    int id;
    struct Carta *prox;
};

struct Carta *inicio1 = NULL, *fim1 = NULL, *inicio2 = NULL, *fim2 = NULL, *inicio3 = NULL, *fim3 = NULL;

//=========================================================
//                     IMPRIMI LINHA
//=========================================================
void imprimeLinha(){
    printf("=========================\n");
}

//=========================================================
//              LÊ ARQUIVO E COLOCA NA STRUCT
//=========================================================
void leArquivo(){
    FILE * carts;
    int i = 0;
    
    carts = fopen ("alimentos.txt", "r") ;
    while(!feof(carts)){
        fscanf (carts, "%s %f %f %f", &atributos[i].nome, &atributos[i].proteina, &atributos[i].carboidrato, &atributos[i].valorEnergetico);
        //printf(" %d |%20s | %.2f | %.2f | %.2f |\n", i ,atributos[i].nome, atributos[i].proteina, atributos[i].carboidrato, atributos[i].valorEnergetico);
        i++;
	}
}

//=========================================================
//               INSERE NO FIM DA FILA
//=========================================================
void InsereFim(struct Carta **inicio, struct Carta **fim, int id){
    struct Carta *novo;
    novo= (struct Carta *) malloc(sizeof(struct Carta));

    if(novo){
        novo->id = id;
        novo->prox = NULL;
        if(*inicio == NULL){
            *inicio = novo;
        }else{
            (*fim)->prox = novo;
        }
        *fim = novo;
    }   
}

//=========================================================
//              EXCLUI NÓ DO INÍCIO DA FILA
//=========================================================
void ExcluiInicio(struct Carta **inicio, struct Carta **fim){
    struct Carta *novo;

    if(*inicio==NULL){
        printf("\nLista Vazia\n");
    }else{
        novo= *inicio;
        *inicio= (*inicio)->prox;
        if(*inicio==NULL){
            *fim= NULL;
        }
        free(novo);
    }
}

//=========================================================
//                    BUSCA NA FILA
//=========================================================
struct Carta* buscaEndereco(struct Carta **inicio, int id){
    struct Carta *novo, *aux;
    novo = *inicio;
    int flag = 0; 

    if(novo==NULL){
        //printf("\nLista Vazia!!!\n");
        return 0;
    }else{
        while(novo != NULL){
            if(id == (novo->id)){                
                //printf("| end: %d | id: %d | prox: %d |\n", novo, novo->id, novo->prox);
                aux = novo;
                flag = 1;
                return aux;
            }
            novo = novo->prox;
        }
        if(flag == 0){
            //printf("\nNao foi encontrado o alimento na Lista!\n\a");
            return 0; 
        }
        flag = 0;
    }
}

//========================================================
//                  IMPRIME FILA 
//========================================================
void ImprimeFILA(struct Carta **inicio){
    struct Carta *novo;
    novo= *inicio;
    if(novo==NULL){
        printf("\n\tLISTA VAZIA!!!\n");
    }else{
        printf("| ID |         NOME        |  PROTEINA   |  CARBOIDRATO  |  KCAL  |\n");
        imprimeLinha();
        while(novo!=NULL){
            printf("| %2d | %20s| %11.2f | %13.2f | %6.2f |\n", novo->id ,atributos[novo->id].nome, atributos[novo->id].proteina, atributos[novo->id].carboidrato, atributos[novo->id].valorEnergetico);
            //printf("| end: %d | valor: %15d | prox: %d |\n", novo, novo->id, novo->prox);
            novo= novo->prox;
        }
    }
}

//=========================================================
//                 MOSTRA CARTA
//=========================================================
void mostraCarta(struct Carta **inicio){
    struct Carta *novo;
    novo = *inicio;

    printf("===========================\n");
    printf("= %23s =\n", atributos[novo->id].nome);
    printf("===========================\n");
    printf("= Proteina    -> %8.2f =\n", atributos[novo->id].proteina);
    printf("= Carboidrato -> %8.2f =\n", atributos[novo->id].carboidrato);
    printf("= Calorias    -> %8.2f =\n", atributos[novo->id].valorEnergetico);
    printf("===========================\n");
}

//=========================================================
//                      EMPATE
//=========================================================
void empate(){
    InsereFim(&inicio3, &fim3, inicio1->id);
    ExcluiInicio(&inicio1, &fim1);
    cartasJog1--;
    InsereFim(&inicio3, &fim3, inicio2->id);
    ExcluiInicio(&inicio2, &fim2);
    cartasJog2--;
    //ImprimeFILA(&inicio3);//teste
}

//=========================================================
//               GANHOU DEPOIS DO EMPATE
//=========================================================
void ganhouEmpate(int ganhador){
    //printf("ENTREI NA GANHOU EMPATE\n");
    struct Carta *novo3;
    novo3 = inicio3;

    while(novo3 !=  NULL){
        //printf("ID DA CARTA NA FILA 3: %d\n", novo3->id);
        if (ganhador == 1){
            InsereFim(&inicio1, &fim1, novo3->id);
            novo3 = novo3->prox;
            ExcluiInicio(&inicio3, &fim3);
            cartasJog1++;
        }else{
            InsereFim(&inicio2, &fim2, novo3->id);
            novo3 = novo3->prox;
            ExcluiInicio(&inicio3, &fim3);
            cartasJog2++;
        }
    }
}

//=========================================================
//            COMPARAÇÃO DE ATRIBUTO
//=========================================================
void comparaAtributo(int vez, int op){
    switch(op){
        case 1://PROTEINA
            if(strcmp(atributos[inicio1->id].nome, "Whey_Protein_Iso") == 0){
                printf("Jogador 1 ganhou, TRUNFO!\n");
                mostraCarta(&inicio1);
                mostraCarta(&inicio2);
                InsereFim(&inicio1, &fim1, inicio1->id);
                InsereFim(&inicio1, &fim1, inicio2->id);
                cartasJog1++;
                ExcluiInicio(&inicio1, &fim1);          
                ExcluiInicio(&inicio2, &fim2);
                cartasJog2--;
                ganhou = 1;
            }else if(strcmp(atributos[inicio2->id].nome, "Whey_Protein_Iso") == 0){ // se jogador 2 ganhou no trunfo
                printf("Jogador 2 ganhou, TRUNFO!\n");
                mostraCarta(&inicio1);
                mostraCarta(&inicio2);
                InsereFim(&inicio2, &fim2, inicio2->id);
                InsereFim(&inicio2, &fim2, inicio1->id);
                cartasJog2++;
                ExcluiInicio(&inicio2, &fim2);
                ExcluiInicio(&inicio1, &fim1);
                cartasJog1--;
                ganhou = 2;
            }else{
                if(atributos[inicio1->id].proteina > atributos[inicio2->id].proteina){
                    printf("Jogador 1 ganhou\n");
                    mostraCarta(&inicio1);
                    mostraCarta(&inicio2);
                    InsereFim(&inicio1, &fim1, inicio1->id);
                    InsereFim(&inicio1, &fim1, inicio2->id);
                    cartasJog1++;
                    ExcluiInicio(&inicio1, &fim1);
                    ExcluiInicio(&inicio2, &fim2);
                    cartasJog2--;
                    ganhouEmpate(1);
                    ganhou = 1;
                }else if (atributos[inicio1->id].proteina < atributos[inicio2->id].proteina){
                    printf("Jogador 2 ganhou\n");
                    mostraCarta(&inicio1);
                    mostraCarta(&inicio2);
                    InsereFim(&inicio2, &fim2, inicio2->id);
                    InsereFim(&inicio2, &fim2, inicio1->id);
                    cartasJog2++;
                    ExcluiInicio(&inicio2, &fim2);
                    ExcluiInicio(&inicio1, &fim1);
                    cartasJog1--;
                    ganhouEmpate(2);
                    ganhou = 2;
                }else{
                    if(vez == 0){
                        ganhou = 1;
                    }else{
                        ganhou = 2;
                    }
                    printf("empatou\n");
                    empate();
                }
            }
        break;
        
        case 2://CARBOIDRATO
            if(strcmp(atributos[inicio1->id].nome, "Whey_Protein_Iso") == 0){ // se jogador 1 ganhou no trunfo
                printf("Jogador 1 ganhou, TRUNFO!\n");
                mostraCarta(&inicio1);
                mostraCarta(&inicio2);
                InsereFim(&inicio1, &fim1, inicio1->id);
                InsereFim(&inicio1, &fim1, inicio2->id);
                cartasJog1++;
                ExcluiInicio(&inicio1, &fim1);          
                ExcluiInicio(&inicio2, &fim2);
                cartasJog2--;
                ganhou = 1;
            }else if(strcmp(atributos[inicio2->id].nome, "Whey_Protein_Iso") == 0){ // se jogador 2 ganhou no trunfo
                printf("Jogador 2 ganhou, TRUNFO!\n");
                mostraCarta(&inicio1);
                mostraCarta(&inicio2);
                InsereFim(&inicio2, &fim2, inicio2->id);
                InsereFim(&inicio2, &fim2, inicio1->id);
                cartasJog2++;
                ExcluiInicio(&inicio2, &fim2);
                ExcluiInicio(&inicio1, &fim1);
                cartasJog1--;
                ganhou = 2;
            }else{
                if(atributos[inicio1->id].carboidrato < atributos[inicio2->id].carboidrato){
                    printf("Jogador 1 ganhou\n");
                    mostraCarta(&inicio1);
                    mostraCarta(&inicio2);
                    InsereFim(&inicio1, &fim1, inicio1->id);
                    InsereFim(&inicio1, &fim1, inicio2->id);
                    cartasJog1++;
                    ExcluiInicio(&inicio1, &fim1);
                    ExcluiInicio(&inicio2, &fim2);
                    cartasJog2--;
                    ganhouEmpate(1);
                    ganhou = 1;
                }else if (atributos[inicio1->id].carboidrato > atributos[inicio2->id].carboidrato){
                    printf("Jogador 2 ganhou\n");
                    mostraCarta(&inicio1);
                    mostraCarta(&inicio2);
                    InsereFim(&inicio2, &fim2, inicio2->id);
                    InsereFim(&inicio2, &fim2, inicio1->id);
                    cartasJog2++;
                    ExcluiInicio(&inicio2, &fim2);
                    ExcluiInicio(&inicio1, &fim1);
                    cartasJog1--;
                    ganhouEmpate(2);
                    ganhou = 2;
                }else{
                    if(vez == 0){
                        ganhou = 1;
                    }else{
                        ganhou = 2;
                    }
                    printf("empatou\n");
                    empate();
                }
            }
        break;
        
        case 3://VALOR ENERGETICO
            if(strcmp(atributos[inicio1->id].nome, "Whey_Protein_Iso") == 0){
                printf("Jogador 1 ganhou, TRUNFO!\n");
                mostraCarta(&inicio1);
                mostraCarta(&inicio2);
                InsereFim(&inicio1, &fim1, inicio1->id);
                InsereFim(&inicio1, &fim1, inicio2->id);
                cartasJog1++;
                ExcluiInicio(&inicio1, &fim1);          
                ExcluiInicio(&inicio2, &fim2);
                cartasJog2--;
                ganhou = 1;
            }else if(strcmp(atributos[inicio2->id].nome, "Whey_Protein_Iso") == 0){
                printf("Jogador 2 ganhou, TRUNFO!\n");
                mostraCarta(&inicio1);
                mostraCarta(&inicio2);
                InsereFim(&inicio2, &fim2, inicio2->id);
                InsereFim(&inicio2, &fim2, inicio1->id);
                cartasJog2++;
                ExcluiInicio(&inicio2, &fim2);
                ExcluiInicio(&inicio1, &fim1);
                cartasJog1--;
                ganhou = 2;
            }else{
                if(atributos[inicio1->id].valorEnergetico < atributos[inicio2->id].valorEnergetico){
                    printf("Jogador 1 ganhou\n");
                    mostraCarta(&inicio1);
                    mostraCarta(&inicio2);
                    InsereFim(&inicio1, &fim1, inicio1->id);
                    InsereFim(&inicio1, &fim1, inicio2->id);
                    cartasJog1++;
                    ExcluiInicio(&inicio1, &fim1);
                    ExcluiInicio(&inicio2, &fim2);
                    cartasJog2--;
                    ganhouEmpate(1);
                    ganhou = 1;
                }else if (atributos[inicio1->id].valorEnergetico > atributos[inicio2->id].valorEnergetico){
                    printf("Jogador 2 ganhou\n");
                    mostraCarta(&inicio1);
                    mostraCarta(&inicio2);
                    InsereFim(&inicio2, &fim2, inicio2->id);
                    InsereFim(&inicio2, &fim2, inicio1->id);
                    cartasJog2++;
                    ExcluiInicio(&inicio2, &fim2);
                    ExcluiInicio(&inicio1, &fim1);
                    cartasJog1--;
                    ganhouEmpate(2);
                    ganhou = 2;
                }else{
                    if(vez == 0){
                        ganhou = 1;
                    }else{
                        ganhou = 2;
                    }
                    printf("empatou\n");
                    empate();   
                }
            }
        break;
        
        default:
            printf("Atributo inexistente\n");
        break;
    }
}

//=========================================================
//                DIVISÃO DAS CARTAS
//=========================================================
void divideCartas() {
    int aux= 0, cont= 0;
    srand(time(NULL));
    while(cont < 32) {
        aux = (rand() % 32);
        if(buscaEndereco(&inicio1, aux) == 0 && cartasJog1 < 16 && buscaEndereco(&inicio2, aux) == 0){
            InsereFim(&inicio1, &fim1, aux);
            cont++;
            cartasJog1++;
        }
        else if(buscaEndereco(&inicio2, aux) == 0 && cartasJog2 < 16 && buscaEndereco(&inicio1, aux) == 0) {
            InsereFim(&inicio2, &fim2, aux);
            cont++;
            cartasJog2++;
        }
        else {
            //printf("ja tem\n");
        }
    }
}

//=========================================================
//                      JOGO
//=========================================================
void game(){
    int inicia= 0, flag = 0, op, joga = 1, espera = 0, fim_jogo = 0, vencedor = 0;
    float proteina, carboidrato, caloria;
    
    divideCartas();
    
    srand(time(NULL));
    inicia = (rand() % 2);
    printf("SORTEEI O JOGADOR %d\n", inicia+1);
    do{
        if(inicia == 0){
            printf("JOGADOR 1 COMEÇA JOGANDO\n");
            mostraCarta(&inicio1);
            
            send(s[0], &joga, sizeof(joga), 0);
            send(s[1], &espera, sizeof(espera), 0);

            send(s[0], &vencedor, sizeof(vencedor), 0);
            send(s[1], &vencedor, sizeof(vencedor), 0);

            send(s[0], &fim_jogo, sizeof(fim_jogo), 0);
            send(s[1], &fim_jogo, sizeof(fim_jogo), 0);

            send(s[0], &cartasJog1, sizeof(cartasJog1), 0);
            send(s[1], &cartasJog2, sizeof(cartasJog2), 0);

            send(s[0], &atributos[inicio1->id].nome, sizeof(atributos[inicio1->id].nome), 0);
            send(s[1], &atributos[inicio2->id].nome, sizeof(atributos[inicio2->id].nome), 0);

            send(s[0], &atributos[inicio1->id].proteina, sizeof(atributos[inicio1->id].proteina), 0);
            send(s[1], &atributos[inicio2->id].proteina, sizeof(atributos[inicio2->id].proteina), 0);

            send(s[0], &atributos[inicio1->id].carboidrato, sizeof(atributos[inicio1->id].carboidrato), 0);
            send(s[1], &atributos[inicio2->id].carboidrato, sizeof(atributos[inicio2->id].carboidrato), 0);

            send(s[0], &atributos[inicio1->id].valorEnergetico, sizeof(atributos[inicio1->id].valorEnergetico), 0);
            send(s[1], &atributos[inicio2->id].valorEnergetico, sizeof(atributos[inicio2->id].valorEnergetico), 0);
          
            //recebe o op
            recv(s[0], &op, sizeof(op), 0);
            comparaAtributo(inicia, op);
            if(ganhou == 1){
                inicia = 0;
                printf("Jogador 1 joga agora...\n");
                printf("| Cartas jogador 1: %d vs Cartas Jogador 2: %d |\n", cartasJog1, cartasJog2);
            }
            if(ganhou == 2){
                inicia = 1;
                printf("Jogador 2 joga agora...\n");
                printf("| Cartas jogador 1: %d vs Cartas Jogador 2: %d |\n", cartasJog1, cartasJog2);
                
            }
        }
        if(inicia == 1){
            printf("JOGADOR 2 COMEÇA JOGANDO\n");
            mostraCarta(&inicio2);
            
            send(s[0], &espera, sizeof(espera), 0);
            send(s[1], &joga, sizeof(joga), 0);

            send(s[0], &vencedor, sizeof(vencedor), 0);
            send(s[1], &vencedor, sizeof(vencedor), 0);

            send(s[0], &fim_jogo, sizeof(fim_jogo), 0);
            send(s[1], &fim_jogo, sizeof(fim_jogo), 0);

            send(s[0], &cartasJog1, sizeof(cartasJog1), 0);
            send(s[1], &cartasJog2, sizeof(cartasJog2), 0);

            send(s[0], &atributos[inicio1->id].nome, sizeof(atributos[inicio1->id].nome), 0);
            send(s[1], &atributos[inicio2->id].nome, sizeof(atributos[inicio2->id].nome), 0);

            send(s[0], &atributos[inicio1->id].proteina, sizeof(atributos[inicio1->id].proteina), 0);
            send(s[1], &atributos[inicio2->id].proteina, sizeof(atributos[inicio2->id].proteina), 0);

            send(s[0], &atributos[inicio1->id].carboidrato, sizeof(atributos[inicio1->id].carboidrato), 0);
            send(s[1], &atributos[inicio2->id].carboidrato, sizeof(atributos[inicio2->id].carboidrato), 0);

            send(s[0], &atributos[inicio1->id].valorEnergetico, sizeof(atributos[inicio1->id].valorEnergetico), 0);
            send(s[1], &atributos[inicio2->id].valorEnergetico, sizeof(atributos[inicio2->id].valorEnergetico), 0);

            //recebe o op
            recv(s[1], &op, sizeof(op), 0);
            comparaAtributo(inicia, op);
            if(ganhou == 1){
                inicia = 0;
                printf("Jogador 1 joga agora...\n");
                printf("| Cartas jogador 1: %d vs Cartas Jogador 2: %d |\n", cartasJog1, cartasJog2);
            }
            if(ganhou == 2){
                inicia = 1;
                printf("Jogador 2 joga agora...\n");
                printf("| Cartas jogador 1: %d vs Cartas Jogador 2: %d |\n", cartasJog1, cartasJog2);
            }
        }
    }while(cartasJog1 != 0 && cartasJog2 != 0);

    if(cartasJog1 == 0 || cartasJog2 == 0){
        fim_jogo = 1;
        if(cartasJog1 == 0){
            vencedor = 2;
        }else if(cartasJog2 == 0){
            vencedor = 1;
        }
    }else{
        fim_jogo = 0;
    }

    //vez
    send(s[0], &espera, sizeof(espera), 0);
    send(s[1], &joga, sizeof(joga), 0);

    //vencedor
    send(s[0], &vencedor, sizeof(vencedor), 0);
    send(s[1], &vencedor, sizeof(vencedor), 0);

    //fim do jogo
    send(s[0], &fim_jogo, sizeof(fim_jogo), 0);
    send(s[1], &fim_jogo, sizeof(fim_jogo), 0);

    //cartas do jogador
    send(s[0], &cartasJog1, sizeof(cartasJog1), 0);
    send(s[1], &cartasJog2, sizeof(cartasJog2), 0);
}


int main(){    
    conecta();
    leArquivo();
    game();

    close(s[0]);
    close(s[1]);
}   