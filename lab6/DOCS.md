## Расчет RTT (Round-Trip Time)

Алгоритм в коде (`PingClient::Run`):

1. Фиксация времени отправки
```c++
double startTime = TimeUtils::GetTimeInSeconds();
```

2. Сетевое взаимодействие:
Выполняется отправка (sendto) и ожидание ответа (recvfrom). Пока ответ не придет (или не истечет тайм-аут), программа ждет внутри метода ReceiveFrom.

3. Фиксация времени получения: 
Сразу после успешного возврата из функции получения данных фиксируется текущее время.
```c++
double endTime = TimeUtils::GetTimeInSeconds();
```

4. Вычисление разницы:

```c++
double rtt = endTime - startTime;
```


## Обработка тайм-аутов

Реализация в коде (`UdpSocket`):

1. Настройка сокета
```c++
struct timeval tv;
tv.tv_sec = seconds; // 1 секунда
tv.tv_usec = 0;
setsockopt(m_socketFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
```

2. Поведение recvfrom:

    * Нормальный случай: Данные пришли быстрее чем за 1 секунду. Функция возвращает количество прочитанных байт.

    * Тайм-аут: Прошла 1 секунда, данных нет. Функция recvfrom возвращает -1.

3. Обработка в обертке (ReceiveFrom):
```c++
ssize_t recvBytes = recvfrom(...);
if (recvBytes < 0) {
    return ""; // Возвращаем пустую строку как признак ошибки/тайм-аута
}
```

4. Логика клиента
```c++
if (response.empty()) {
    // recvfrom вернул -1, сработал тайм-аут
    std::cout << "Request timed out" << std::endl;
} else {
    // Ответ получен, считаем RTT
    ...
}
```

