all:
	g++ -c -fPIC main.cpp -o main.o
	g++ -shared -Wl,-soname,libdb.so -o libdb.so main.o

