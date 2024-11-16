/*
 * Indications (à respecter) :
 * - Les erreurs sont gérées avec des assert ; les erreurs traitées sont :
 *    . appel trop tôt ou trop tard d'une méthode (cf. config.h)
 *    . fichier de configuration inaccessible
 *    . une position erronée
 * - Le fichier (si on arrive à l'ouvrir) est considéré comme bien
 *   formé sans qu'il soit nécessaire de le vérifier
 *
 * Un code minimal est fourni et permet d'utiliser le module "config" dès
 * le début du projet ; il faudra le remplacer par l'utilisation du fichier
 * de configuration.
 * Il est inutile de faire plus que ce qui est demandé
 *
 * Dans cette partie vous avez le droit d'utiliser les entrées-sorties
 * de haut niveau (fopen, fgets, ...)
 */


// TODO include des .h système
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../UTILS/myassert.h"

#include "config.h"

// TODO Définition des données ici
#define MAX_SERVICES 10 // Définir le nombre maximum de services

static int nbServices; // Nombre de services
static char exeName[256]; // Nom de l'exécutable
static bool serviceStatus[MAX_SERVICES]; // Statut des services (ouvert ou fermé)
static bool initialized = false; // Indicateur d'initialisation


void config_init(const char *filename)
{
    // TODO erreur si la fonction est appelée deux fois
    myassert(!initialized, "config_init a déjà été appelé"); // Vérifie si config_init a déjà été appelé

    // TODO code vide par défaut, à remplacer
    //      il faut lire le fichier et stocker toutes les informations en
    //      mémoire
    FILE *file = fopen(filename, "r"); // Ouvre le fichier de configuration
    myassert(file != NULL, "Impossible d'ouvrir le fichier de configuration"); // Vérifie si le fichier s'est ouvert correctement

    fscanf(file, "%d", &nbServices); // Lit le nombre de services
    myassert(nbServices <= MAX_SERVICES, "Le nombre de services dépasse la limite autorisée"); // Vérifie que le nombre de services est dans la limite

    fscanf(file, "%s", exeName); // Lit le nom de l'exécutable

    for (int i = 0; i < nbServices; i++) {
        int pos;
        char status[10];
        fscanf(file, "%d %s", &pos, status); // Lit la position et le statut du service
        myassert(pos >= 0 && pos < MAX_SERVICES, "Position du service non valide"); // Vérifie que la position est valide
        serviceStatus[pos] = (strcmp(status, "ouvert") == 0); // Définit le statut du service
    }

    fclose(file); // Ferme le fichier
    initialized = true; // Marque comme initialisé
}

void config_exit()
{
    // TODO erreur si la fonction est appelée avant config_init
    myassert(initialized, "config_exit appelé avant config_init"); // Vérifie que config_exit est appelé après config_init

    // TODO code vide par défaut, à remplacer
    //      libération des ressources
    initialized = false; // Réinitialise l'indicateur
}

int config_getNbServices()
{
    // erreur si la fonction est appelée avant config_init
    // erreur si la fonction est appelée après config_exit
    myassert(initialized, "config_getNbServices appelé avant l'initialisation"); // Vérifie que la fonction est appelée après l'initialisation
    
    // code par défaut, à remplacer
    return nbServices; // Retourne le nombre de services
}

const char * config_getExeName()
{
    // TODO erreur si la fonction est appelée avant config_init
    // TODO erreur si la fonction est appelée après config_exit
    myassert(initialized, "config_getExeName appelé avant l'initialisation"); // Vérifie que la fonction est appelée après l'initialisation
    
    // TODO code par défaut, à remplacer
    return exeName; // Retourne le nom de l'exécutable
}

bool config_isServiceOpen(int pos)
{
    // TODO erreur si la fonction est appelée avant config_init
    // TODO erreur si la fonction est appelée après config_exit
    // TODO erreur si "pos" est incorrect
    myassert(initialized, "config_isServiceOpen appelé avant l'initialisation"); // Vérifie que la fonction est appelée après l'initialisation
    myassert(pos >= 0 && pos < nbServices, "Position du service non valide"); // Vérifie que la position est valide

    // TODO code par défaut, à remplacer
    return serviceStatus[pos]; // Retourne le statut du service
}
