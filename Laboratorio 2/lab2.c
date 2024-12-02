#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

int randomInRange(int min, int max) {
    return min + rand() % (max - min + 1);
}

int objetivo (int *Num){
    int ran = randomInRange(1,4);
    if(ran == *Num){
        ran = objetivo(Num);
    }
    return ran;
}

void initChildRandom() {
    srand(time(NULL) ^ getpid()); // XOR con el PID del proceso
}

//Creamos funcion de punteros que inicializa en valores aleatorios dependiendo el tiempo el ataque, defensa y evasion
void creacionStats(int *ATCK, int *DEF, int *EVS){
    *ATCK = randomInRange(30,40);
    *DEF = randomInRange(10,25);
    *EVS = (60 - *DEF);
}

int main(){
    printf("Bienvenido a Wrestlemania! \nEres el Jugador 1!\n");
    int pipes[8][2]; //8 pipes en total, dos para cada par de jugadores

    //Iniciamos los pipes
    for ( int i = 0; i < 8; i++){
        pipe(pipes[i]);
    }

    /*
    ORDEN PIPES:

    //Comunicacion entre padre y jugador 1
    pipes[0] padre a Jugador 1
    pipes[1] Jugador 1 a padre

    //Comunicacion entre padre y jugador 2
    pipes[2] padre a Jugador 2
    pipes[3] Jugador 2 a padre

    //Comunicacion entre padre y jugador 3
    pipes [4] padre a Jugador 3
    pipes [5] Jugador 3 a padre

    //Comunicacion entre padre y jugador 4
    pipes [6] padre a Jugador 4
    pipes [7] Jugador 4 a padre
    */
    
    

    //Iniciamos variables comunes para los 4 jugadores
    int Num;
    int Vida = 100;
    int Ataque = 0;
    int Defensa = 0;
    int Evasion = 0;

    //Crear el primer hijo
    pid_t Jugador1 = fork();
    if (Jugador1 == 0) {
        //Cerramos canales de comunicacion que no se usan y no pertenecen a este hijo
        close(pipes[2][0]);
        close(pipes[2][1]);
        close(pipes[3][0]);
        close(pipes[3][1]);
        close(pipes[4][0]);
        close(pipes[4][1]);
        close(pipes[5][0]);
        close(pipes[5][1]);
        close(pipes[6][0]);
        close(pipes[6][1]);
        close(pipes[7][0]);
        close(pipes[7][1]);

        //Seteamos estadisticas
        Num = 1;
        initChildRandom();
        creacionStats(&Ataque,&Defensa,&Evasion);
        printf("\nJugador %d\nVida: %d\nAtaque: %d\nDefensa: %d\nEvasion: %d\n\n",Num,Vida,Ataque,Defensa,Evasion);
        
        //Enviamos las estadisticas de ataque y defensa al padre
        write(pipes[1][1],&Ataque,sizeof(Ataque));
        write(pipes[1][1],&Defensa,sizeof(Defensa));
        write(pipes[1][1],&Evasion,sizeof(Evasion));

        //Acciones del jugador 1
        if(Vida > 0){
            //Enviamos las estadisticas al padre
            write(pipes[1][1],&Vida,sizeof(Vida));
            
        }
        exit(0);
        
        
    } else {
        //Crear el segundo hijo
        pid_t Jugador2 = fork();
        if (Jugador2 == 0) {
            //Esperamos a que el jugador 1 termine de inicializar
            //Cerramos canales de comunicacion que no se usan y no pertenecen a este hijo
            close(pipes[0][0]);
            close(pipes[0][1]);
            close(pipes[1][0]);
            close(pipes[1][1]);
            close(pipes[4][0]);
            close(pipes[4][1]);
            close(pipes[5][0]);
            close(pipes[5][1]);
            close(pipes[6][0]);
            close(pipes[6][1]);
            close(pipes[7][0]);
            close(pipes[7][1]);
            
            //Seteamos estadisticas
            Num = 2;
            initChildRandom();
            creacionStats(&Ataque,&Defensa,&Evasion);
            printf("\nJugador %d\nVida: %d\nAtaque: %d\nDefensa: %d\nEvasion: %d\n\n",Num,Vida,Ataque,Defensa,Evasion);
            
            //Enviamos las estadisticas de ataque y defensa al padre
            write(pipes[3][1],&Ataque,sizeof(Ataque));
            write(pipes[3][1],&Defensa,sizeof(Defensa));
            write(pipes[3][1],&Evasion,sizeof(Evasion));
            write(pipes[3][1],&Vida,sizeof(Vida));

            while (Vida > 0){
                //Acciones del jugador 2
                int toAtck = objetivo(&Num);
                //Enviamos el objetivo al padre
                write(pipes[3][1],&toAtck,sizeof(toAtck));
            }
            exit(0);

        } else {
            
            //Crear el tercer hijo
            pid_t Jugador3 = fork();
            if (Jugador3 == 0) {
                //Cerramos canales de comunicacion que no se usan y no pertenecen
                close(pipes[0][0]);
                close(pipes[0][1]);
                close(pipes[1][0]);
                close(pipes[1][1]);
                close(pipes[2][0]);
                close(pipes[2][1]);
                close(pipes[3][0]);
                close(pipes[3][1]);
                close(pipes[6][0]);
                close(pipes[6][1]);
                close(pipes[7][0]);
                close(pipes[7][1]);

                //Seteamos estadisticas
                Num = 3;
                initChildRandom();
                creacionStats(&Ataque,&Defensa,&Evasion);
                printf("\nJugador %d\nVida: %d\nAtaque: %d\nDefensa: %d\nEvasion: %d\n\n",Num,Vida,Ataque,Defensa,Evasion);
                
                //Enviamos las estadisticas de ataque y defensa al padre
                write(pipes[5][1],&Ataque,sizeof(Ataque));
                write(pipes[5][1],&Defensa,sizeof(Defensa));
                write(pipes[5][1],&Evasion,sizeof(Evasion));
                write(pipes[5][1],&Vida,sizeof(Vida));
                
                while(Vida > 0){
                    //Acciones del jugador 3
                    int toAtck = objetivo(&Num);
                    //Enviamos el objetivo al padre
                    write(pipes[5][1],&toAtck,sizeof(toAtck));

                    

                }
                exit(0);
            } else {
                //Crear el cuarto hijo
                pid_t Jugador4 = fork();
                if (Jugador4 == 0) {
                    //Cerramos canales de comunicacion que no se usan y no pertenecen
                    close(pipes[0][0]);
                    close(pipes[0][1]);
                    close(pipes[1][0]);
                    close(pipes[1][1]);
                    close(pipes[2][0]);
                    close(pipes[2][1]);
                    close(pipes[3][0]);
                    close(pipes[3][1]);
                    close(pipes[4][0]);
                    close(pipes[4][1]);
                    close(pipes[5][0]);
                    close(pipes[5][1]);
                    

                    //Seteamos estadisticas
                    Num = 4;
                    initChildRandom();
                    creacionStats(&Ataque,&Defensa,&Evasion);
                    printf("\nJugador %d\nVida: %d\nAtaque: %d\nDefensa: %d\nEvasion: %d\n\n",Num,Vida,Ataque,Defensa,Evasion);

                    //Enviamos las estadisticas de ataque y defensa al padre
                    write(pipes[7][1],&Ataque,sizeof(Ataque));
                    write(pipes[7][1],&Defensa,sizeof(Defensa));
                    write(pipes[7][1],&Evasion,sizeof(Evasion));
                    write(pipes[7][1],&Vida,sizeof(Vida));

                    //Acciones del jugador 4
                    while (Vida > 0){
                        
                        int toAtck = objetivo(&Num);
                        //Enviamos el objetivo al padre
                        write(pipes[7][1],&toAtck,sizeof(toAtck));
                       
                    }

                    exit(0);
                } else {
                    //Codigo del padre
                    //Cerramos canales de comunicacion que no se usan y no pertenecen
                    close(pipes[0][0]);
                    close(pipes[1][1]);
                    close(pipes[2][0]);
                    close(pipes[3][1]);
                    close(pipes[4][0]);
                    close(pipes[5][1]);
                    close(pipes[6][0]);
                    close(pipes[7][1]);

                    //Iniciamos variables para los 4 jugadores
                    int Ataques[4];
                    int Defensas[4];
                    int Evasiones[4];
                    int Vidas[4];
                    int Objetivos[4];
                    int Danios[4];
                    int ronda = 1;
                    int input, reroll;
                    bool flag = true;
                    

                    for(int i = 0; i < 4; i++){
                        //Recolectamos el valor de Ataque de cada jugador
                        read(pipes[i*2+1][0],&Ataques[i],sizeof(Ataques[i]));

                        //Recolectamos el valor de Defensa de cada jugador
                        read(pipes[i*2+1][0],&Defensas[i],sizeof(Defensas[i]));

                        //Recolectamos el valor de Evasion de cada jugador
                        read(pipes[i*2+1][0],&Evasiones[i],sizeof(Evasiones[i]));

                        //Recolectamos las vidas actuales de los jugadores
                        read(pipes[i*2+1][0],&Vidas[i],sizeof(Vidas[i]));
                    }

                    
                    while(flag != false){             //LOOP
                        printf("\n\nRonda %d\n\n",ronda);
                        
                        for (int i = 0; i < 4; i++) {
                            //Mostramos la vida de todos los jugadores
                            printf("\nJugador %d Vida: %d/100\n",i+1,Vidas[i]);
                        }

                        for(int i = 0; i < 4; i++){
                            //Comienzan los turnos
                            if(Vidas[i] > 0){               
                                if( i == 0){
                                    printf("\nAtacar a:\n 1.Jugador 2\n 2.Jugador 3\n 3.Jugador 4\n ");
                                    scanf("%d",&input);
                                    Objetivos[0] = input + 1;

                                    //comprobamos que sea un jugador valido y que no se ataque a si mismo
                                    while(Objetivos[0] < 2 || Objetivos[0] > 4 || Vidas[Objetivos[0]-1] <= 0){
                                        //comprobar si esta vivo el jugador
                                        if(Vidas[Objetivos[0]-1] <= 0){
                                            printf("Jugador %d esta muerto, intenta de nuevo: ",Objetivos[0]);
                                            scanf("%d",&input);
                                            Objetivos[0] = input + 1;
                                        }else{
                                            printf("Jugador invalido, intenta de nuevo: ");
                                            scanf("%d",&input);
                                            Objetivos[0] = input + 1;
                                        }
                                    }

                                }else{
                                    //Recibimos el objetivo de los jugadores
                                    read(pipes[i*2+1][0],&Objetivos[i],sizeof(Objetivos[i]));

                                    //Comprobamos que el jugador este vivo
                                    while(Vidas[Objetivos[i]-1] <= 0){
                                        reroll = i+1;
                                        Objetivos[i] = objetivo(&reroll);
                                    }
                                }


                                //Calculamos Danios
                                Danios[i] = Ataques[i] - Defensas[Objetivos[i]-1];

                            }else{
                                printf("Jugador %d esta muerto\n",i+1);
                            }
                        }

                        printf("\nAcciones\n");
                        for (int i = 0; i < 4; i++) {
                            if(Vidas[i] > 0){
                                //Mostramos los targets de los jugadores
                                printf("\nJugador %d ataca a Jugador %d\n",i+1,Objetivos[i]);
                                //Comprobamos si el jugador ha sido atacado
                                if(Vidas[Objetivos[i]-1] > 0){
                                    if (randomInRange(0,100) < Evasiones[Objetivos[i]-1]){
                                        printf("Jugador %d ha evadido el ataque\n",Objetivos[i]);
                                    }else{
                                        if((Vidas[Objetivos[i]-1] - Danios[i]) <= 0){
                                            Vidas[Objetivos[i]-1] = 0;
                                        }else{
                                            Vidas[Objetivos[i]-1] -= Danios[i];
                                        }
                                        printf("Jugador %d ha recibido %d de daño\n",Objetivos[i],Danios[i]);
                                    }
                                } else {
                                    printf("Pero el jugador %d ha muerto antes del ataque\n",Objetivos[i]);
                                }
                            }
                        }


                        int recuento = 0;
                        //Enviamos las vidas actualizadas y contamos los jugadores vivos
                        for (int i = 0; i < 4; i++) {
                            write(pipes[i*2+1][1],&Vidas[i],sizeof(Vidas[i]));
                            if(Vidas[i] > 0){
                                recuento++;
                            }
                        }
                        
                        

                        //Finalizamos el juego en caso de quedar un vivo
                        if(recuento == 1){
                            flag = false;
                            for(int i = 0; i < 4; i++){
                                if(Vidas[i] > 0){
                                    printf("\nEl juego ha terminado, el ganador del WrestleMania es el Jugador %d\n\n",i+1);
                                }
                            }
                            break;
                        }
                        ronda++;
                        
                    }
                    //Eliminamos los procesos hijos
                    kill(Jugador1, SIGTERM);
                    kill(Jugador2, SIGTERM);
                    kill(Jugador3, SIGTERM);
                    kill(Jugador4, SIGTERM);
                    printf("\nProcesos hijo finalizados\n");
                }
            }
        }
    }
    return 0;
}