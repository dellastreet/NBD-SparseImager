nbdsrvr is (C) 2003 by folkert@vanheusden.com
New versions can be obtained from: http://www.vanheusden.com/ (browse to the
"microsoft windows software" section).

Usage:
nbdsrvr filename portnumber [partitionnumber]

Serving a file-image
-=----------------=-
filename must be an image of a filesystem or whatever kind of device you
whish to use. Can also be an empty file. For example, create an empty file
of 10MB called
"image.dat"
Then:
nbdsrvr image.dat 9000
On your linux-box:
nbd-client hostname 9000 /dev/ndX
hostname is the hostname of your windows-box, /dev/ndX is the device you
want to couple the image to. After that, you could create a filesystem o
this image: on your Linux-box, type:
mke2fs /dev/ndX
and then mount it:
mount /dev/ndX /mnt

Serving a partition(!)
-=------------------=-
filename must be a disk-identifier. Something like: \\.\PHYSICALDRIVE0
for the first phsyical(!) disk, \\.\PHYSICALDRIVE1   for the second
physical disk, etc. When using a physical disk, you can can enter a
partition-number. Default is partition 0.
For example.
nbdsrvr \\.\PHYSICALDRIVE0 9000 0
This will serve the first partition on the first physical disk. The server
will listen portnumber 9000, that is something you need to know to for the
client on the Linux-system.

                    -*--------=] WARNING! [=--------*-
Do *NOT* share partitions/files that are already in mounted/in use! It is
almost for sure that corruptions will occure! Be really carefull when using
this kind of tooling: it is easy to make mistakes and damage file(systems)
beyond repair!
I do *NOT* take any responsibilities for any damage you'll find on your way.
If you do not agree with this limitation, you are *NOT* allowed to use this
program!!


Good luck!


-- Folkert van Heusden, 2003/07/14, 19:39