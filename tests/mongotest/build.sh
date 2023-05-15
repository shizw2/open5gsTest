gcc mongotest.c -o mongotest $(pkg-config --cflags --libs libmongoc-1.0) -lbson-1.0 -lpthread
