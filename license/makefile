CC = gcc
CFLAGS = -Wall -Wno-address-of-packed-member

all: CheckLicense MakeMachineID MakeLicense

CheckLicense: CheckLicense.c license.c psMD5.c
	$(CC) $(CFLAGS) -o CheckLicense CheckLicense.c license.c psMD5.c

MakeMachineID: MakeMachineID.c license.c psMD5.c
	$(CC) $(CFLAGS) -o MakeMachineID MakeMachineID.c license.c psMD5.c

MakeLicense: MakeLicense.c license.c psMD5.c
	$(CC) $(CFLAGS) -o MakeLicense MakeLicense.c license.c psMD5.c

clean:
	rm -f CheckLicense MakeMachineID MakeLicense

