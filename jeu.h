#ifndef JEU_H_INCLUDED
#define JEU_H_INCLUDED

#include <stdbool.h>

#define MAX_LENGTH 50

typedef struct {
    char nom[MAX_LENGTH];
    int score;
} Joueur;

void afficherLettresDevinees(const char* word, const char* guessedLetters);

const char* getRandomWordFromFile(const char* filename);

void ajouterMotAuFichier(const char* word, const char* filename);

void afficherMessagePositifAleatoire(const char* filename);

void afficherMessageNegatifAleatoire(const char* filename);

void dessiner_Pendu(const char* word, int incorrectAttempts, int maxAttempts);

void PenduContreTemps(const char* file, int maxTime);

void PenduNbreTentatives(const char* file, int maxAttempts);

Joueur chargerMeilleurScore(const char* filename);

void sauvegarderMeilleurScore(const char* filename, Joueur meilleurScore);

Joueur PenduMultijoueur(Joueur joueur1, Joueur joueur2, const char* filename, const char* filenameScore, int maxAttempts);

int modeDuJeu(const char* filename);

#endif
