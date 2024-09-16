# TP1-SO
En este trabajo práctico, se busca desarrollar un sistema que distribuya el cálculo del hash MD5 de múltiples archivos entre diferentes procesos utilizando los mecanismos de Inter-Process Communication (IPC). El objetivo principal es implementar una aplicación que divida el cálculo de manera eficiente, demostrando la ventaja de separar las tareas paralelamente.
El sistema se compone de tres procesos. El primero es el proceso llamado aplicación (app) que recibe los archivos a procesar y crea los procesos esclavos (slave), compila los resultados y se comunica con el proceso vista (view) si es activado. Luego, están los procesos esclavos que son responsables de recibir un path de un archivo y calcular el hash MD5. Por último, se encuentra el proceso vista que al ser ejecutado se conecta a un buffer compartido y muestra en pantalla los resultados a medida que son procesados.
Para implementar la comunicación entre los procesos se emplearon diversos mecanismos de IPC, como pipes anónimos para la comunicación entre el proceso aplicación y los esclavos, y memoria compartida junto con semáforos para la sincronización y comunicación entre el proceso aplicación y el proceso vista.

## Miembros
Nacuzzi, Gianna Lucia - 64006
Squillari, Maria Agostina - 64047
Frutos, Pilar - 64225