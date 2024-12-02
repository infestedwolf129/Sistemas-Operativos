* Nombre: José Manzano Rol:202173581-K Paralelo:200
* Nombre: Thomas Rodriguez Rol:202173593-3 Paralelo: 200

*VERSION DE JAVA USADA: openjdk "21.0.3" LTS 

*El archivo principal se llama Laberinto.java el cual debe estar en el mismo directorio que el archivo Makefile

*Se incluye archivo Makefile para el compilado del programa, que como ya se menciono debe estar en el mismo directorio que Laboratorio.java

*En la terminal donde esta el makefile se escribe "make compile" para crear el archivo "Laberinto", después se tiene que escribir "make run" para ejecutarlo 
y al final "make clean" para borrar el archivo que se creo

*Debe existir un archivo txt llamado laberinto.txt el cual debe estar en el mismo directorio que Laberinto.java

*Se asume que el laberinto siempre estara rodeado en la capa mas externa de 0 ,es decir, paredes en caso contrario el programa tomara como out of index.

*Adicionalmente dentro del codigo existe la posibilidad de solucionar el Laberinto mediante Threads y ForkJoin, por lo que en la linea 278 se encuentra el siguiente codigo:
	boolean usarThreads = true;
 en este apartado se elige que metodo se usara para resolver el laberinto, por lo que si se resuelve usando Hebras, dejar true y hacer el make,
 en caso contrario, si se desea usar el metodo de ForkJoin debe cambiar este parametro a false y hacer el make.


