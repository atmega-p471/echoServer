# Простой многопоточный эхо-сервер для Windows

Это простая программа на языке C для Windows, которая создает эхо-сервер. Сервер принимает подключения от клиентов и отправляет им обратно все, что они присылают.

## Что умеет программа

- Принимает множество клиентов одновременно (многопоточность)
- Отправляет обратно все полученные данные (эхо)
- Можно настроить порт через аргумент командной строки
- Корректно завершается по Ctrl+C

## Как компилировать

### С MinGW:
```cmd
gcc -o echo_server.exe echo_server.c -lws2_32
```

### С Visual Studio:
```cmd
cl echo_server.c ws2_32.lib
```

## Как запустить

### Настройка кодировки (чтобы русские буквы отображались правильно):
```cmd
chcp 65001
```

### На порту по умолчанию (8080):
```cmd
echo_server.exe
```

### На другом порту:
```cmd
echo_server.exe 9090
```

## Как тестировать

### С помощью telnet:
```cmd
telnet localhost 8080
```
Введите любой текст и нажмите Enter - сервер вернет его обратно.

### С помощью netcat (если установлен):
```cmd
ncat localhost 8080
```

### Несколько клиентов одновременно:
Откройте несколько командных строк и подключитесь к серверу из каждой.

## Как работает программа

1. **Главный поток** - ждет новых подключений
2. **Рабочие потоки** - каждый клиент обрабатывается в своем потоке
3. **Структура client_data** - передает информацию о клиенте в поток
4. **WinSock** - библиотека Windows для работы с сетью

## Основные функции

- `main()` - создает сервер и ждет подключений
- `handle_client()` - обрабатывает одного клиента 
- `stop_server()` - останавливает сервер по Ctrl+C

## Настройки в коде

```c
#define PORT 8080        // Порт по умолчанию
#define BUFFER_SIZE 1024 // Размер буфера для данных
#define MAX_CLIENTS 10   // Максимум клиентов в очереди
```
