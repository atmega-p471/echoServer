#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int server_running = 1;

// Структура для передачи данных клиента в поток
struct client_data {
    int socket;
    struct sockaddr_in address;
};

// Обработчик сигнала для остановки сервера
void stop_server(int sig) {
    printf("\nОстанавливаем сервер...\n");
    server_running = 0;
}



// Функция обработки клиента в отдельном потоке
void* handle_client(void* arg) {
    struct client_data* client = (struct client_data*)arg;
    char buffer[BUFFER_SIZE];
    char client_ip[16];
    int bytes_read;
    
    // Получаем IP адрес клиента
    inet_ntop(AF_INET, &client->address.sin_addr, client_ip, 16);
    printf("Подключился клиент: %s:%d\n", client_ip, ntohs(client->address.sin_port));
    
    // Читаем данные от клиента и отправляем их обратно
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_read = recv(client->socket, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_read <= 0) {
            break;
        }
        
        // Отправляем данные обратно клиенту
        send(client->socket, buffer, bytes_read, 0);
    }
    
    // Закрываем соединение
    printf("Клиент %s:%d отключился\n", client_ip, ntohs(client->address.sin_port));
    close(client->socket);
    free(client);
    return NULL;
}

int main(int argc, char* argv[]) {
    int server_socket;
    struct sockaddr_in server_addr;
    int port = PORT;
    
    // Проверяем аргументы командной строки
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            printf("Неправильный порт, используем %d\n", PORT);
            port = PORT;
        }
    }
    
    // Устанавливаем обработчик сигнала
    signal(SIGINT, stop_server);
    
    // Создаем сокет
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Ошибка создания сокета\n");
        return 1;
    }
    
    // Настраиваем повторное использование адреса
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // Привязываем сокет к адресу
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Ошибка привязки сокета\n");
        close(server_socket);
        return 1;
    }
    
    // Начинаем слушать входящие соединения
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        printf("Ошибка прослушивания\n");
        close(server_socket);
        return 1;
    }
    
    printf("Сервер запущен на порту %d\n", port);
    printf("Нажмите Ctrl+C для остановки\n");
    
    // Основной цикл сервера
    while (server_running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket;
        
        // Принимаем новое соединение
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            if (server_running) {
                printf("Ошибка принятия соединения\n");
            }
            continue;
        }
        
        // Создаем структуру для передачи данных в поток
        struct client_data* client = malloc(sizeof(struct client_data));
        if (client == NULL) {
            printf("Ошибка выделения памяти\n");
            close(client_socket);
            continue;
        }
        
        client->socket = client_socket;
        client->address = client_addr;
        
        // Создаем новый поток для обработки клиента
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client) != 0) {
            printf("Ошибка создания потока\n");
            close(client_socket);
            free(client);
            continue;
        }
        
        // Отсоединяем поток
        pthread_detach(thread);
    }
    
    // Закрываем сервер
    printf("Закрываем сервер\n");
    close(server_socket);
    return 0;
} 