# Первое письмо

Добрый вечер, господа!
Высылаю обещанные материалы по примитивам синхронизации (семафоры и взаимоисключения в одном документе, условные переменные в другом)
Общий обзор pthreads можно найти в Google :)

Задача же следующая:
сделать параллельный (многопоточный) вариант игры Жизнь, правила [тут](https://ru.wikipedia.org/wiki/%D0%96%D0%B8%D0%B7%D0%BD%D1%8C_(%D0%B8%D0%B3%D1%80%D0%B0))

Язык программирования - C/C++ (на ваш выбор), многопоточность - pthreads

Модель реализации - master-worker, master в этой модели не занимается расчетами, а обеспечивает консольный интерфейс пользователя с командами:
1. START Старт с заданным начальным распределением (см далее) и количеством потоков K
Начальное распределение может быть задано в двух вариантах (нужно реализовать оба):
    а) файл в формате CSV (https://ru.wikipedia.org/wiki/CSV)
    б) задаются лишь размеры NxM, поле генерируется случайным образом
2. STATUS Отображение статуса текущей итерации (на которой остановились, после старта -  отображение начального распределения)
3. RUN Выполнение Y итераций
4. STOP Досрочная остановка вычислений:
потоки-worker'ы приостанавливаются, вычислив текущую итерации и на консоли выводится ее номер (с возможностью дальнейшей печати с помощью STATUS)
5. QUIT Завершение программы с корректной приостановкой потоков

Нужно продемонстрировать применение всех описанных примитивов синхронизации
(mutex, semaphore, conditional variable)

На ближайшем занятии отвечу на все вопросы, которые у вас возникнут по мере реализации задания
Приемка будет идти в индивидуальном режиме до конца семестра во время семинаров

# Уточнение

1. START подразумевают сразу начало выполнений итераций или просто считывание начальных условий и создание worker'ов?
2. После команды STOP может ли следовать команда RUN или START?
3. Команда RUN Y запускается после старта или в момент выполнения можно сказать, что надо выполнить еще У итераций и остановиться?
4. mutex и семафоры, обязательное ли использование обоих вариантов в программе или можно обойтись одним, если получится?

1. только инициализацию и создание спящих потоков
2. RUN корректна, START - нет (она разовая и вы эту ошибку корректно отрабатываете)
RUN - это аналог NEXT в отладчике с указанием к-ва шагов
3. в момент остановленных вычислений (после START и STOP), при запуске при работающих WORKER'а надо сказать, что система занята; про шаги верно
4. еще и conditonal variables
нужно все три варианта
просто похожие синхронизации сделайте по-разному
