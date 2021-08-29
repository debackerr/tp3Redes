all:
	g++ -Wall -c common.cpp 
	g++ -Wall emissor.cpp common.o -o emissor
	g++ -Wall exibidor.cpp common.o -o exibidor
	g++ -Wall servidor.cpp common.o -o servidor 
clean:
