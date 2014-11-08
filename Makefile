all:
	g++ -c -fPIC main.cpp -o main.o -g -ggdb
	g++ -shared -Wl,-soname,libdb.so -o libdb.so main.o -g -ggdb


