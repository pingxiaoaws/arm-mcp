CC=g++
CFLAGS=-O2 -mavx2 -mfma -march=x86-64-v3

app: main.cpp
	$(CC) $(CFLAGS) -o app main.cpp

clean:
	rm -f app
