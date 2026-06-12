# Horizon Prime — Архитектура репозиториев

## Репозитории

### horizon-prime-proto (этот репо)
Общий протокол: структуры пакетов, константы, коды сообщений.
Подключается как git submodule в server и client.
Изменение здесь сразу отражается в обоих проектах.

```
horizon-prime-proto/
└── protocol/
    ├── packets.h      # Структуры пакетов (PacketHeader, LoginPacket и т.д.)
    ├── constants.h    # Порты, версии, лимиты
    └── message_types.h # Коды типов сообщений (enum)
```

### horizon-prime-server
C++ сервер, запускается в Docker-контейнере на VPS.
Закрытый репозиторий.

```
horizon-prime-server/
├── src/
├── include/
├── CMakeLists.txt
├── Dockerfile
├── docker-compose.yml
├── .env.example
└── proto/             # git submodule → horizon-prime-proto
```

### horizon-prime-client
C++ клиент с OpenGL окном. Открытый репозиторий.
Пользователи скачивают .exe из GitHub Releases.

```
horizon-prime-client/
├── src/
├── include/
├── assets/
├── CMakeLists.txt
├── proto/             # git submodule → horizon-prime-proto
└── .github/
    └── workflows/
        └── release.yml  # CI: билдит .exe и публикует GitHub Release
```

---

## Почему открытый клиент безопасен

Весь авторитет — на сервере. Клиент только отображает то, что прислал сервер.

- Сервер валидирует каждое действие (позиция, баланс, урон)
- Сервер не отправляет данные о невидимых объектах
- Клиент не может выполнить логику — только отправить запрос
- Открытые исходники не дают читеру больше, чем даёт packet sniffer

Закрытый клиент лишь усложняет реверс-инжиниринг, но не предотвращает его.

---

## Принципы безопасности сервера

1. Никогда не доверять данным от клиента
2. Все игровые состояния хранятся только на сервере
3. PPM: при >5 команд за 3 сек — блокировка на 5 минут
4. Panic Mode: сервер перестаёт отвечать всем кроме админа
5. Никогда не исполнять код из пользовательского ввода

---

## Стек

| Компонент | Технология |
|-----------|------------|
| Клиент    | C++ + OpenGL (GLFW + glad) |
| Сервер    | C++ + Asio (standalone, без Boost) |
| Сеть      | TCP, собственный бинарный протокол |
| БД        | SQLite + Argon2id для паролей |
| Деплой    | Docker + docker-compose на VPS |
| Дистрибуция клиента | GitHub Releases (.exe) |
