 #Лабораторная работа 2

Название："Разработка драйверов блочных устройств"
Цель работы: Получение навыков разработки блочных устройств, создания дисков и разделения их на разделы.
шаг
При написании расчета
файл устройства /dev/var2, с
и сохранить его в корре
Результат ядра
содержимое файла.

использовать
входная исходная программа d
исполнительное производство
Создайте точку монтирования: make do
Проверяем созданный диск:
sudo fdisk -l /dev/mydisk
5. Записывать сообщения в кольцевой буфер: dmesg
6. Удалите модуль из ядра и размонтируйте файловую систему: make rm

Примеры использования

##Profiling
1.Copying between virtual disk partitions
lilin@lilin:~# dd if=/dev/mydisk1 of=/dev/mydisk6 bs=512 count=30720 oflag=direct
20480+0 records in
20479+0 records out
10485248 bytes (10 MB, 10 MiB) copied, 2.18378 s, 4.8 MB/s

2.Copy from real disk to virtual disk
lilin@lilin:~# dd if=/dev/sda of=/dev/mydisk6 bs=512 count=30720 oflag=direct
20480+0 records in
20479+0 records out
10485248 bytes (10 MB, 10 MiB) copied, 2.49536 s, 4.2 MB/s

