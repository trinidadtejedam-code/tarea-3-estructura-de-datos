#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include <string.h>

// Definición de la estructura para el estado del puzzle
typedef struct {
    char nombre[100];
    int valor;
    int peso;
} Item;


typedef struct {
    int id;
    char nombre[100];
    char descripcion[500];
    List* items;
    int arriba;
    int abajo;
    int izquierda;
    int derecha;
    int esFinal;
} Escenario;

typedef struct
{
    int escenarioActual;
    List* inventario;
    int pesoTotal;
    int puntaje;
    int tiempoRestante;

} Jugador;

typedef struct
{
    Escenario* escenarios[17];
} Grafo;

void recogerItems(Grafo* grafo, Jugador* jugador);
void avanzar(Grafo* grafo, Jugador* jugador);
void descartarItems(Jugador* jugador);

int separarCSV(char* linea, char campos[][1024]) {
    int columna = 0;
    int posicion = 0;
    int comillas = 0;

    for (int i = 0; linea[i] != '\0'; i++) {
        char c = linea[i];

        if (c == '"') {
            if (comillas && linea[i + 1] == '"') {
                if (posicion < 1023)
                    campos[columna][posicion++] = '"';
                i++;
            } else {
                comillas = !comillas;
            }
        }
        else if (c == ',' && !comillas) {
            campos[columna][posicion] = '\0';
            columna++;
            posicion = 0;

            if (columna >= 9)
                return 0;
        }
        else if (c != '\n' && c != '\r') {
            if (posicion < 1023)
                campos[columna][posicion++] = c;
        }
    }

    campos[columna][posicion] = '\0';
    return columna + 1;
}

void cargarItems(Escenario* escenario, char* texto) {
    escenario->items = list_create();

    if (texto[0] == '\0')
        return;

    char copia[1024];
    snprintf(copia, sizeof(copia), "%s", texto);

    char* itemTexto = strtok(copia, ";");

    while (itemTexto != NULL) {
        Item* item = malloc(sizeof(Item));

        if (item == NULL)
            break;

        if (sscanf(itemTexto, " %99[^,],%d,%d",
                   item->nombre, &item->valor, &item->peso) == 3) {
            list_pushBack(escenario->items, item);
        } else {
            free(item);
        }

        itemTexto = strtok(NULL, ";");
    }
}


void liberarGrafo(Grafo* grafo) {
    for (int i = 1; i <= 16; i++) {
        Escenario* escenario = grafo->escenarios[i];

        if (escenario == NULL)
            continue;

        Item* item = list_first(escenario->items);

        while (item != NULL) {
            list_popCurrent(escenario->items);
            free(item);
            item = list_first(escenario->items);
        }

        free(escenario->items);
        free(escenario);
        grafo->escenarios[i] = NULL;
    }
}


void cargarLaberinto(Grafo* grafo) {
    FILE* archivo = fopen("graphquest.csv", "r");

    if (archivo == NULL) {
        printf("Error al abrir el archivo CSV\n");
        return;
    }

    char linea[2048];
    char campos[9][1024];
    int cantidad = 0;

    // inicializar el grafo
    liberarGrafo(grafo);

    // saltar encabezados
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        printf("El archivo esta vacio\n");
        fclose(archivo);
        return;
    }

    // leer escenarios
    while (fgets(linea, sizeof(linea), archivo) != NULL) {

        if (separarCSV(linea, campos) != 9)
            continue;

        int id = atoi(campos[0]);

        if (id < 1 || id > 16)
            continue;

        Escenario* escenario = malloc(sizeof(Escenario));

        if (escenario == NULL)
            break;

        escenario->id = id;

        snprintf(escenario->nombre,
                 sizeof(escenario->nombre), "%s", campos[1]);

        snprintf(escenario->descripcion,
                 sizeof(escenario->descripcion), "%s", campos[2]);

        cargarItems(escenario, campos[3]);

        escenario->arriba = atoi(campos[4]);
        escenario->abajo = atoi(campos[5]);
        escenario->izquierda = atoi(campos[6]);
        escenario->derecha = atoi(campos[7]);

        escenario->esFinal = strcmp(campos[8], "Si") == 0;

        grafo->escenarios[id] = escenario;
        cantidad++;
    }

    fclose(archivo);

    printf("\nLaberinto cargado correctamente\n");
    printf("Escenarios cargados: %d\n", cantidad);
}


void mostrarEstado(Grafo* grafo, Jugador* jugador) {
    Escenario* escenario = grafo->escenarios[jugador->escenarioActual];

    printf("\n===== ESTADO ACTUAL =====\n");
    printf("Escenario: %s\n", escenario->nombre);
    printf("Descripcion: %s\n", escenario->descripcion);

    printf("\nItems disponibles:\n");
    Item* item = list_first(escenario->items);

    if (item == NULL)
        printf("No hay items disponibles\n");

    while (item != NULL) {
        printf("- %s (%d pts, %d kg)\n",
               item->nombre, item->valor, item->peso);
        item = list_next(escenario->items);
    }

    printf("\nTiempo restante: %d\n", jugador->tiempoRestante);
    printf("Puntaje: %d\n", jugador->puntaje);
    printf("Peso total: %d kg\n", jugador->pesoTotal);

    printf("\nInventario:\n");
    item = list_first(jugador->inventario);

    if (item == NULL)
        printf("Inventario vacio\n");

    while (item != NULL) {
        printf("- %s\n", item->nombre);
        item = list_next(jugador->inventario);
    }

    printf("\nDirecciones disponibles:\n");

    if (escenario->arriba != -1)
        printf("1. Arriba\n");
    if (escenario->abajo != -1)
        printf("2. Abajo\n");
    if (escenario->izquierda != -1)
        printf("3. Izquierda\n");
    if (escenario->derecha != -1)
        printf("4. Derecha\n");
}


void limpiarInventario(Jugador* jugador) {
    Item* item = list_first(jugador->inventario);

    while (item != NULL) {
        list_popCurrent(jugador->inventario);
        free(item);
        item = list_first(jugador->inventario);
    }
}


void iniciarPartida(Grafo* grafo) {
    
    if (grafo->escenarios[1] == NULL) {
        printf("Primero debes cargar el laberinto\n");
        return;
    }

    cargarLaberinto(grafo);
    if (grafo->escenarios[1] == NULL) {
        printf("No se pudo cargar el laberinto\n");
        return;
    }
    
    Jugador jugador;
    jugador.escenarioActual = 1;
    jugador.inventario = list_create();
    jugador.pesoTotal = 0;
    jugador.puntaje = 0;
    jugador.tiempoRestante = 10;

    int opcion = 0;

    do {
        if (jugador.tiempoRestante <= 0) {
            printf("\nSe acabo el tiempo. Perdiste!\n");
            break;
        }

        mostrarEstado(grafo, &jugador);

        printf("\n===== MENU DEL JUEGO =====\n");
        printf("1. Recoger items\n");
        printf("2. Descartar items\n");
        printf("3. Avanzar en una direccion\n");
        printf("4. Reiniciar partida\n");
        printf("5. Salir del juego\n");

        printf("Ingrese una opcion: ");
        if (scanf("%d", &opcion) != 1) {
            printf("Entrada no valida\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        switch (opcion) {
            case 1:
                recogerItems(grafo, &jugador);
                break;

            case 2:
                descartarItems(&jugador);
                break;

            case 3:
                avanzar(grafo, &jugador);

                if (grafo->escenarios[jugador.escenarioActual]->esFinal) {
                    limpiarInventario(&jugador);
                    free(jugador.inventario); 
                    return;
                }

                break;

            case 4:
                limpiarInventario(&jugador);
                cargarLaberinto(grafo);

                jugador.escenarioActual = 1;
                jugador.pesoTotal = 0;
                jugador.puntaje = 0;
                jugador.tiempoRestante = 10;

                printf("\nPartida reiniciada correctamente\n");
                break;

            case 5:
                printf("Saliendo del juego...\n");
                limpiarInventario(&jugador);
                free(jugador.inventario);
                liberarGrafo(grafo);
                exit(0);


            default:
                printf("Opcion no valida\n");
        }

    } while (opcion != 5);
    limpiarInventario(&jugador); 
    free(jugador.inventario);  
    
}



void recogerItems(Grafo* grafo, Jugador* jugador) {
    Escenario* escenario = grafo->escenarios[jugador->escenarioActual];

    if (list_first(escenario->items) == NULL) {
        printf("\nNo hay items disponibles para recoger.\n");
        return;
    }

    int recogidos = 0;
    int opcion;

    while (1) {
        Item* item = list_first(escenario->items);
        int numero = 1;

        if (item == NULL)
            break;

        printf("\n===== RECOGER ITEMS =====\n");

        while (item != NULL) {
            printf("%d. %s (%d pts, %d kg)\n",
                   numero, item->nombre, item->valor, item->peso);
            item = list_next(escenario->items);
            numero++;
        }

        printf("0. Terminar seleccion\n");
        printf("Seleccione un item: ");

        if (scanf("%d", &opcion) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Entrada no valida\n");
            break;
        }

        if (opcion == 0)
            break;

        if (opcion < 1 || opcion >= numero) {
            printf("Opcion no valida\n");
            continue;
        }

        item = list_first(escenario->items);

        for (int i = 1; i < opcion; i++) {
            item = list_next(escenario->items);
        }

        list_pushBack(jugador->inventario, item);

        jugador->pesoTotal += item->peso;
        jugador->puntaje += item->valor;

        list_popCurrent(escenario->items);

        printf("Recogiste: %s\n", item->nombre);
        recogidos++;
    }

    if (recogidos > 0) {
        jugador->tiempoRestante--;

        printf("\nItems recogidos: %d\n", recogidos);
        printf("Puntaje: %d\n", jugador->puntaje);
        printf("Peso: %d kg\n", jugador->pesoTotal);
        printf("Tiempo restante: %d\n", jugador->tiempoRestante);
    }
}



void avanzar(Grafo* grafo, Jugador* jugador) {
    Escenario* escenario = grafo->escenarios[jugador->escenarioActual];

    int direccion;
    int destino = -1;

    printf("\n===== AVANZAR =====\n");

    if (escenario->arriba != -1)
        printf("1. Arriba\n");

    if (escenario->abajo != -1)
        printf("2. Abajo\n");

    if (escenario->izquierda != -1)
        printf("3. Izquierda\n");

    if (escenario->derecha != -1)
        printf("4. Derecha\n");

    printf("Seleccione una direccion: ");

    if (scanf("%d", &direccion) != 1) {
        printf("Entrada no valida\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }

    switch (direccion) {
        case 1:
            destino = escenario->arriba;
            break;

        case 2:
            destino = escenario->abajo;
            break;

        case 3:
            destino = escenario->izquierda;
            break;

        case 4:
            destino = escenario->derecha;
            break;

        default:
            printf("Direccion no valida\n");
            return;
    }

    if (destino == -1 || destino < 1 || destino > 16 ||
        grafo->escenarios[destino] == NULL) {
        printf("No puedes avanzar en esa direccion\n");
        return;
    }

    // Calcular el tiempo segun el peso del inventario
    int tiempo = (jugador->pesoTotal + 10) / 10;

    if (tiempo > jugador->tiempoRestante) {
        printf("No tienes tiempo suficiente para avanzar\n");
        return;
    }

    jugador->tiempoRestante -= tiempo;
    jugador->escenarioActual = destino;

    printf("\nAvanzaste a: %s\n",
           grafo->escenarios[destino]->nombre);

    printf("Tiempo utilizado: %d\n", tiempo);
    printf("Tiempo restante: %d\n", jugador->tiempoRestante);

    // Comprobar si llego a la salida
    if (grafo->escenarios[destino]->esFinal) {
        printf("\n===== FELICIDADES =====\n");
        printf("Llegaste a la salida!\n");
        printf("Puntaje final: %d\n", jugador->puntaje);
        printf("Peso total: %d kg\n", jugador->pesoTotal);

        printf("\nItems conseguidos:\n");

        Item* item = list_first(jugador->inventario);

        if (item == NULL)
            printf("Inventario vacio\n");

        while (item != NULL) {
            printf("- %s (%d pts)\n",
                   item->nombre, item->valor);
            item = list_next(jugador->inventario);
        }
    }
}


void descartarItems(Jugador* jugador) {
    Item* item = list_first(jugador->inventario);

    if (item == NULL) {
        printf("\nTu inventario esta vacio.\n");
        return;
    }

    printf("\n===== DESCARTAR ITEMS =====\n");

    int numero = 1;

    while (item != NULL) {
        printf("%d. %s (%d pts, %d kg)\n",
               numero, item->nombre, item->valor, item->peso);
        item = list_next(jugador->inventario);
        numero++;
    }

    printf("0. Cancelar\n");
    printf("Seleccione un item: ");

    int opcion;

    if (scanf("%d", &opcion) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Entrada no valida\n");
        return;
    }

    if (opcion == 0)
        return;

    if (opcion < 1 || opcion >= numero) {
        printf("Opcion no valida\n");
        return;
    }

    item = list_first(jugador->inventario);

    for (int i = 1; i < opcion; i++) {
        item = list_next(jugador->inventario);
    }

    jugador->pesoTotal -= item->peso;
    jugador->puntaje -= item->valor;
    jugador->tiempoRestante--;

    printf("\nItem descartado: %s\n", item->nombre);

    list_popCurrent(jugador->inventario);
    free(item);

    printf("Puntaje actual: %d\n", jugador->puntaje);
    printf("Peso total: %d kg\n", jugador->pesoTotal);
    printf("Tiempo restante: %d\n", jugador->tiempoRestante);
}


int main() {

    int opcion = 0;
    Grafo grafo = {0};
    do {
        printf("\n===== GRAPHQUEST =====\n");
        printf("1. Cargar laberinto desde CSV\n");
        printf("2. Iniciar partida\n");
        printf("3. Salir\n");

        printf("Ingrese una opcion: ");
        if (scanf("%d", &opcion) != 1) {
            printf("Entrada no valida\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        switch (opcion) {
            case 1:
                cargarLaberinto(&grafo);
                break;

            case 2:
                iniciarPartida(&grafo);
                break;

            case 3:
                printf("Saliendo del juego...\n");
                break;

            default:
                printf("Opcion no valida\n");
        }

    } while (opcion != 3);

    liberarGrafo(&grafo);

    return 0;

}
