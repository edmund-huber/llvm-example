CPPFLAGS=-Wall
CXXFLAGS=`llvm-config --cppflags`
LDFLAGS=`llvm-config  --ldflags --libs all` -lLLVM-2.7 -lstdc++

all:	client.o host

host: host.o

clean:
	rm *.o host