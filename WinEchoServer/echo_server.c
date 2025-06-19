#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")  // Автоматическая линковка библиотеки

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};
    int bytes_received;

    // 1. Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Ошибка инициализации Winsock: %d\n", WSAGetLastError());
        return 1;
    }

    // 2. Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Ошибка создания сокета: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 3. Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 4. Привязка сокета
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Ошибка привязки: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // 5. Ожидание подключений
    if (listen(server_fd, 5) == SOCKET_ERROR) {
        printf("Ошибка прослушивания: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Эхо-сервер запущен на порту %d\nОжидание подключений...\n", PORT);

    // 6. Основной цикл сервера
    while (1) {
        // Принятие подключения
        if ((client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len)) == INVALID_SOCKET) {
            printf("Ошибка принятия соединения: %d\n", WSAGetLastError());
            continue;
        }

        // Получение IP клиента
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Новое подключение: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // 7. Обработка данных
        while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
    // Добавляем данные в буфер
    static char line_buffer[BUFFER_SIZE * 2] = {0};
    static size_t line_length = 0;
    
    for(int i = 0; i < bytes_received; i++) {
        // Проверяем конец строки
        if(buffer[i] == '\n' || buffer[i] == '\r') {
            if(line_length > 0) {
                // Отправляем накопленную строку
                send(client_socket, line_buffer, line_length, 0);
                send(client_socket, "\n", 1, 0); // Добавляем перевод строки
                
                // Очищаем буфер
                memset(line_buffer, 0, sizeof(line_buffer));
                line_length = 0;
            }
        } else {
            // Добавляем символ в буфер
            if(line_length < sizeof(line_buffer) - 1) {
                line_buffer[line_length++] = buffer[i];
            }
        }
    }
}

        if (bytes_received == 0) {
            printf("Клиент отключился\n");
        } else if (bytes_received == SOCKET_ERROR) {
            printf("Ошибка чтения: %d\n", WSAGetLastError());
        }

        // Закрытие клиентского сокета
        closesocket(client_socket);
        printf("Соединение закрыто\n\n");
    }

    // Очистка ресурсов (теоретически недостижимо)
    closesocket(server_fd);
    WSACleanup();
    return 0;
}