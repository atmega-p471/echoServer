# Простой многопоточный эхо-сервер для Linux

Это простая программа на языке C, которая создает эхо-сервер. Сервер принимает подключения от клиентов и отправляет им обратно все, что они присылают.

## Что умеет программа

- Принимает множество клиентов одновременно (многопоточность)
- Отправляет обратно все полученные данные (эхо)
- Можно настроить порт через аргумент командной строки
- Корректно завершается по Ctrl+C

## Как компилировать

```bash
gcc -o server server.c -lpthread
```

## Как запустить

### На порту по умолчанию (8080):
```bash
./server
```

### На другом порту:
```bash
./server 9090
```

## Как тестировать

### С помощью telnet:
```bash
telnet localhost 8080
```
Введите любой текст и нажмите Enter - сервер вернет его обратно.

### С помощью netcat:
```bash
nc localhost 8080
```

### Несколько клиентов одновременно:
Откройте несколько терминалов и подключитесь к серверу из каждого.

## Как работает программа

1. **Главный поток** - ждет новых подключений
2. **Рабочие потоки** - каждый клиент обрабатывается в своем потоке
3. **Структура client_data** - передает информацию о клиенте в поток

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
