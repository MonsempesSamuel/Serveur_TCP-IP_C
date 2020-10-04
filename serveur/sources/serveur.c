#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "../headers/serveur.h"
#include "../headers/fonctionnalites.h"

#define PORT 4444

//paramètres :  chaine de caractères heure
//retour :      1 si l'heure est correcte et 0 sinon
int regex_heure(char* heure){
  int correct = 1;
  int deuxpoints = 0;
  int inddeuxpoints = 0;
  int i = 0;
  while(i < 5 && heure[i] != '\0' && correct == 1) {  //tant qu'on na pas atteint le dernier caractère et que le caractère courant n'est pas celui de fin de chaine e caractères
    if (heure[i] == ':') {
      inddeuxpoints = i;
      deuxpoints++;
      if (i != 1 && i != 2) {
        correct = 0;
      }
    } else {
      if (!isdigit(heure[i])) { //faux si caractère autre que : ou digit
        correct = 0;
      }
    }
    i++;
  }
  if (i != 4 && i != 5) { // faux si heure a moins de 4 caractères
    correct = 0;
  }
  if (deuxpoints > 1) { //faux si plus que 1 ':'
    correct = 0;
  }
  if (i == 5 && inddeuxpoints == 1) { //faux si 3 caractères après :
    correct = 0;
  }
  return correct;
}

//paramètres :  chaine de caractères ville
//retour :      1 si la ville est correcte et 0 sinon
int regex_ville(char* ville) {
  int correct = 1;
  int i = 0;
  char courant;
  char precedent;
  while (i < strlen(ville) && ville[i] != '\0' && correct == 1) {
    courant = ville[i];
    if ((courant >= 'a' && courant <= 'z') || (courant >= 'A' && courant <= 'Z')/* || (courant == "à") || (courant == 'â') || (courant == 'ä') || (courant == 'é') || (courant == 'è') || (courant == 'ë') || (courant == 'ê') || (courant == 'ï') || (courant == 'î') || (courant == 'ô') || (courant == 'ö') || (courant == 'ù') || (courant == 'û') || (courant == 'ü') || (courant == 'ç')*/) {
      i++;
      precedent = courant;
    } else if (courant == '\'' || courant == ' '  || courant == '-') {
      if (i == 0) {
        correct = 0;
      } else if (precedent == '\'' || precedent == ' ' || precedent == '-') {  // cas où deux caractères spéciaux se suivent
        correct = 0;
      } else {
        i++;
        precedent = courant;
      }
    } else {
      correct = 0;
    }
  }
  return correct;
}

void finfils(){
  wait(NULL);
}

int main(){
  int menu = 0;
  char* villeDepart;
  char* villeArrivee;
  char* heureDepart;
  char* heureArrivee;
  struct sigaction a;
  a.sa_handler = finfils;
  a.sa_flags = SA_RESTART;
  sigaction(SIGCHLD, &a, NULL);
  int sockecoute, retour, sockclient, no, n1, status=0;
  struct sockaddr_in serveurAdr, clientAdr;
  socklen_t adrsizeclient;
  char buffer[1024];
  pid_t childpid;

  //création du socket d'écoute et véréfication qu'il est bien créé
  sockecoute = socket(AF_INET, SOCK_STREAM, 0);
  if(sockecoute < 0){
    perror("\033[1;31merreur de connection.\n\033[0;0m");
    exit(1);
  } else {
    printf("La socket de serveur est créée.\n");
    memset(&serveurAdr, '\0', sizeof(serveurAdr));
    serveurAdr.sin_family = AF_INET;
    serveurAdr.sin_port = htons(PORT);
    serveurAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    retour = bind(sockecoute, (struct sockaddr*)&serveurAdr, sizeof(serveurAdr));
    if(retour < 0){
      perror("\033[1;31merreur dans le bind.\n\033[0;0m");
      exit(1);
    } else {
      printf("Bind au port %d\n", PORT);
      if(listen(sockecoute, 10) == 0){
        printf("Ecoute....\n");
      } else {
        perror("\033[1;31merreur dans le bind.\n\033[0;0m");
      }
      while(1){
        sockclient = accept(sockecoute, (struct sockaddr*)&clientAdr, &adrsizeclient);
        if(sockclient < 0){
          exit(1);
        }
        printf("Connection acceptée de %s:%d\n", inet_ntoa(clientAdr.sin_addr), ntohs(clientAdr.sin_port));
        switch (n1=fork()){
          case -1:
          perror("\033[1;31mEchec de création du processus fils\033[0;0m");
          exit(-1);
          break;
          case 0:
          close(sockecoute);
          struct train* trains;
          int nb=0;
          char* msgPasDeTrain="\033[1;33mIl n'y a pas de train disponible\033[0;0m\n";
          char* msg_menu = "\033[0;32mMenu :\n(1)       Recherche du premier train possible sur un trajet à partir d'un horaire\n(2)       Recherche des trains possibles sur un trajet et une tranche horaire\n(3)       Recherche des trains possibles sur un trajet\n(Quitter)\nVeuillez entrer 1, 2, 3 ou Quitter\n\033[0;0m";
          char* msg_menu2 = "\033[0;32mMenu :\n(1)       Recherche du  train le plus optimum niveau temps\n(2)       Recherche du  train le plus optimum niveau prix\nVeuillez entrer 1 ou 2\n\033[0;0m";
          char* msg = msg_menu;
          do{
            send(sockclient, msg, strlen(msg), 0);
            bzero(buffer,1024);
            if (recv(sockclient, buffer, 1024, 0) < 0) {
              perror("\033[1;31mErreur de reception des données.\n\033[0;0m");
            } else {
              printf("Client: %s\n",buffer);  //affiche la réponse du client côté serveur
              if(strcmp(buffer, msg) == 0||buffer[0]=='\0'){
                strcpy(buffer, "Quitter");
              } else {
                if (menu == 0) {
                  if (strcmp(buffer, "1") == 0 || strcmp(buffer, "2") == 0 || strcmp(buffer, "3") == 0) {
                    char**e;
                    menu = (int) strtol(buffer, e, 10);
                    msg = "\033[0;32mVeuillez entrer la ville de départ : \n\033[0;0m";
                  } else {
                    msg = "\033[0;32mMerci de bien entrer 1, 2, 3 ou Quitter\n\033[0;0m";
                  }
                } else if (menu == 1 || menu == 2 || menu == 3) {
                  if (regex_ville(buffer) == 1) {
                    villeDepart = malloc(sizeof(buffer));
                    strcpy(villeDepart,buffer);
                    msg = "\033[0;32mVeuillez entrer la ville d'arrivée : \n\033[0;0m";
                    menu += 10;
                  }
                } else if (menu == 11) {
                  if (regex_ville(buffer) == 1) {
                    villeArrivee = malloc(sizeof(buffer));
                    strcpy(villeArrivee,buffer);
                    msg = "\033[0;32mVeuillez entrer l'horaire de départ (de forme xx:xx ex : 13:50 pour treize heure cinquantes minutes) : \n\033[0;0m";
                    menu += 10;
                  }
                } else if (menu == 21) {  //AFFICHAGE
                  if(regex_heure(buffer) == 1) {
                    heureDepart = malloc(sizeof(buffer));
                    strcpy(heureDepart,buffer);
                    char* retour = fonction1(villeDepart, villeArrivee, heureDepart);
                    msg = malloc(sizeof(retour)+sizeof(msg_menu));
                    msg = retour;
                    strcat(msg, msg_menu);
                    menu = 0;
                  }
                } else if (menu == 12) {
                  if (regex_ville(buffer) == 1) {
                  villeArrivee = malloc(sizeof(buffer));
                    strcpy(villeArrivee,buffer);
                    msg = "\033[0;32mVeuillez entrer l'heure à partir de laquelle vous souhaitez partir (de forme xx:xx ex : 13:50 pour treize heure cinquantes minutes) : \n\033[0;0m";
                    menu += 10;
                  }
                } else if (menu == 22) {
                  if (regex_heure(buffer) == 1) {
                    heureDepart = malloc(sizeof(buffer));
                    strcpy(heureDepart,buffer);
                    msg = "\033[0;32mVeuillez entrer l'heure jusqu'à laquelle vous souhaitez partir (de forme xx:xx ex : 13:50 pour treize heure cinquantes minutes) : \n\033[0;0m";
                    menu += 10;
                  }
                }  else if (menu == 32) {  //AFFICHAGE
                  if (regex_heure(buffer) == 1) {
                    heureArrivee = malloc(sizeof(buffer));
                    strcpy(heureArrivee,buffer);
                    trains = fonction2(villeDepart, villeArrivee, heureDepart, heureArrivee, &nb);
                    char* retour = trainToString(trains, &nb);
                    if(nb==0){
                      msg = malloc(sizeof(retour)+sizeof(msg_menu2));
                      msg = retour;
                      //  strcat(msg, msg_menu);                                                A FAIRE
                      menu = 0;
                    }else{
                      msg = malloc(sizeof(retour)+sizeof(msg_menu2));
                      msg=retour;
                      strcat(msg, msg_menu2);
                      menu += 10;
                    }
                  }
                } else if (menu == 42) {
                  if((strcmp(buffer, "1") == 0 )|| (strcmp(buffer, "2") == 0)){
                    trains = (strcmp(buffer, "1") == 0 )? ChercherTempOptimum(trains, nb) : ChercherCoutOptimum(trains, nb);
                    nb=1;
                    char* retour = trainToString(trains, &nb);
                    msg = realloc(msg,sizeof(retour)+sizeof(msg_menu));
                    strcpy(msg,retour);
                    strcat(msg, msg_menu);
                    menu = 0;
                  }
                } else if (menu == 13) {  //AFFICHAGE
                  if (regex_ville(buffer) == 1) {
                    villeArrivee = malloc(sizeof(buffer));
                    strcpy(villeArrivee,buffer);
                    trains = fonction3(villeDepart, villeArrivee,&nb);
                    char* retour = trainToString(trains, &nb);
                    if(nb==0){
                      msg = malloc(sizeof(retour)+sizeof(msg_menu2));
                      msg = retour;
                      //  strcat(msg, msg_menu);                                                A FAIRE
                      menu = 0;
                    }else{
                      msg = malloc(sizeof(retour)+sizeof(msg_menu2));
                      msg=retour;
                      strcat(msg, msg_menu2);
                      menu =42;
                    }
                  }
                } else {
                  printf("\033[0;32mR.I.P.\033[0;0m");
                }
              }
            }
          } while(strcmp(buffer, "Quitter") != 0);
          char err[80]="";
          strcat(err,"Déconnecté de ");
          strcat(err,inet_ntoa(clientAdr.sin_addr));
          strcat(err,"\n");
          printf("%s",err);
          exit(0);
          break;
          default:
          close(sockclient);
        }
      }
    }
  }
  return 0;
}
