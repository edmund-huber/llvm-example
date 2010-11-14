CPPFLAGS=-Wall
CXXFLAGS=`llvm-config --cppflags`
LDFLAGS=`llvm-config  --ldflags --libs all` -lLLVM-2.7 -lstdc++

all:	client.s.bc host

client.s.bc: client.s
	llvm-as client.s

client.s: client.cl
	clang -emit-llvm -S client.cl

host: host.o

clean:
	rm *.o *.s *.s.bc host || true