#include "jeu.h"

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <windows.h>
#include <MMsystem.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>




/*--------------------------------------------------------------------------------------------------------------*/
/* Les fonctions du jeu du Pendu */

void afficherLettresDevinees(const char* word, const char* guessedLetters) {
    for (int i = 0; i < strlen(word); i++) {
        if (strchr(guessedLetters, word[i]) != NULL) {
            printf("%c ", word[i]);
        } else {
            printf("_ ");
        }
    }
    printf("\n");
}


const char* getRandomWordFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(EXIT_FAILURE);
    }

    int lineCount = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        lineCount++;
    }

    if (lineCount == 0) {
        fprintf(stderr, "Erreur : le fichier est vide.\n");
        exit(EXIT_FAILURE);
    }

    rewind(file);

    srand(time(NULL));
    int randomIndex = rand() % lineCount;

    for (int i = 0; i < randomIndex; i++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            fprintf(stderr, "Erreur : impossible d'atteindre l'indice aléatoire.\n");
            exit(EXIT_FAILURE);
        }
    }

    char* newline = strchr(line, '\n');
    if (newline != NULL) {
        *newline = '\0';
    }

    fclose(file);

    char* randomWord = (char*)malloc(strlen(line) + 1);
    if (randomWord == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(randomWord, line);

    return randomWord;
}

void ajouterMotAuFichier(const char* word, const char* filename) {
    FILE* file = fopen(filename, "a"); // ouvrir le fichier en mode append

    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s\n", word); // ajouter le nouveau mot dans le fichier

    fclose(file);
}




void afficherMessagePositifAleatoire(const char* filename) {
    const char** messages;
    const char* messages_fr[] = {
        "Cool! Lettre devinee.",
        "Impressionnant!",
        "Bonne intuition! Lettre devinee.",
        "Bravo!",
        "Bien joue!",
        "Excellent choix!",
        "Vous etes sur la bonne voie!",
        "Magnifique!",
        "Fantastique!",
        "Incroyable!",
        "Vous avez du talent!",
        "Genial!",
        "Superbe!",
        "Vous maitrisez ce jeu!",
        "Continuez comme ça!",
        "Vous etes un pro du pendu!"
    };

   const char* messages_en[] = {
    "Cool! Guessed letter.",
    "Impressive!",
    "Good intuition! Guessed letter.",
    "Well done!",
    "Great job!",
    "Excellent choice!",
    "You're on the right track!",
    "Magnificent!",
    "Fantastic!",
    "Incredible!",
    "You've got talent!",
    "Awesome!",
    "Superb!",
    "You master this game!",
    "Keep it up!",
    "You're a hangman pro!"
};

if (strcmp(filename, "dictionnaireEn.txt") == 0) {
        messages = messages_en;
    } else {
        messages = messages_fr;
    }

    // Calculate the number of messages in the array
    int numMessages;
    if (strcmp(filename, "dictionnaireEn.txt") == 0) {
        numMessages = sizeof(messages_en) / sizeof(messages_en[0]);
    } else {
        numMessages = sizeof(messages_fr) / sizeof(messages_fr[0]);
    }

    // Generate a random index to pick a message
    int randomIndex = rand() % numMessages;

    // Display the randomly chosen message
    printf("%s\n", messages[randomIndex]);
}



void afficherMessageNegatifAleatoire(const char* filename) {
    const char** messages;
    const char* messages_fr[] = {
        "Ooups! Lettre incorrecte.",
        "Oh non! Essaie encore.",
        "Pas tout à fait. Continue a chercher.",
        "Mauvaise tentative. Reessaie.",
        "Pas cette fois-ci. Trouve une autre lettre.",
        "Oh non! La lettre n'est pas la.",
        "Ce n'est pas ça. Essaie une autre lettre.",
        "Raté! Trouve la bonne lettre.",
        "Dommage! Ce n'est pas la bonne lettre.",
        "Non, essaie encore. Tu peux le faire!",
        "Oh non! La lettre n'est pas correcte.",
        "Pas de chance cette fois-ci. Réessaie.",
        "Incorrect. Essaie une autre lettre.",
        "Tu es proche, mais pas tout à fait.",
        "Malheureusement, ce n'est pas la bonne lettre.",
    };

    const char* messages_en[] = {
        "Oops! Wrong letter.",
        "Oh no! Try again.",
        "Not quite. Keep looking.",
        "Wrong guess. Try again.",
        "Not this time. Find another letter.",
        "Oh no! The letter isn't there.",
        "That's not it. Try another letter.",
        "Missed! Find the right letter.",
        "Too bad! That's not the right letter.",
        "No, try again. You can do it!",
        "Oh no! The letter is not correct.",
        "No luck this time. Try again.",
        "Incorrect. Try another letter.",
        "You're close, but not quite.",
        "Unfortunately, that's not the right letter.",
    };

    if (strcmp(filename, "dictionnaireEn.txt") == 0) {
        messages = messages_en;
    } else {
        messages = messages_fr;
    }

    // Calculate the number of messages in the array
    int numMessages;
    if (strcmp(filename, "dictionnaireEn.txt") == 0) {
        numMessages = sizeof(messages_en) / sizeof(messages_en[0]);
    } else {
        numMessages = sizeof(messages_fr) / sizeof(messages_fr[0]);
    }


    // Generate a random index to pick a message
    int randomIndex = rand() % numMessages;

    // Display the randomly chosen message
    printf("%s\n", messages[randomIndex]);
}


void dessiner_Pendu(const char* word, int incorrectAttempts, int maxAttempts) {

    //printf("%s : %d/%d echecs\n", word, incorrectAttempts, maxAttempts);
     printf("%d/%d echecs\n",incorrectAttempts, maxAttempts);

    if (incorrectAttempts == maxAttempts) {
                printf("   +---+\n");
                printf("   O   |\n");
                printf("  /|\\  |\n");
                printf("  / \\  |\n");
                printf("       |\n");
                printf("==========\n");
   }

    else if(incorrectAttempts < maxAttempts && incorrectAttempts >5) {
                printf("   +---+\n");
                printf("   O   |\n");
                printf("  /|\\  |\n");
                printf("  /    |\n");
                printf("       |\n");
                printf("==========\n");
  }

   else {

        switch (incorrectAttempts) {
            case 0:
                printf("   +---+\n");
                printf("       |\n");
                printf("       |\n");
                printf("       |\n");
                printf("       |\n");
                printf("==========\n");
                break;
            case 1:
                printf("   +---+\n");
                printf("   O   |\n");
                printf("       |\n");
                printf("       |\n");
                printf("       |\n");
                printf("==========\n");
                break;
            case 2:
                printf("   +---+\n");
                printf("   O   |\n");
                printf("   |   |\n");
                printf("       |\n");
                printf("       |\n");
                printf("==========\n");
                break;
            case 3:
                printf("   +---+\n");
                printf("   O   |\n");
                printf("  /|   |\n");
                printf("       |\n");
                printf("       |\n");
                printf("==========\n");
                break;
            case 4:
                printf("   +---+\n");
                printf("   O   |\n");
                printf("  /|\\  |\n");
                printf("       |\n");
                printf("       |\n");
                printf("==========\n");
                break;
            case 5:
                printf("   +---+\n");
                printf("   O   |\n");
                printf("  /|\\  |\n");
                printf("  /    |\n");
                printf("       |\n");
                printf("==========\n");
                break;

        }
    }
}



void PenduContreTemps(const char* file, int maxTime) {
    const int MAX_WORD_LENGTH = 50;
    char word[MAX_WORD_LENGTH];

    const char* word_alea = getRandomWordFromFile(file);
    strcpy(word, word_alea);
    free((void*)word_alea);

    char guessedLetters[MAX_WORD_LENGTH];
    guessedLetters[0] = '\0';

    int correctGuesses = 0;

    if (strcmp(file, "dictionnaireEn.txt") == 0)
    printf("Welcome to the hangman game!\n");
    else
    printf("Bienvenue dans le jeu du pendu!\n");


    // Enregistrez le temps initial
    time_t startTime = time(NULL);

    do {
       char guess;
        if (strcmp(file, "dictionnaireEn.txt") == 0)
         printf("Guess a letter : ");
        else
        printf("Proposez une lettre : ");
        scanf(" %c", &guess);

        // Vérifier si la lettre a déjà été devinée
        if (strchr(guessedLetters, guess) != NULL) {
            if (strcmp(file, "dictionnaireEn.txt") == 0)
            printf("You have already guessed this letter. Try another letter.\n");
            else
            printf("Vous avez déjà deviné cette lettre. Essayez une autre lettre.\n");
            PlaySound(TEXT("sons/incorrect.wav"), NULL, SND_ASYNC);
            afficherLettresDevinees(word, guessedLetters);
            continue;
        }

        // Ajouter la lettre aux lettres déjà devinées
        strncat(guessedLetters, &guess, 1);

        // Vérifier si la lettre proposée est dans le mot
        bool found = false;
        for (int i = 0; word[i] != '\0'; i++) {
            if (word[i] == guess) {
                found = true;
                PlaySound(TEXT("sons/correct.wav"), NULL, SND_ASYNC);
                afficherMessagePositifAleatoire(file);
                correctGuesses++;
            }
        }

        // Vérifier si toutes les lettres ont été devinées
        if (correctGuesses == strlen(word)) {
            if (strcmp(file, "dictionnaireEn.txt") == 0)
            printf("Congratulations, you have guessed the word: %s\n", word);
            else
            printf("Felicitations, vous avez devine le mot : %s\n", word);
            PlaySound(TEXT("sons/win.wav"), NULL, SND_ASYNC);
            break;
        }

        // Incrémenter le nombre d'essais incorrects si la lettre n'est pas dans le mot
        if (!found) {
            PlaySound(TEXT("sons/incorrect.wav"), NULL, SND_ASYNC);
            afficherMessageNegatifAleatoire(file);
        }

        // Calculez le temps écoulé
        time_t currentTime = time(NULL);
        int elapsedTime = difftime(currentTime, startTime);

        // Vérifie si le temps écoulé dépasse le temps maximal autorisé
        if (elapsedTime > maxTime) {
           PlaySound(TEXT("sons/failure.wav"), NULL, SND_ASYNC);
           if (strcmp(file, "dictionnaireEn.txt") == 0)
               printf("Sorry, you have exceeded the time limit. The word was: %s\n", word);
           else
               printf("Désolé, vous avez dépassé le temps limite. Le mot était : %s\n", word);
            break;
        }

        afficherLettresDevinees(word, guessedLetters);

    } while (1);
}



void PenduNbreTentatives(const char* file, int maxAttempts) {
    const int MAX_WORD_LENGTH = 50;
    char word[MAX_WORD_LENGTH];

    const char* word_alea = getRandomWordFromFile(file);
    strcpy(word, word_alea);
    free((void*)word_alea);

    char guessedLetters[MAX_WORD_LENGTH];
    guessedLetters[0] = '\0';

    int incorrectAttempts = 0;
    int correctGuesses = 0;

    if (strcmp(file, "dictionnaireEn.txt") == 0)
    printf("Welcome to the hangman game!\n");
    else
    printf("Bienvenue dans le jeu du pendu!\n");



    do {
        char guess;
       if (strcmp(file, "dictionnaireEn.txt") == 0)
         printf("Guess a letter : ");
        else
        printf("Proposez une lettre : ");
        scanf(" %c", &guess);

        // Vérifier si la lettre a déjà été devinée
        if (strchr(guessedLetters, guess) != NULL) {
            if (strcmp(file, "dictionnaireEn.txt") == 0)
            printf("You have already guessed this letter. Try another letter.\n");
            else
            printf("Vous avez deja devine cette lettre. Essayez une autre lettre.\n");
            PlaySound(TEXT("sons/incorrect.wav"), NULL, SND_ASYNC);
            afficherLettresDevinees(word, guessedLetters);
            incorrectAttempts++;
            dessiner_Pendu(word, incorrectAttempts, maxAttempts);
            continue;
        }

        // Ajouter la lettre aux lettres déjà devinées
        strncat(guessedLetters, &guess, 1);

        // Vérifier si la lettre proposée est dans le mot
        bool found = false;
        for (int i = 0; word[i] != '\0'; i++) {
            if (word[i] == guess) {
                found = true;
                PlaySound(TEXT("sons/correct.wav"), NULL, SND_ASYNC);
                afficherMessagePositifAleatoire(file);
                correctGuesses++;
            }
        }

        // Vérifier si toutes les lettres ont été devinées
        if (correctGuesses == strlen(word)) {
            if (strcmp(file, "dictionnaireEn.txt") == 0)
            printf("Congratulations, you have guessed the word: %s\n", word);
            else
            printf("Felicitations, vous avez devine le mot : %s\n", word);
            PlaySound(TEXT("sons/win.wav"), NULL, SND_ASYNC);
            break;
        }

        // Incrémenter le nombre d'essais incorrects si la lettre n'est pas dans le mot
        if (!found) {
            incorrectAttempts++;
            PlaySound(TEXT("sons/incorrect.wav"), NULL, SND_ASYNC);
            afficherMessageNegatifAleatoire(file);
            dessiner_Pendu(word, incorrectAttempts, maxAttempts);
        }

        afficherLettresDevinees(word, guessedLetters);

    } while (incorrectAttempts < maxAttempts);

    if (incorrectAttempts == maxAttempts) {
        PlaySound(TEXT("sons/failure.wav"), NULL, SND_ASYNC);
        if (strcmp(file, "dictionnaireEn.txt") == 0)
        printf("Sorry, you have reached the maximum number of attempts. The word was: %s\n", word);
        else
        printf("Desole, vous avez atteint le nombre maximum de tentatives. Le mot etait : %s\n", word);
    }
}

Joueur chargerMeilleurScore(const char* filename) {
    Joueur meilleurScore = {"personne", 0};
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        fscanf(file, "%s %d", meilleurScore.nom, &meilleurScore.score);
        fclose(file);
    }
    return meilleurScore;
}

void sauvegarderMeilleurScore(const char* filename, Joueur meilleurScore) {
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, "%s %d", meilleurScore.nom, meilleurScore.score);
        fclose(file);
    }
}

Joueur PenduMultijoueur(Joueur joueur1, Joueur joueur2, const char* filename, const char* filenameScore, int maxAttempts) {
    const int MAX_WORD_LENGTH = 50;
    char word[MAX_WORD_LENGTH];
    Joueur personne= {"personne", 0};

    const char* word_alea = getRandomWordFromFile(filename);
    strcpy(word, word_alea);
    free((void*)word_alea);

    char guessedLetters1[MAX_WORD_LENGTH];
    guessedLetters1[0] = '\0';
    char guessedLetters2[MAX_WORD_LENGTH];
    guessedLetters2[0] = '\0';

    int incorrectAttempts1 = 0;
    int incorrectAttempts2 = 0;
    int correctGuesses1 = 0;
    int correctGuesses2 = 0;

    //Joueur meilleurScore = chargerMeilleurScore(filenameScore);
    if (strcmp(filename, "dictionnaireEn.txt") == 0)
    printf("Welcome to the hangman game!\n");
    else
    printf("Bienvenue dans le jeu du pendu!\n");

    if (strcmp(filename, "dictionnaireEn.txt") == 0)
    printf("Player 1, enter your name: ");
    else
    printf("Joueur 1, entrez votre nom : ");
    scanf("%s", joueur1.nom);

    if (strcmp(filename, "dictionnaireEn.txt") == 0)
    printf("Player 2, enter your name: ");
     else

    printf("Joueur 2, entrez votre nom : ");
    scanf("%s", joueur2.nom);

    do {
        PlaySound(TEXT("sons/pendu.wav"), NULL, SND_ASYNC);
        bool found = false;
        char guess;

        // tour joueur 1
        Joueur joueur = joueur1;
        if (strcmp(filename, "dictionnaireEn.txt") == 0)
         printf("%s, Guess a letter : ", joueur1.nom);
        else
        printf("%s, proposez une lettre : ", joueur1.nom);

        scanf(" %c", &guess);

        // Vérifier si la lettre a déjà été devinée
        if (strchr(guessedLetters1, guess) != NULL) {
            if (strcmp(filename, "dictionnaireEn.txt") == 0)
            printf("You have already guessed this letter. Try another letter.\n");
            else
            printf("Vous avez deja devine cette lettre. Essayez une autre lettre.\n");
            PlaySound(TEXT("sons/incorrect.wav"), NULL, SND_ASYNC);
            afficherLettresDevinees(word, guessedLetters1);
            incorrectAttempts1++;
            dessiner_Pendu(word, incorrectAttempts1, maxAttempts);
        }

        // Ajouter la lettre aux lettres déjà devinées
        strncat(guessedLetters1, &guess, 1);

        // Vérifier si la lettre proposée est dans le mot

        for (int i = 0; word[i] != '\0'; i++) {
            if (word[i] == guess) {
                found = true;
                PlaySound(TEXT("sons/correct.wav"), NULL, SND_ASYNC);
                afficherMessagePositifAleatoire(filename);
                afficherLettresDevinees(word, guessedLetters1);
                correctGuesses1++;
                joueur1.score++;
            }
        }



        // Vérifier si toutes les lettres ont été devinées
        if (correctGuesses1 == strlen(word)) {
            if (strcmp(filename, "dictionnaireEn.txt") == 0)
            printf("Congratulations, you have guessed the word: %s\n", word);
            else
            printf("Felicitations, vous avez devine le mot : %s\n", word);
            PlaySound(TEXT("sons/win.wav"), NULL, SND_ASYNC);
            return joueur1;
        }

        // Incrémenter le nombre d'essais incorrects si la lettre n'est pas dans le mot
        if (!found) {
            incorrectAttempts1++;
            PlaySound(TEXT("sons/incorrect.wav"), NULL, SND_ASYNC);
            afficherMessageNegatifAleatoire(filename);
            afficherLettresDevinees(word, guessedLetters1);
            dessiner_Pendu(word, incorrectAttempts1, maxAttempts);
        }

        //displayWord(word, guessedLetters1);

        // Tour du joueur 2
        joueur = joueur2;
        found = false;
         if (strcmp(filename, "dictionnaireEn.txt") == 0)
         printf("%s, Guess a letter : ", joueur2.nom);
        else
        printf("%s, proposez une lettre : ", joueur2.nom);
        scanf(" %c", &guess);

        // Vérifier si la lettre a déjà été devinée
        if (strchr(guessedLetters2, guess) != NULL) {
            if (strcmp(filename, "dictionnaireEn.txt") == 0)
            printf("You have already guessed this letter. Try another letter.\n");
            else
            printf("Vous avez deja devine cette lettre. Essayez une autre lettre.\n");
            PlaySound(TEXT("sons/incorrect.wav"), NULL, SND_ASYNC);
            afficherLettresDevinees(word, guessedLetters2);
            incorrectAttempts2++;
            dessiner_Pendu(word, incorrectAttempts2, maxAttempts);
        }

        // Ajouter la lettre aux lettres déjà devinées
        strncat(guessedLetters2, &guess, 1);

        // Vérifier si la lettre proposée est dans le mot

        for (int i = 0; word[i] != '\0'; i++) {
            if (word[i] == guess) {
                found = true;
                PlaySound(TEXT("sons/correct.wav"), NULL, SND_ASYNC);
                afficherMessagePositifAleatoire(filename);
                correctGuesses2++;
                joueur2.score++;
                afficherLettresDevinees(word, guessedLetters2);
            }
        }

        // Vérifier si toutes les lettres ont été devinées
        if (correctGuesses2 == strlen(word)) {
            if (strcmp(filename, "dictionnaireEn.txt") == 0)
            printf("Congratulations, you have guessed the word: %s\n", word);
            else
            printf("Felicitations, vous avez devine le mot : %s\n", word);
            PlaySound(TEXT("sons/win.wav"), NULL, SND_ASYNC);
            return joueur2;
        }

        // Incrémenter le nombre d'essais incorrects si la lettre n'est pas dans le mot
        if (!found) {
            incorrectAttempts2++;
            PlaySound(TEXT("sons/incorrect.wav"), NULL, SND_ASYNC);
            afficherMessageNegatifAleatoire(filename);
            afficherLettresDevinees(word, guessedLetters2);
            dessiner_Pendu(word, incorrectAttempts2, maxAttempts);
        }



    } while (incorrectAttempts1 < maxAttempts && incorrectAttempts2 < maxAttempts);

    if (strcmp(filename, "dictionnaireEn.txt") == 0)
    printf("The game is over.\n");
    else
    printf("La partie est terminée.\n");

    if (joueur1.score > joueur2.score) return joueur1;
    else if (joueur1.score == 0 && joueur2.score == 0) return personne;
    else return joueur2;
}

int modeDuJeu(const char* filename){
    int optJeu;
    int maxAttempts;
    if (strcmp(filename, "dictionnaireEn.txt") == 0) {
        printf("1. Easy Mode (10 attempts)\n");
        printf("2. Hard Mode (3 attempts)\n");
        printf("3. Free Mode\n");
        printf("\nMake your choice: ");
    }
    else{
    printf("1.Mode Facile (10 tentatives)\n");
    printf("2.Mode Difficile (3 tentatives)\n");
    printf("3.Mode Libre\n");
    printf("\nFaites votre choix : ");
    }
    scanf("%d", &optJeu);
    if(optJeu == 1) return 10;
    else if(optJeu == 2) return 3;
    else
   {
    printf("Entrez le nombre de tentatives: ");
    scanf("%d", &maxAttempts);
    return maxAttempts;
   }

}
