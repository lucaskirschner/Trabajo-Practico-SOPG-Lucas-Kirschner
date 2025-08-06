/**
 * @file server_tcp.c
 * @brief Servidor TCP clave-valor para el trabajo práctico de SOPG.
 *
 * Este servidor escucha conexiones en el puerto 5000 y procesa comandos
 * en texto plano: SET, GET y DEL. Almacena la información en archivos
 * locales, usando la clave como nombre de archivo.
 *
 * @author Lucas Kirschner
 * @date 2025-08-05
 * @version 1.0
 *
 * Comandos aceptados:
 * - SET clave valor: crea un archivo llamado "clave" con el contenido "valor"
 * - GET clave: si existe, devuelve su contenido; si no, responde NOTFOUND
 * - DEL clave: borra el archivo si existe (si no existe, también responde OK)
 *
 * Solo atiende un cliente por vez. El cliente puede ser nc o telnet.
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 5000
#define MAX_CMD_LEN 1024
#define RESPONSE_OK "OK\n"
#define RESPONSE_NOTFOUND "NOTFOUND\n"

/**
 * Imprime un mensaje de error y termina el programa.
 */
void error_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

/**
 * Procesa el comando recibido por el cliente.
 *
 * El comando debe ser uno de los siguientes: SET, GET o DEL.
 * Según el comando, realiza operaciones sobre archivos locales
 * y envía una respuesta por el socket.
 */
void procesar_comando(int cliente_fd) {
    char linea[MAX_CMD_LEN + 1] = {0};
    int leidos = read(cliente_fd, linea, MAX_CMD_LEN);
    if (leidos < 0) error_exit("read");
    linea[leidos] = '\0';

    // Eliminar salto de línea final, si lo hay
    char *salto = strchr(linea, '\n');
    if (salto) *salto = '\0';

    // Dividir la línea en partes: comando, clave y valor
    char *comando = strtok(linea, " ");
    char *clave = strtok(NULL, " ");
    char *valor = strtok(NULL, "");

    // Validar que al menos haya comando y clave
    if (!comando || !clave) {
        dprintf(cliente_fd, "ERROR comando inválido\n");
        return;
    }

    if (strcmp(comando, "SET") == 0) {
        int archivo = open(clave, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (archivo < 0) {
            dprintf(cliente_fd, "ERROR al abrir archivo\n");
            return;
        }
        if (valor && write(archivo, valor, strlen(valor)) < 0) {
            close(archivo);
            dprintf(cliente_fd, "ERROR al escribir archivo\n");
            return;
        }
        close(archivo);
        write(cliente_fd, RESPONSE_OK, strlen(RESPONSE_OK));
    }

    else if (strcmp(comando, "GET") == 0) {
        int archivo = open(clave, O_RDONLY);
        if (archivo < 0) {
            write(cliente_fd, RESPONSE_NOTFOUND, strlen(RESPONSE_NOTFOUND));
            return;
        }

        char contenido[4096];
        int bytes = read(archivo, contenido, sizeof(contenido) - 1);
        close(archivo);
        if (bytes < 0) {
            dprintf(cliente_fd, "ERROR al leer archivo\n");
            return;
        }

        contenido[bytes] = '\0';
        dprintf(cliente_fd, "OK\n%s\n", contenido);
    }

    else if (strcmp(comando, "DEL") == 0) {
        unlink(clave); // Si no existe, no importa
        write(cliente_fd, RESPONSE_OK, strlen(RESPONSE_OK));
    }

    else {
        dprintf(cliente_fd, "ERROR comando desconocido\n");
    }
}

/**
 * Función principal del servidor.
 *
 * Crea el socket, lo enlaza al puerto 5000, y queda esperando conexiones.
 * Cuando un cliente se conecta, lee un comando, lo procesa, y luego
 * cierra la conexión. Vuelve a esperar por el siguiente cliente.
 */
int main(void) {
    int servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_fd < 0) error_exit("socket");

    struct sockaddr_in direccion;
    memset(&direccion, 0, sizeof(direccion));
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(PORT);

    if (bind(servidor_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0)
        error_exit("bind");

    if (listen(servidor_fd, 1) < 0)
        error_exit("listen");

    printf("Servidor esperando conexiones en el puerto %d...\n", PORT);

    while (1) {
        struct sockaddr_in cliente;
        socklen_t tam = sizeof(cliente);
        int cliente_fd = accept(servidor_fd, (struct sockaddr *)&cliente, &tam);
        if (cliente_fd < 0) {
            perror("accept");
            continue;
        }

        procesar_comando(cliente_fd);
        close(cliente_fd);
    }

    close(servidor_fd);
    return 0;
}
