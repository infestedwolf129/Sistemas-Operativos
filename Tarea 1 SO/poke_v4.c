#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

//Funcion que copia un archivo de una ruta a otra y recibe como parametros la ruta de origen y la ruta de destino
void copiarArchivo(const char *rutaOrigen, const char *rutaDestino) {
    FILE *archivoOriginal, *destinoDeArchivo;
    char buffer[BUFSIZ];
    size_t bytesRead;

    archivoOriginal = fopen(rutaOrigen, "rb");
    destinoDeArchivo = fopen(rutaDestino, "wb");
    // Copiar el archivo byte por byte
    while ((bytesRead = fread(buffer, 1, BUFSIZ, archivoOriginal)) > 0) {
        fwrite(buffer, 1, bytesRead, destinoDeArchivo);
    }

    fclose(archivoOriginal);
    fclose(destinoDeArchivo);
}

//Funcion que copia los archivos de la carpeta de origen a la carpeta de destino segun la primera letra del nombre del archivo
void copiaAlfabetica(const char *carpetaOrigen, const char *carpetaDestino) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(carpetaOrigen);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Solo procesar archivos regulares
            // Determinar la carpeta de destino según la primera letra del nombre del archivo
            char primeraLetra = entry->d_name[0];
            char *direccionDestino = malloc(strlen(carpetaDestino) + 2); // +2 para / y \0
            sprintf(direccionDestino, "%s/%c", carpetaDestino, primeraLetra);

            // Construir las rutas de origen y destino
            char *rutaOrigen = malloc(strlen(carpetaOrigen) + strlen(entry->d_name) + 2); // +2 para / y \0
            sprintf(rutaOrigen, "%s/%s", carpetaOrigen, entry->d_name);

            char *rutaDestino = malloc(strlen(direccionDestino) + strlen(entry->d_name) + 2); // +2 para / y \0
            sprintf(rutaDestino, "%s/%s", direccionDestino, entry->d_name);

            // Copiar el archivo al directorio de destino
            copiarArchivo(rutaOrigen, rutaDestino);

            // Liberar la memoria
            free(direccionDestino);
            free(rutaOrigen);
            free(rutaDestino);
        }
    }

    closedir(dir);
}

//Funcion que mueve los archivos de la carpeta de origen a la carpeta de destino segun la generacion a la que pertenecen
void copiaGeneracion(const char *carpetaOrigen, const char *carpetaDestino) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(carpetaOrigen);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Solo procesar archivos regulares
            // Determinar la carpeta de destino según el numero de la generacion luego del '_' en el nombre del archivo
            char *nombreArchivo = strdup(entry->d_name); // Duplicar el nombre del archivo para no modificar el original
            char *generacion = strtok(nombreArchivo, "_");
            generacion = strtok(NULL, "_"); // Ahora sí podemos obtener la generación
            // Si el numero va entre 1 - 151 es para 'I', 152 - 251 es para 'II', 252 - 386 es para 'III' y 387 - 493 es para 'IV'
            char *direccionDestino = malloc(strlen(carpetaDestino) + 2); // +2 para / y \0
            if (atoi(generacion) <= 151) {
                sprintf(direccionDestino, "%s/I", carpetaDestino);
            } else if (atoi(generacion) <= 251) {
                sprintf(direccionDestino, "%s/II", carpetaDestino);
            } else if (atoi(generacion) <= 386) {
                sprintf(direccionDestino, "%s/III", carpetaDestino);
            } else {
                sprintf(direccionDestino, "%s/IV", carpetaDestino);
            }

            // Construir las rutas de origen
            char *rutaOrigen = malloc(strlen(carpetaOrigen) + strlen(entry->d_name) + 2); // +2 para / y \0
            sprintf(rutaOrigen, "%s/%s", carpetaOrigen, entry->d_name);

            // Construir las rutas de destino
            char *rutaDestino = malloc(strlen(direccionDestino) + strlen(entry->d_name) + 2); // +2 para / y \0
            sprintf(rutaDestino, "%s/%s", direccionDestino, entry->d_name);

            // Mover el archivo al directorio de destino
            rename(rutaOrigen, rutaDestino);

            // Liberar la memoria
            free(direccionDestino);
            free(rutaOrigen);
            free(rutaDestino);
            free(nombreArchivo); 
        }
    }

    closedir(dir);

}

// Funcion que escribe en el archivo RegistroPokemon.txt 'Alfabetico' salto de linea y luego 'Letra - Cantidad de archivos' por cada subcarpeta
void escribirRegistro(const char *carpeta) {
    FILE *registro = fopen("RegistroPokemon.txt", "a");
    DIR *dir;
    struct dirent *entry;
    int cantidadArchivos = 0;

    dir = opendir(carpeta);
    
    // Contar la cantidad de archivos en la carpeta
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Solo procesar archivos regulares
            cantidadArchivos++;
        }
    }

    fprintf(registro, "%s- %d\n", carpeta, cantidadArchivos);

    fclose(registro);
    closedir(dir);
}



int main() {
    // Crear la carpeta "Alfabetico"
    mkdir("Alfabetico", 0777);

    // Crear las carpetas de la A a la Z
    for (char letra = 'A'; letra <= 'Z'; letra++) {
        // Crear la carpeta correspondiente
        char nombreCarpeta[20]; // Suficientemente grande para contener el nombre de la carpeta
        sprintf(nombreCarpeta, "Alfabetico/%c", letra);
        mkdir(nombreCarpeta, 0777);
    }

    // Crear la carpeta "Generación"
    mkdir("Generación", 0777);

    // Nombres de las generaciones
    char *generaciones[] = {"I", "II", "III", "IV"};

    // Crear las carpetas de "I" a "IV" dentro de "Generación"
    for (int i = 0; i < 4; i++) {
        // Crear la carpeta correspondiente
        char nombreCarpeta[20];
        sprintf(nombreCarpeta, "Generación/%s", generaciones[i]);
        mkdir(nombreCarpeta, 0777);
    }

    // Crear txt RegistroPokemon.txt
    FILE *registro = fopen("RegistroPokemon.txt", "w");
    fclose(registro);

    // Copiar los sprites a las carpetas alfabetizadas
    copiaAlfabetica("Sprites", "Alfabetico");

    // Copiar los sprites a las carpetas de generación
    copiaGeneracion("Sprites", "Generación");


    // Escribir en el archivo RegistroPokemon.txt 'Generacion:\n' y cerrar el archivo
    FILE *registroGeneracion = fopen("RegistroPokemon.txt", "a");
    fprintf(registroGeneracion, "Generación:\n");
    fclose(registroGeneracion);

    // Escribir en el archivo RegistroPokemon.txt
    for (int i = 0; i < 4; i++) {
        // Escribir en el archivo RegistroPokemon.txt
        char nombre_carpeta[20];
        sprintf(nombre_carpeta, "Generación/%s", generaciones[i]);
        escribirRegistro(nombre_carpeta);
    }

    // Escribir en el archivo RegistroPokemon.txt 'Alfabetico:\n' y cerrar el archivo
    FILE *registroGeneracion1 = fopen("RegistroPokemon.txt", "a");
    fprintf(registroGeneracion1, "Alfabetico:\n");
    fclose(registroGeneracion1);

    for (char letra = 'A'; letra <= 'Z'; letra++) {
        // Escribir en el archivo RegistroPokemon.txt
        char nombreCarpeta[20];
        sprintf(nombreCarpeta, "Alfabetico/%c", letra);
        escribirRegistro(nombreCarpeta);
    }


    

    printf("Carpetas creadas y archivos copiados con exito.\n");

    return 0;
}
