# GraphQuest - Tarea 3

## Descripción del programa

GraphQuest es un juego desarrollado en lenguaje C que consiste en recorrer un laberinto formado por 16 escenarios conectados entre sí.

El objetivo del juego es llegar a la salida antes de que se termine el tiempo, intentando conseguir el mayor puntaje posible mediante la recolección de ítems que se encuentran en diferentes escenarios.

Cada ítem tiene un valor y un peso, por lo que el jugador debe decidir cuáles recoger, ya que el peso acumulado influye en el tiempo que demora en avanzar.

## Funcionalidades implementadas

**1. Cargar laberinto desde CSV**

El programa permite cargar los 16 escenarios desde el archivo `graphquest.csv`. Para esto se utiliza la función `cargarLaberinto`, que lee la información de cada escenario, incluyendo su nombre, descripción, ítems y conexiones con otros escenarios.

También se utilizan las funciones `separarCSV` y `cargarItems` para separar los datos y almacenar los ítems correspondientes.

**2. Iniciar partida**

La función `iniciarPartida` permite comenzar el juego desde la entrada principal. El jugador comienza con 10 unidades de tiempo, sin ítems en el inventario y con un puntaje de 0.

**3. Mostrar estado actual**

La función `mostrarEstado` muestra la información del escenario en el que se encuentra el jugador, incluyendo los ítems disponibles, el tiempo restante, el puntaje, el peso total y las direcciones por las que puede avanzar.

**4. Recoger ítems**

La función `recogerItems` permite seleccionar uno o varios ítems disponibles en el escenario y agregarlos al inventario.

Al recogerlos, aumenta el puntaje y el peso total del jugador. Esta acción descuenta una unidad de tiempo.

**5. Descartar ítems**

El jugador puede seleccionar un ítem de su inventario para descartarlo. Al hacerlo, disminuyen el peso total y el puntaje correspondiente al ítem descartado.

Esta acción también descuenta una unidad de tiempo.

**6. Avanzar entre escenarios**

El jugador puede elegir entre las direcciones disponibles: arriba, abajo, izquierda y derecha.

Al avanzar, se actualiza el escenario actual y se descuenta el tiempo correspondiente según el peso que lleva el jugador.

Para calcular el tiempo de movimiento se utiliza la fórmula:

`T = (pesoTotal + 10) / 10`

Esta operación permite obtener el tiempo de movimiento utilizando división entera.

**7. Reiniciar partida**

El programa permite reiniciar la partida, volviendo a la entrada principal, con el inventario vacío, puntaje 0, peso 0 y tiempo inicial de 10.

**8. Finalizar partida**

Cuando el jugador llega al escenario de salida, el programa muestra un mensaje de felicitaciones junto con el puntaje final, el peso total y los ítems que consiguió.

Si el tiempo llega a 0 antes de alcanzar la salida, se muestra un mensaje indicando que perdió la partida.

Además, el jugador puede seleccionar la opción 5 del menú del juego para salir y cerrar completamente el programa.

## Estructuras de datos utilizadas

Para desarrollar el programa utilicé las siguientes estructuras:

- `Item`: almacena el nombre, valor y peso de cada ítem.
- `Escenario`: almacena la información de cada escenario, sus ítems y las conexiones disponibles.
- `Jugador`: guarda la posición actual, inventario, peso, puntaje y tiempo restante.
- `Grafo`: permite almacenar los escenarios del laberinto.

También utilicé el TDA `List` para manejar los ítems disponibles en los escenarios y el inventario del jugador.

## Cómo ejecutar el programa

Para compilar el programa desde la Shell de Replit se utiliza el siguiente comando:

```bash
gcc -w tarea3.c tdas/*.c -o tarea3
```

Luego, para ejecutarlo:

```bash
./tarea3
```

Es necesario que el archivo `graphquest.csv` se encuentre en el proyecto para cargar correctamente los escenarios.

## Ejemplo de uso

1. Al ejecutar el programa aparece el menú principal.
2. Selecciono la opción 1 para cargar el laberinto desde el archivo CSV.
3. Selecciono la opción 2 para comenzar la partida.
4. Desde la entrada principal avanzo hacia abajo y llego a la cocina.
5. En la cocina puedo recoger el cuchillo y el pan, aumentando mi puntaje a 5 y mi peso a 2 kg.
6. Después puedo continuar avanzando, recoger otros ítems o descartar alguno.
7. Si llego a la salida antes de que se termine el tiempo, se muestra el puntaje final y los ítems conseguidos.

## Problemas conocidos

Durante las pruebas realizadas, las funcionalidades principales del programa funcionaron correctamente, incluyendo cargar los escenarios, recoger y descartar ítems, avanzar, reiniciar y finalizar la partida.

También se comprobó que el programa muestra un mensaje cuando se ingresan opciones no válidas y que permite ganar al llegar a la salida o perder cuando se termina el tiempo.

No se encontraron errores durante estas pruebas, aunque no se ha realizado un análisis completo de memoria.

## Contribuciones

La tarea fue desarrollada de manera individual.

Me encargué de implementar la lectura del archivo CSV, las estructuras de datos para representar los escenarios y el jugador, el manejo de los ítems y el inventario, los movimientos entre escenarios, el cálculo del tiempo y puntaje, y los menús del juego.

También realicé pruebas para comprobar el funcionamiento de las diferentes opciones y corregir los errores que aparecieron durante el desarrollo.