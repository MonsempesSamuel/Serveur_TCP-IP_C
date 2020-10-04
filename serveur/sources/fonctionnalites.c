#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/fonctionnalites.h"

/*Transforme un horaire "X:XX" en "0X:XX"*/
char* rajouteZero(char* horaire){

  int nbChar = 0;
  while(horaire[nbChar] != ':'){ //On compte s'il y a 1 ou 2 caractères avant le ':'
      nbChar++;
  }
  char* temp = malloc(sizeof(horaire));
  strcpy(temp,horaire);

  if(nbChar < 2){ //Si l'horaire est en format "X:XX", on rajoute un '0' en début de chaine
    horaire=malloc(sizeof('0')+sizeof(temp));
    sprintf(horaire,"0%s",temp);
    horaire[5]='\0';
  }
  return horaire;
}

/*Retourne le nombre total de trains dans le fichier .txt*/
int nbTrainsTotal() {
    FILE *fp;
    char str[10000];
    char* filename = "serveur/Trains.txt";

    fp = fopen(filename, "r");//On ouvre le fichier ayant le filename
    if (fp == NULL){ //Test si le fichier a été bien ouvert
        perror("fopen");
        return 1;
    }
    int compt=0;
    while (fgets(str, 10000, fp) != NULL) { //Boucle qui compte le nombre de retour à la ligne pour connaitre le nombre de trains
        if (strchr(str, '\n') != NULL){
            compt++;
        }
    }
    fclose(fp);
    return compt;

}

/*Retourne le nombre de trains disponibles en fonction des critères sélectionnés*/
int nbTrainsDispo(struct train trains[], int nbTrain, char* villeD, char* villeA, char* debPlageHoraireD, char* finPlageHoraireD) {
    int j = 0;
    for (int i = 0; i < nbTrain; i++){
        if (strcmp(trains[i].villeDep, villeD) == 0 && strcmp(trains[i].villeArr, villeA) == 0){
            if (strcmp(trains[i].horaireDep, debPlageHoraireD) >= 0 && strcmp(trains[i].horaireDep, finPlageHoraireD) <= 0){
                j++;
            }
        }
    }
    return j;
}

/*Retourne le train partant le plus vite en fonction des villes et de l'horaire de départ*/
struct train premierTrainDispo(struct train trains[], int nbTrain, char* villeD, char* villeA, char* horaireD) {
    struct train t = {0, "", "", "", "", 0, 0};
    for (int i = 0; i < nbTrain; i++){
        if (strcmp(trains[i].villeDep, villeD) == 0 && strcmp(trains[i].villeArr, villeA) == 0){
            if (strcmp(trains[i].horaireDep, horaireD) >= 0){
                if (t.numero == 0) {
                    t = trains[i];
                } else if (strcmp(trains[i].horaireDep, t.horaireDep) < 0) {
                    t = trains[i];
                }
            }
        }
    }

    return t;
}

/*Retourne un tableau des trains disponibles en fonction des villes, du nombre de trains et d'une plage horaire de départ*/
struct train* trainsDispoHoraire(struct train trains[], int nbTrain, char* villeD, char* villeA, char* debPlageHoraireD, char* finPlageHoraireD) {
    struct train *t;
    int nbTrainD = nbTrainsDispo(trains, nbTrain, villeD, villeA, debPlageHoraireD, finPlageHoraireD);
    t = malloc(nbTrainD * sizeof(struct train));
    int j = 0;
    for (int i = 0; i < nbTrain; i++){
        if (strcmp(trains[i].villeDep, villeD) == 0 && strcmp(trains[i].villeArr, villeA) == 0){ //Si les villes correspondent
            if (strcmp(trains[i].horaireDep, debPlageHoraireD) >= 0 && strcmp(trains[i].horaireDep, finPlageHoraireD) <= 0){ //Si les horaires correspondent
                t[j] = trains[i]; //On remplit le tableau
                j++;
            }
        }
    }
    return t;
}

/*Retourne les trains disponibles en fonction du nombre de trains, des villes de départ et d'arrivée*/
struct train* trainsDispoVilleDA(struct train trains[], int nbTrain, char* villeD, char* villeA) {
    struct train *t;
    int nbTrainD = nbTrainsDispo(trains, nbTrain, villeD, villeA, "00:00", "24:00"); //On récupère le nombre total de trains de la journée entre les 2 villes
    t = malloc(nbTrainD * sizeof(struct train));
    int j = 0;
    for (int i = 0; i < nbTrain; i++){
        if (strcmp(trains[i].villeDep, villeD) == 0 && strcmp(trains[i].villeArr, villeA) == 0){ //Si les villes correspondent
            t[j] = trains[i]; //On remplit le tableau
            j++;
        }
    }
    return t;
}

/*Retourne un tableau contenant tout les trains du fichier .txt*/
struct train* getTrains() {
    int nb = nbTrainsTotal(); //Nombre total de trains du fichier .txt
    struct train *trains=malloc(sizeof(struct train)*nb);


    FILE *fp;
    char str[10000]="";
    char* filename = "serveur/Trains.txt";


    fp = fopen(filename, "r");
    if (fp == NULL){
        perror("Le fichier ne peut pas être ouvert");
        return NULL;
    }

    char* temp[7];
    int itemp=0;
    int i=0;
    int deb=0;
    int fin=0;
    int k=0;

    int iTrain = 0;
    while (fgets(str, 10000, fp) != NULL){
        temp[6] = NULL;
        itemp = 0;
        i     = 0;
        deb   = 0;
        fin   = 0;
        while (str[i] != '\n' && str[i] != '\0'){ //On découpe les lignes en fonction des espaces et retour à la ligne
            if (str[i] == ';' || str[i+1] == '\n' || str[i+1] == '\0'){  //On découpe chaque ligne en fonction des ';'
                fin = (str[i] == ';') ? i : i+1;
                temp[itemp] = malloc(sizeof(char) * (fin-deb));
                k = 0;
                for (int j = deb; j < fin; j++){ //On ajoute dans le tableau
                    temp[itemp][k] = str[j];
                    k++;
                }
                temp[itemp][k] = '\0'; //Ajout d'un caractère vide à la fin du tableau
                deb = i+1;
                itemp++;
            }
            i++;
        }

        double chg;
        if (temp[6] == NULL) { //On test si il y a une modification sur le prix
            chg = 1;
        } else if (strcmp(temp[6], "REDUC") == 0){
            chg = REDUC;
        } else if (strcmp(temp[6], "SUPPL") == 0) {
            chg = SUPPL;
        }

        int nbChar = 0;
        char temp0[5] = "0";

	/*Test pour le format*/
        while(temp[3][nbChar] != '\0' && temp[3][nbChar] != ':'){
            nbChar++;
        }
        if(nbChar < 2){
            strcat(temp0, temp[3]);
            strcpy(temp[3],temp0);
        }
        while(temp[4][nbChar] != ':'){
            nbChar++;
        }
        char temp0bis[5] = "0";
        if(nbChar < 2){
            strcat(temp0bis, temp[4]);
            strcpy(temp[4],temp0bis);
        }

        struct train t = { atoi(temp[0]), temp[1], temp[2], temp[3], temp[4], atof(temp[5]), chg};
        trains[iTrain++] = t;
    }

    fclose(fp);

    return trains;
}

/*Transforme un horaire de format char* en int */
int horaireToInt(char* horaire){
  int nbChar = 0;
  while(horaire[nbChar] != ':'){ //Compteur pour séparer minutes et heures
    nbChar++;
  }
  int horInt;
  char heures[3];
  char minutes[3];
  if(nbChar <2 ){ //Cas où le format est "X:XX"
    sprintf(heures,"%c",horaire[0]);
    sprintf(minutes,"%c%c",horaire[2],horaire[3]);
  } else { //Cas où le format est "XX:XX"
    sprintf(heures,"%c%c",horaire[0],horaire[1]);
    sprintf(minutes,"%c%c",horaire[3],horaire[4]);
  }
  horInt = strtol(heures,(char **)NULL,10)*60+strtol(minutes,(char **)NULL,10);
  return horInt;
}

/*Calcul en minutes le temps du trajet en fonction des horaires de ce trajet
  Return : Un entier contenant le temps en minutes*/
int calculTempsTrajet(char* horaireDep, char* horaireArr){
  int dep = horaireToInt(horaireDep);
  int duree = horaireToInt(horaireArr) - dep;
  return duree;
  }

/*Paramètres : Ville  de  départ,  Ville  d'arrivée  et  un  Horaire  de  départ
  Return : Premier train possible à partir de l'horaire de départ demandé*/
char* fonction1(char* villeDep, char* villeArriv, char* HoraireDep) {
    HoraireDep=rajouteZero(HoraireDep); //Changement format horaire si besoin ("X:XX" -> "XX:XX")
    int nb = nbTrainsTotal(); //Nombre total trains
    struct train *trains = getTrains(); //On récupère les trains

    struct train t = premierTrainDispo(trains, nb, villeDep, villeArriv, HoraireDep);
    char *str = malloc(1024);
    if(t.numero == 0){ //Test si il y a des trains disponibles
      sprintf(str, "\033[1;33mIl n'y a pas de train disponible\033[0;0m\n");
    } else {
      sprintf(str, "\033[1;33mTrain n°\033[1;35m%d\033[1;33m de \033[1;35m%s\033[1;33m à \033[1;35m%s\033[1;33m partant à \033[1;35m%s\033[1;33m et arrivant à \033[1;35m%s\033[1;33m pour \033[1;35m%.02f\033[1;33m€\n\033[0;0m", t.numero, t.villeDep, t.villeArr,
              t.horaireDep, t.horaireArr, (t.prix * t.changement));
    }
    return str;
}

/*Paramètres : Ville  de  départ,  Ville  d'arrivée, une Plage horaire et le Nombre de trains disponibles
  Return : Tableau de trains ayant un départ durant la plage horaire demandée*/
struct train* fonction2(char* villeDep, char* villeArriv, char* debPlageHoraireDep, char* finPlageHoraireDep, int *nbDispo) {
    debPlageHoraireDep=rajouteZero(debPlageHoraireDep); //Changement format horaire si besoin ("X:XX" -> "XX:XX")
    finPlageHoraireDep=rajouteZero(finPlageHoraireDep); ///Changement format horaire si besoin ("X:XX" -> "XX:XX")
    int nb = nbTrainsTotal(); //Nombre total trains
    struct train *trains = getTrains();  //On récupère les trains

    struct train* t = trainsDispoHoraire(trains, nb, villeDep, villeArriv, debPlageHoraireDep, finPlageHoraireDep);
    *nbDispo = nbTrainsDispo(trains, nb, villeDep, villeArriv, debPlageHoraireDep, finPlageHoraireDep); //On set le nouveau nombre de trains disponibles

    return t;
}

/*Paramètres : Ville  de  départ,  Ville  d'arrivée et le Nombre de trains disponibles
  Return : Tableau contenant tous les trains de la journée entre les deux villes demandées*/
struct train* fonction3(char* villeDep, char* villeArriv, int *nbDispo) {
    int nb = nbTrainsTotal();
    struct train *trains = getTrains();
    struct train* t = trainsDispoVilleDA(trains, nb, villeDep, villeArriv);
    *nbDispo = nbTrainsDispo(trains, nb, villeDep, villeArriv, "00:00", "24:00");

    return t;
}

/*Paramètres : Tableau contenant les trains et la taille de ce tableau
  Return : Une chaine de caractères contenant tous les trains disponibles*/
char* trainToString(struct train* t,int *nb){
  int test=0;
  char *str = malloc(1024);
  strcpy(str,"");
  for (int i = 0; i < *nb; i++) {
    if(t[i].numero != 0)test=1; //Si il y a des trains disponibles
      sprintf(str, "%s\033[1;33mTrain n°\033[1;35m%d\033[1;33m de \033[1;35m%s\033[1;33m à \033[1;35m%s\033[1;33m partant à \033[1;35m%s\033[1;33m et arrivant à \033[1;35m%s\033[1;33m pour \033[1;35m%.02f\033[1;33m€\n\033[0;0m", str, t[i].numero, t[i].villeDep, t[i].villeArr,
            t[i].horaireDep, t[i].horaireArr, (t[i].prix * t[i].changement));
          }
    if(!test){strcpy(
      str,"\033[1;33mIl n'y a pas de train disponible\033[0;0m\n\033[0;32mMenu :\n(1)       Recherche du premier train possible sur un trajet à partir d'un horaire\n(2)       Recherche des trains possibles sur un trajet et une tranche horaire\n(3)       Recherche des trains possibles sur un trajet\n(Quitter)\nVeuillez entrer 1, 2, 3 ou Quitter\n\033[0;0m");
      nb=0;
    }
return str; //Test si il y a des trains disponibles - Sinon on renvoit une chaine de caractères : "Il n'y a pas de train disponible"

}

/*Paramètres : Tableau contenant les trains et la taille de ce tableau
  Return : Un tableau ayant comme premier élément le train ayant le trajet le plus court*/
struct train*ChercherTempOptimum(struct train* t,int nb){
  struct train* train = malloc(sizeof(struct train));
  int min=calculTempsTrajet(t[0].horaireDep,t[0].horaireArr);
  train[0]=t[0];
  for (int i = 1; i < nb; i++) { //Parcours le tableau des trains disponibles
    if(calculTempsTrajet(t[i].horaireDep,t[i].horaireArr)<min){ //Si la durée du trajet est minimale
      min=calculTempsTrajet(t[i].horaireDep,t[i].horaireArr);
      train[0]=t[i];
    }
  }
  return train;
}

/*Paramètres : Tableau contenant les trains et la taille de ce tableau
  Return : Un tableau ayant comme premier élément le train ayant le meilleur prix*/
struct train*ChercherCoutOptimum(struct train* t,int nb){
  struct train* train = malloc(sizeof(struct train));
  double min=t[0].prix*t[0].changement;
  train[0]=t[0];
  for (int i = 1; i < nb; i++) { //Parcours le tableau des trains disponibles
    if(t[i].prix*t[i].changement<min){ //Si le prix du trajet est minimal
      min=t[i].prix*t[i].changement;
      train[0]=t[i];
    }
  }
  return train;
}
