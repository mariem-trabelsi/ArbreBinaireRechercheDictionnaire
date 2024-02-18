#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include <aspell.h>
#include <curl/curl.h>
#include <jansson.h>

#include "jeu.h"

#define BUFFER_SIZE 100

/* Structure Arbre */

struct chararbre {
    char data;
    struct chararbre *gauche;
    struct chararbre *droite;
};

typedef struct chararbre *arbre;





/*Création d'un nouveau noued*/
arbre noeud(char data, arbre g, arbre d){
  arbre res;
  res = (arbre) malloc(sizeof(struct chararbre));
  res->data = data;
  res->gauche = g;
  res->droite = d;
  return res;
}

/*-------------------------------------------------------------------------------------------------------*/
/* Gestion du dictionnaire */

int est_dans_dictionnaire(char *mot, arbre dictionnaire){
  if((dictionnaire == NULL)||(dictionnaire->data > mot[0])) return 0;
  if(mot[0] == dictionnaire->data){
    if(mot[0] == '\0') return 1;
    return est_dans_dictionnaire(mot+1, dictionnaire->gauche);
  }
  return est_dans_dictionnaire(mot, dictionnaire->droite);
}

arbre construire_arbre_mot(char *mot){
  if(mot[0] == '\0') return noeud('\0',NULL,NULL);
  return noeud(mot[0],construire_arbre_mot(mot+1),NULL);
}

arbre ajoute_dans_dictionnaire(char *mot, arbre dico){
  if(dico == NULL) return construire_arbre_mot(mot);
  if(mot[0] == '\0') return (dico->data == '\0')?dico:noeud('\0',NULL,dico);
  /* Car '\0' est le plus petit char possible */
  if(mot[0] < dico->data) return noeud(mot[0], construire_arbre_mot(mot+1), dico);
  if(mot[0] == dico->data){
    dico->gauche = ajoute_dans_dictionnaire(mot+1, dico->gauche);
    return dico;
  }
  dico->droite = ajoute_dans_dictionnaire(mot, dico->droite);
  return dico;
}

arbre supprime_dans_dictionnaire_recursif(char *mot, arbre dic) {
  if (dic == NULL) {
        return NULL;
  }
  if (mot[0] < dic->data) {
        dic->gauche = supprime_dans_dictionnaire_recursif(mot, dic->gauche);
  }
  else if (mot[0] > dic->data) {
        dic->droite = supprime_dans_dictionnaire_recursif(mot, dic->droite);
  }
  // Si le premier caractère du mot est égal à la valeur du nœud actuel.
  else {
        if (mot[0] == '\0') {
          free(dic);
          return NULL;
        }
        dic->gauche = supprime_dans_dictionnaire_recursif(mot + 1, dic->gauche);
      }

  return dic;
}

arbre supprime_dans_dictionnaire(char *mot, arbre dic, const char* file) {
  if (dic == NULL) {
       if (strcmp(file, "dictionnaireEn.txt") == 0)
       printf("The word \"%s\" does not exist in the dictionary.\n", mot);
       else
       printf("Le mot \"%s\" n'existe pas dans le dictionnaire.\n", mot);
    return NULL;
  }

  if (strcmp(mot, "") == 0) {
    if (strcmp(file, "dictionnaireEn.txt") == 0)
    printf("Deletion impossible. The word to be removed is empty.\n");
    else
    printf("Suppression impossible. Le mot à supprimer est vide.\n");
    return dic;
  }

  if (!est_dans_dictionnaire(mot, dic)) {
    if (strcmp(file, "dictionnaireEn.txt") == 0)
    printf("The word \"%s\" does not exist in the dictionary.\n", mot);
    else
    printf("Le mot \"%s\" n'existe pas dans le dictionnaire.\n", mot);
    return dic;
  }

  dic = supprime_dans_dictionnaire_recursif(mot, dic);
  if (strcmp(file, "dictionnaireEn.txt") == 0)
  printf("The word \"%s\" has been successfully removed from the dictionary.\n", mot);
  else
  printf("Le mot \"%s\" a ete supprime du dictionnaire avec succes.\n", mot);

  return dic;
}



void SuppresionDepuisFichier(const char* word, const char* filename) {
    FILE* file = fopen(filename, "r"); // ouvrir fichier mode lecture

    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    FILE* tempFile = fopen("temp.txt", "w"); // ouvrir fichier mode écriture

    if (tempFile == NULL) {
        fprintf(stderr, "Error creating temporary file.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    // tableau de caractères pour stocker une ligne du fichier
    char buffer[100];

    //Copier les lignes du fichier original vers le fichier temporaire, en excluant le mot spécifié.
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
    // supprimer les caractères de retour à la ligne et les remplacer par le caractère nul ('\0')
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, word) != 0) {
            fprintf(tempFile, "%s\n", buffer);
        }
    }
    fclose(file);
    fclose(tempFile);

    remove(filename);

    // renommer le fichier temporaire pour remplacer le fichier original
    if (rename("temp.txt", filename) != 0) {
        fprintf(stderr, "Error renaming temporary file.\n");
        exit(EXIT_FAILURE);
    }
}

/* Création du dictionnaire */
int lireLigne(FILE *flux, char *buffer){//buffer de taille BUFFER_SIZE
  int c,count;
  //char *fgets(char *buffer, int size, FILE *stream);
  for(count=0;(c=fgetc(flux))!=EOF && count<BUFFER_SIZE; count++){
    if(c=='\n') {
      buffer[count]='\0';
      return 1;
    }
    else if (c=='\r') count--;
    else buffer[count]=c;
  }
  if(count<BUFFER_SIZE){//une ligne finale, non terminée par \n
    buffer[count]='\0';
    return 0;
  }
  buffer[BUFFER_SIZE-1]='\0';
  fprintf(stderr,"Le mot commençant par %s est trop long\n",buffer);
  return 1;
}

arbre cree_dictionnaire(const char *fichier){
  arbre dic =NULL;
  FILE *file;
  char buffer[BUFFER_SIZE];
  file = fopen(fichier,"r");
  while(lireLigne(file, buffer)) dic = ajoute_dans_dictionnaire(buffer,dic);

  if(buffer[0]!='\0')  dic = ajoute_dans_dictionnaire(buffer,dic);
  fclose(file);
  return dic;
}

void affiche_arbre_recursif(arbre dic, int niveau) {
  if (dic != NULL) {
    affiche_arbre_recursif(dic->droite, niveau + 1);
    for (int i = 0; i < niveau; i++) {
      printf("    "); // Ajoute une indentation
    }
    printf("%c\n", dic->data);
    affiche_arbre_recursif(dic->gauche, niveau + 1);
  }
}

void affiche_arbre(arbre dic) {
  printf("Affichage du dictionaire de mots sous une forme arborescente:\n");
  affiche_arbre_recursif(dic, 0);
}


/*----------------------------------------------------------------------------------------------------------*/
/*Fonctions supplémentaires*/

const char* choisir_dictionnaire() {
    int choix;

    printf("Veuillez choisir le dictionnaire :\n");
    printf("1. Dictionnaire français\n");
    printf("2. Dictionnaire anglais\n");
    printf("Votre choix : ");

    scanf("%d", &choix);

    if (choix == 1) {
        return "dictionnaireFr.txt";
    } else if (choix == 2) {
        return "dictionnaireEn.txt";
    } else {
        printf("Choix invalide.\n");
        return NULL;
    }
}


void verifierOrthographe(const char *mot, const char* filename) {
    AspellConfig *config;
    AspellCanHaveError *possible_err;
    AspellSpeller *speller;

    // Create a new AspellConfig object
    config = new_aspell_config();

    if (filename == "dictionnaireFr.txt")
    // Set the language to French
    aspell_config_replace(config, "lang", "fr");

   // Set the language to English (American)
   else
    aspell_config_replace(config, "lang", "en");

    // Create a new AspellSpeller object
    possible_err = new_aspell_speller(config);
    if (aspell_error_number(possible_err) != 0) {
        printf("Error creating speller: %s\n", aspell_error_message(possible_err));
        return 1;
    }
    speller = to_aspell_speller(possible_err);

    // Ask the user to enter a word
    if (strcmp(filename, "dictionnaireEn.txt") == 0)
    printf("Enter a word: ");
    else
    printf("Entrez un mot: ");
    scanf("%ms", &mot);

  if (!aspell_speller_check(speller, mot, -1)) {
        if (strcmp(filename, "dictionnaireEn.txt") == 0)
        printf("%s is incorrect. Suggestions:\n", mot);
        else
        printf("%s est incorrect. Suggestions:\n", mot);
        const AspellWordList *suggestions = aspell_speller_suggest(speller, mot, -1);
        AspellStringEnumeration *elements = aspell_word_list_elements(suggestions);
        const char *suggestion;
        while ((suggestion = aspell_string_enumeration_next(elements)) != NULL) {
            printf("- %s\n", suggestion);
        }
        delete_aspell_string_enumeration(elements);
    } else {
        if (strcmp(filename, "dictionnaireEn.txt") == 0)
        printf("%s is correct.\n", mot);
        else
        printf("%s est correct.\n", mot);
    }

    // Clean up
    delete_aspell_speller(speller);
}


/* UI pour afficher le dictionnaire arborescent*/
void affiche_arbre_ui(GtkWidget *container, arbre dico, int niveau) {
    if (dico != NULL) {
        // Affiche le sous-arbre droit
        affiche_arbre_ui(container, dico->droite, niveau + 1);

        // Crée un conteneur pour organiser le nœud et la ligne
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_container_add(GTK_CONTAINER(container), hbox);

        // Crée une étiquette pour le nœud
        gchar label_text[2] = { dico->data, '\0' };
        GtkWidget *label = gtk_label_new(label_text);

        // Applique un style CSS au label (vous pouvez personnaliser les couleurs selon vos besoins)
        gtk_widget_set_name(label, "node-label");

        // Crée une ligne verticale
        GtkWidget *line = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

        // Applique un style CSS à la ligne (vous pouvez personnaliser les couleurs selon vos besoins)
        gtk_widget_set_name(line, "node-line");

        // Ajoute l'étiquette et la ligne au conteneur
        gtk_container_add(GTK_CONTAINER(hbox), label);
        gtk_container_add(GTK_CONTAINER(hbox), line);

        // Indentation visuelle
        gtk_widget_set_margin_start(hbox, niveau * 30);

        // Affiche le sous-arbre gauche
        affiche_arbre_ui(container, dico->gauche, niveau + 1);
    }
}


// Callback function to write response data
size_t fonction_callback_ecriture(void *ptr, size_t size, size_t nmemb, char *data) {
    strcat(data, ptr);
    return size * nmemb;
}
// Fonction pour effectuer la requête HTTP, analyser la réponse JSON et imprimer les définitions
void rechercher_definition_mot() {
    char input_word[100];
    char final_url[300] = "https://api.dictionaryapi.dev/api/v2/entries/en/";
    char response[4096] = "";

    // Initialiser les données du mot
    printf("Enter the word you want to look up: ");
    scanf("%s", input_word);
    strcat(final_url, input_word);

    // Effectuer la requête HTTP
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, final_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fonction_callback_ecriture);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Error during HTTP request: %s\n", curl_easy_strerror(res));
            exit(1);
        }
    }

    // Parse JSON response
    json_t *root;
    json_error_t error;

    root = json_loads(response, 0, &error);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        exit(1);
    }

    // Extract and print the word and its definitions
    json_t *word = json_array_get(root, 0);
    printf("Word: %s\n", json_string_value(json_object_get(word, "word")));

    json_t *meanings = json_object_get(word, "meanings");
    size_t i;
    json_t *meaning;
    json_array_foreach(meanings, i, meaning) {
        printf("Part of Speech: %s\n", json_string_value(json_object_get(meaning, "partOfSpeech")));
        json_t *definitions = json_object_get(meaning, "definitions");
        json_t *definition = json_array_get(definitions, 0);
        printf("Definition: %s\n", json_string_value(json_object_get(definition, "definition")));
    }

    // Cleanup
    json_decref(root);
}


int main(int argc, char *argv[]) {

  setlocale(LC_ALL, "UTF-8");
  gtk_init(&argc, &argv);
  const char* filename = choisir_dictionnaire();
  const char* filenameScore = "meilleur_score.txt";
  arbre dictionnaire = cree_dictionnaire(filename);

    // Crée une fenêtre GTK
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Affichage de l'arbre");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Crée un conteneur pour organiser les éléments de l'arbre
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    // Crée un conteneur de boîte verticale pour les étiquettes
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrolled_window), vbox);


     if (strcmp(filename, "dictionnaireEn.txt") == 0) {
      printf("\nLoading the dictionary ");
    } else
    printf("\nChargement du dictionnaire ");
    printf(".");
    fflush(stdout);
    usleep(1000000);

    printf(".");
    fflush(stdout);
    usleep(100000); // Pause pendant 1 seconde

    printf(".");
    fflush(stdout);
    usleep(100000);
    printf("\n");


// Déterminer la langue du menu
    int menuLangue = 0; // 0 pour le français, 1 pour l'anglais
    if (strcmp(filename, "dictionnaireEn.txt") == 0) {
        menuLangue = 1;
    }



    int choixMenu;
    while (1) {
        if (menuLangue == 1) {
        printf("_______________________________________________________________________________________________________________________________________");
        printf("\n1.Play   ");
        printf("2.Dispaly tree    ");
        printf("3.Search a word    ");
        printf("4.Add a word   ");
        printf("5.Delete a word    ");
        printf("6.Check Spelling    ");
        printf("7.Look up a word   ");
        printf("8.Quit\n");
        printf("_______________________________________________________________________________________________________________________________________");
        printf("\nMake your choice : ");
        } else {
        printf("____________________________________________________________________________________________________________________________________________________________");
        printf("\n1.Jouer   ");
        printf("2.Afficher l'arbre    ");
        printf("3.Rechercher un mot    ");
        printf("4.Ajouter un mot    ");
        printf("5.Supprimer un mot    ");
        printf("6.Verifier Orthographe    ");
        printf("7.Apprendre l'anglais   ");
        printf("8.Quitter\n");
        printf("____________________________________________________________________________________________________________________________________________________________");
        printf("\nFaites votre choix : ");
        }
        scanf("%d", &choixMenu);
        switch (choixMenu) {
              case 1: {
                int jeuOptions;
                if (strcmp(filename, "dictionnaireEn.txt") == 0) {
                printf("Good choice! And with which option?\n");
                printf("1. Number of attempts\t");
                printf("2. Against the clock\t");
                printf("3. Multiplayer mode\n");
                printf("Type your choice: \t");
               }else{
                printf("Bon choix! Et avec quelle option ?\n");
                printf("1. En nombre de tentatives\t");
                printf("2. Contre le temps\t");
                printf("3. Mode Multijoueur\n");
                printf("Tapez votre choix: \t");
                }
              scanf("%d", &jeuOptions);
              if(jeuOptions == 1){
               int maxAttempts = modeDuJeu(filename);
               PenduNbreTentatives(filename, maxAttempts);
               break;
               }
             else if(jeuOptions == 2){
                int maxTime;
                if (strcmp(filename, "dictionnaireEn.txt") == 0)
                printf("Enter the maximum duration of the game in seconds: ");
                else
                printf("Entrez la duree maximale du jeu en secondes : ");
                scanf("%d", &maxTime);
                PenduContreTemps(filename, maxTime);
                break;
             }
            else {
                Joueur joueur1 = {"Joueur 1", 0};
                Joueur joueur2 = {"Joueur 2", 0};
                Joueur meilleurScore = chargerMeilleurScore(filenameScore);
                if (strcmp(filename, "dictionnaireEn.txt") == 0)
                printf("\nThe current high score is %d held by %s\n\n", meilleurScore.score, meilleurScore.nom);
                else
                printf("\nLe meilleur score actuel est de %d detenu par %s.\n\n", meilleurScore.score, meilleurScore.nom);

                int maxAttempts = modeDuJeu(filename);

                Joueur gagnant = PenduMultijoueur(joueur1, joueur2, filename, filenameScore, maxAttempts);
                if (strcmp(filename, "dictionnaireEn.txt") == 0)
                printf("The winning player is: %s with a score of %d\n", gagnant.nom, gagnant.score);
                else
                printf("Le joueur gagnant est : %s avec un score de %d\n", gagnant.nom, gagnant.score);
                if (gagnant.score > meilleurScore.score) {
                meilleurScore = gagnant;
                sauvegarderMeilleurScore(filenameScore, meilleurScore);
                 if (strcmp(filename, "dictionnaireEn.txt") == 0)
                 printf("%s has beaten the high score with a score of %d !\n", gagnant.nom, gagnant.score);
                 else
                 printf("%s a battu le meilleur score avec un score de %d !\n", gagnant.nom, gagnant.score);
              }
               break;
            }
            }
            case 2:
                affiche_arbre(dictionnaire);
                affiche_arbre_ui(vbox, dictionnaire, 0);
                gtk_widget_show_all(window);
                gtk_main();
                break;
            case 3: {
                char motARechercher[50];
                if (strcmp(filename, "dictionnaireEn.txt") == 0)
                 printf("Enter the word to search for: ");
                else
                printf("Entrez le mot a rechercher : ");
                scanf("%s", motARechercher);
                int resultatRecherche = est_dans_dictionnaire(motARechercher, dictionnaire);
                if (resultatRecherche) {
                    if (strcmp(filename, "dictionnaireEn.txt") == 0)
                      printf("The word '%s' has been found in the tree.\n", motARechercher);
                    else
                      printf("Le mot '%s' a ete trouve dans l'arbre.\n", motARechercher);
                } else {
                    if (strcmp(filename, "dictionnaireEn.txt") == 0)
                      printf("The word '%s' has not been found in the tree.\n", motARechercher);
                    else
                      printf("Le mot '%s' n'a pas ete trouve dans l'arbre.\n", motARechercher);
                }
               // free((char *) motARechercher);
                break;
              }


            case 4: {
                char word[50];
                if (strcmp(filename, "dictionnaireEn.txt") == 0)
                printf("The word to add to the dictionary:\n");
                else
                printf("Le mot a ajouter dans le dictionnaire :\n");
                scanf("%s", word);
                dictionnaire = ajoute_dans_dictionnaire(word,dictionnaire);
                ajouterMotAuFichier(word, filename);
                //free((char *) word);
                break;
            }
           case 5: {
               char motASupprimer[50];
               if(strcmp(filename, "dictionnaireEn.txt") == 0)
                printf("Enter the word to delete: ");
               else
               printf("Entrez le mot a supprimer : ");
               scanf("%s", motASupprimer);
               dictionnaire = supprime_dans_dictionnaire(motASupprimer, dictionnaire, filename);
               SuppresionDepuisFichier(motASupprimer, filename);
               dictionnaire = cree_dictionnaire(filename);
               //free((char *) motASupprimer);
               break;
           }
           case 6: {
            const char *mot;
            // Vérifier l'orthographe du mot
            verifierOrthographe(mot, filename);
            // Libérer la mémoire
            free((char *) mot);
            break;
           }
           case 7: {
            rechercher_definition_mot();
            break;
           }
           case 8:
                if(strcmp(filename, "dictionnaireEn.txt") == 0)
                printf("Goodbye!\n");
                else
                printf("Au revoir !\n");
                return 0;
           default:
                if(strcmp(filename, "dictionnaireEn.txt") == 0)
                printf("Invalid choice.\n");
                else
                printf("Choix invalide.\n");
        }
    }

    return 0;
}




