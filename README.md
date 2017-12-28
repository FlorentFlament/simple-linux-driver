Simple Linux Driver

This is one of the simplest Linux driver that "does something". I
wrote this trying to figure out how to register actual modules to the
Kernel based on the [Linux Device Drivers, Third Edition][1] book,
chapter 3.

We need a built Kernel tree (see [Installing a Vanilla Linux Kernel on
Fedora][2])

Let's play with it:

```text
$ make
...
$ insmod scull.ko
$ dmesg | tail -3
[297172.851954] scull: Hello !
[297172.851956] scull: Char device number registered: 243 0
[297172.851957] scull: Char device successfuly added to system
$ sudo mknod scull0 c 243 0
$ sudo chmod a+w scull0
$ echo "Hey !" > scull0
$ dmesg | tail -3
[297338.548802] scull: Read 6 bytes from user
[297338.548804] scull: Hey !

$ dd if=scull0 count=5
Hello !
Hello !
Hello !
Hello !
Hello !
0+5 records in
0+1 records out
40 bytes copied, 6.6226e-05 s, 604 kB/s
$ dmesg | tail -5
[297388.485172] scull: Sent 8 bytes to user
[297388.485174] scull: Sent 8 bytes to user
[297388.485176] scull: Sent 8 bytes to user
[297388.485177] scull: Sent 8 bytes to user
[297388.485178] scull: Sent 8 bytes to user
$ sudo rmmod scull
$ dmesg | tail -3
[297443.547953] scull: Char device removed from system
[297443.547956] scull: Char device number unregistered: 243 0
[297443.547957] scull: Bye !
```

[1]: https://lwn.net/Kernel/LDD3/
[2]: http://www.florentflament.com/blog/installing-a-vanilla-linux-kernel-on-fedora.html
