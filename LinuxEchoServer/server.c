#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define LINE_BUFFER_SIZE 2048 // Увеличенный буфер для строк

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char recv_buffer[BUFFER_SIZE] = {0};

    // Создание TCP-сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязка сокета
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Ошибка привязки");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Ожидание подключений
    if (listen(server_fd, 5) < 0) {
        perror("Ошибка прослушивания");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Эхо-сервер запущен на порту %d\n", PORT);

    while (1) {
        // Принятие подключения
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Ошибка принятия соединения");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Новое подключение: %s:%d\n", client_ip, ntohs(address.sin_port));

        // Буфер для накопления строки
        char line_buffer[LINE_BUFFER_SIZE] = {0};
        size_t line_pos = 0;
        ssize_t bytes_read;

        while ((bytes_read = recv(new_socket, recv_buffer, BUFFER_SIZE, 0)) > 0) {
            for (int i = 0; i < bytes_read; i++) {
                // Обработка конца строки
                if (recv_buffer[i] == '\n' || recv_buffer[i] == '\r') {
                    if (line_pos > 0) {
                        // Отправка полной строки
                        send(new_socket, line_buffer, line_pos, 0);
                        send(new_socket, "\n", 1, 0); // Добавляем перевод строки
                        
                        // Очистка буфера
                        memset(line_buffer, 0, LINE_BUFFER_SIZE);
                        line_pos = 0;
                    }
                } else {
                    // Добавление символа в буфер
                    if (line_pos < LINE_BUFFER_SIZE - 1) {
                        line_buffer[line_pos++] = recv_buffer[i];
                    } else {
                        // Защита от переполнения буфера
                        send(new_socket, "Ошибка: слишком длинная строка\n", 30, 0);
                        memset(line_buffer, 0, LINE_BUFFER_SIZE);
                        line_pos = 0;
                    }
                }
            }
        }

        // Отправка последней строки, если она есть
        if (line_pos > 0) {
            send(new_socket, line_buffer, line_pos, 0);
            send(new_socket, "\n", 1, 0);
        }

        if (bytes_read == 0) {
            printf("Клиент отключился\n");
        } else if (bytes_read < 0) {
            perror("Ошибка чтения данных");
        }

        close(new_socket);
        printf("Соединение закрыто\n\n");
    }

    close(server_fd);
    return 0;
}