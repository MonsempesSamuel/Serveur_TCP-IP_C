#ifndef FONCTIONALITE_H
#define FONCTIONALITE_H
#define SUPPL 1.10
#define REDUC 0.80

struct train {
    int numero;
    char* villeDep;
    char* villeArr;
    char* horaireDep;
    char* horaireArr;
    double prix;
    double changement;
};

int nbTrainsTotal();

char* rajouteZero(char* horaire);

int nbTrainsDispo(struct train trains[], int nbTrain, char* villeD, char* villeA, char* debPlageHoraireD, char* finPlageHoraireD);

struct train premierTrainDispo(struct train trains[], int nbTrain, char* villeD, char* villeA, char* horaireD);

struct train* trainsDispoHoraire(struct train trains[], int nbTrain, char* villeD, char* villeA, char* debPlageHoraireD, char* finPlageHoraireD);

struct train* trainsDispoVilleDA(struct train trains[], int nbTrain, char* villeD, char* villeA);

struct train* getTrains();

char* trainToString(struct train* t,int *nb);

int horaireToInt(char* horaire);

int calculTempsTrajet(char* horaireDep, char* horaireArr);

char* fonction1(char* villeDep, char* villeArriv, char* HoraireDep);

struct train* fonction2(char* villeDep, char* villeArriv, char* debPlageHoraireDep, char* finPlageHoraireDep, int *nbDispo);

struct train* fonction3(char* villeDep, char* villeArriv, int *nbDispo);

struct train*ChercherTempOptimum(struct train* t,int nb);

struct train*ChercherCoutOptimum(struct train* t,int nb);

#endif
