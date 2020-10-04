all: client serveur

client: client/sources/client.o
	gcc $^ -o $@/bin/$@ -Wall -Werror
	rm $@/sources/*.o

serveur: serveur/sources/serveur.o serveur/sources/fonctionnalites.o
	gcc $^ -o $@/bin/$@ -Wall -Werror
	rm $@/sources/*.o
