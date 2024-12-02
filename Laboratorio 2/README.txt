* Nombre: José Manzano Rol:202173581-K Paralelo:200
* Nombre: Thomas Rodriguez Rol:202173593-3 Paralelo: 200

*El archivo principal se llama lab2.c el cual debe estar en el mismo directorio que el archivo Makefile

*Se incluye archivo Makefile para el compilado del programa, que como ya se menciono debe estar en el mismo directorio que lab2.c

*en la terminal donde esta el makefile se escribe "make" para crear el archivo "lab2", después se tiene que escribir ./lab2 para ejecutarlo 
y al final "make clean" para borrar el archivo que se creo

*A continuación para un mejor entendimiento de la tarea se entrega el orden de los pipes que usamos para la comunicacion entre los procesos hijos y padre

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

*Finalmente al ejecutarse el archivo lab2, para atacar debera ingresar por consola 1 2 o 3 para atacar a los jugadores 2,3 y 4 respectivamente
