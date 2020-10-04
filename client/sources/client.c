#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#define CHAR_MAX 100
#define PORT 4444 //demander à utilisateur

void Lire_commande(char* cmd){
  int i=0;
  while(cmd[i] != '\0' && cmd[i] != '\n'){
    i++;
  }
  cmd[i]='\0';
}

int main(int argc,char *argv[],char *arge[]){

  char servAdr[CHAR_MAX+1]; //Stocke l'adresse
  int clientSocket, ret;
  struct sockaddr_in serveurAdr;
  char buffer[1024];
  struct hostent* h;

  clientSocket = socket(AF_INET, SOCK_STREAM, 0); //Création du socket client
  if(clientSocket < 0){ //Teste si le socket est bien ouvert
    perror("\033[1;31mErreur de connection.\n\033[0;0m");
    exit(1);
  }

  do{
    printf("\033[0;32mEntrez le nom du serveur.\n\033[0;0m");
    fgets(servAdr,CHAR_MAX, stdin);
    Lire_commande(servAdr);
    h=gethostbyname(servAdr);
    if(h==NULL){
      fprintf(stderr,"\033[1;31mServeur inconnu.\n\033[0;0m");
    }else
    if(h->h_addr==NULL){
      perror("\033[1;31mAdresse de serveur inconnu.\n\033[0;0m");
    }
  }while(h==NULL||h->h_addr==NULL); //Tant que le nom du serveur en entrée n'est pas accessible


  memset(&serveurAdr, '\0', sizeof(serveurAdr));
  serveurAdr.sin_family = AF_INET;
  serveurAdr.sin_port = htons(PORT);
  memcpy((&serveurAdr.sin_addr.s_addr),(h->h_addr), sizeof(char*));

  ret = connect(clientSocket, (struct sockaddr*)&serveurAdr, sizeof(serveurAdr)); //On connecte le socket client au serveur
  if(ret < 0){ //Vérifie si la connexion est bonne
    perror("\033[1;31mErreur de connexion.\n\033[0;0m");
    exit(1);
  }

  printf("\033[0;36mConnecté au serveur.\n\033[0;0m");
  do{
    bzero(buffer,1024); //On vide le buffer
    if(recv(clientSocket, buffer, 1024, 0) < 0){ //Si le client n'a pas bien reçu la réponse du serveur
      perror("\033[1;31mErreur de réception des données.\n\033[0;0m");
    }else{
      printf("%s", buffer);
      fgets(buffer,1024, stdin);
      if(buffer[0]=='\n') {
        buffer[0]=' ';
      }
      Lire_commande(buffer);
      send(clientSocket, buffer, strlen(buffer), 0);//On envoie ce qu'a tapé le client

      if(strcmp(buffer, "Quitter") == 0){
        close(clientSocket); //Fermeture du socket
        printf("\033[0;36mDéconnecté du serveur.\n\033[0;0m");
      }
    }
  }while(strcmp(buffer, "Quitter") != 0);//Tant que le client ne saisit pas "Quitter"
  return 0;
}
