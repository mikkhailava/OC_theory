# Singleton на базе ортогональных стратегий (Modern C++)

## Описание
Реализация шаблона Singleton с параметризацией стратегиями по Андрею Александреску ("Modern C++ Design").

Поддерживаются стратегии:
- Синхронизации (`SingleThreaded`, `MultiThreaded`)
- Создания (`CreateUsingNew`)
- Уничтожения (`NoDestroy`)

## Как собрать

```bash
g++ src/main.cpp -Iinclude -std=c++17 -o singleton_example
./singleton_example
```

## Пример
При запуске будет выведено:

```
[LOG]: Поток #1
[LOG]: Поток #2
```

Это демонстрирует потокобезопасный доступ к одному экземпляру объекта.
