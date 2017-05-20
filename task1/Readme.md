# Описание:
Каждый процесс получает MPI_recv-ом сообщение, с помощью MPI_Status узнаёт отправлителя и пересылает следующему за собой. 0-ой процесс сначала посылает, а потом принимает. Все действия происходят в цикле N кругов.

# Компиляция:
mpicxx 1b_task.cpp

# Запуск:
mpirun -np 3 ./a.out 8
8 - аргумент программы N

#Лог:
mpirun -np 5 ./a.out 2
thread 0 send message to 1
get message from 0
send message to 2
get message from 1
send message to 3
get message from 2
send message to 4
thread 0 get message from 4
thread 0 send message to 1
get message from 3
send message to 0
get message from 0
send message to 2
get message from 1
send message to 3
get message from 2
send message to 4
get message from 3
send message to 0
thread 0 get message from 4

5 процессов, 2 круга
