#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int nlig = 0; // compteur de numero de ligne
int ncar = 0; // compteur de numero de caractere
int j=1; //compteur d'erreurs lexicales
int k=1; //compteur d'erreurs syntaxiques
int l=1; //compteur d'erreurs semantiques
int err_lex_existe =0;// verification s'il y a au moins une erreur lexicale dans l'algorithme
char semicolon[2] =";\0"; // variable qui contient ';' car on va l'utiliser plusieurs fois
char deux_pts[2] =":\0"; //variable qui contient ':' car on va l'utiliser plusieurs fois
int existe_si =0;
int existe_affectation=0;
int compteur_si=0;
int compteur_finsi=0;
int verif_algo=0; // verifier l'existance du mot Algorithme au debut
int verif_declaration=0; // verifier l'existance du mot Declaration au debut
int verif_debut=0; // verifier l'existance du mot Debut au debut
int verif_fin=0; // verifier l'existance du mot Fin à la fin
int verif_apostrophe =0; //verifier " lors de l'écriture d'une chaine
int error =0;
int variables_declarees[26] = {0}; // Tableau pour stocker les variables déclarées (analyse sémantique) (par exemple, de A à Z)
int variables_dec_type[26] = {0}; // Tableau pour stocker les variables déclarées (analyse sémantique) (par exemple, de A à Z)
int variables_initialisees[26] = {0}; // Tableau pour vérifier les variables initialisées
int variables_ini_type[26] = {0}; // Tableau pour stocker les types des variables initialisées
int pass_si = 0; //pass: si on analyse syntaxiquement des instructions dedans si ou bien dedans sinon
int liste_finsi = 0; // marquant pour la liste chainée de si
typedef struct liste {
	char Tab[10];
	struct liste *next;
} mot;
mot* debut_copie = NULL;
void Analyser_lix(char Tab_Ligne[], FILE*e) {
	int verification_lex = 0; //variable qui indique s'il y a des erreurs lexicales
	int i = 0; // compteur pour le remplissage du tableau
	char c;
	char Tab_Dic[10]; // Augmentation de la taille du tableau pour stocker les mots du dictionnaire

	FILE *d = fopen("dictionnaire.txt", "r");
	if (d == NULL) {
		printf("Problème dans le fichier dictionnaire.\n");
		return; // Sortie de la fonction en cas d'échec d'ouverture du fichier
	}

	// Boucle pour parcourir le fichier dictionnaire et comparer chaque mot avec Tab_Ligne
	while ((c = fgetc(d)) != EOF) {
		if (c == '\n') {
			Tab_Dic[i] = '\0'; // Ajouter le caractère de fin de chaîne
			if (strcmp(Tab_Ligne, Tab_Dic) == 0) {
				verification_lex = 1;
				break; // Sortir de la boucle dès qu'un mot correspond
			}
			i = 0; // Réinitialiser l'index pour le prochain mot du dictionnaire
		} else {
			Tab_Dic[i++] = c;
		}
	}

	if (verification_lex == 0) {
		err_lex_existe = 1;
		if (e == NULL) {
			printf("Impossible d'ouvrir le fichier des erreurs.\n");
			return;
		}
		fprintf(e, "Erreur lexicale numero: %d dans la ligne: %d, mot errone : %s\n\n",j, nlig, Tab_Ligne);
		j++;
	}
	fclose(d); // Fermeture du fichier dictionnaire
}

void InsererFin(mot **debut, const char Tab_Ligne[10]) {
	mot* nouveau = (mot *)malloc(sizeof(mot));
	strcpy(nouveau->Tab, Tab_Ligne);
	nouveau->next = NULL;
	if (*debut == NULL) {
		*debut = nouveau;
	} else {
		mot *temp = *debut;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = nouveau;
	}
}

void Afficher(mot *debut) {
	mot *temp = debut;
	printf("\n nouvelle ligne syntaxique:\n");
	while (temp != NULL) {
		printf("%s ", temp->Tab);
		temp = temp->next;
	}
	printf("\n");
}

void Analyse_syntaxique(mot* debut, FILE* s,int nlig) {

	mot* temp = debut;

	if (strcmp(temp->Tab, "Algorithme") != 0 && verif_algo ==0) {
		fprintf(s, "Erreur syntaxique numero %d dans la ligne numero %d, il faut commencer par le mot \"Algorithme\" a la place de le mot '%s'\n", k, nlig, temp->Tab);
		k++;
		verif_algo = 1;
		error =1;
	} else if(strcmp(temp->Tab, "Algorithme") == 0) {
		verif_algo = 1;
	}

	else if (strcmp(temp->Tab, "debut") != 0 && verif_debut ==0 && error ==0 ) {
		fprintf(s, "Erreur syntaxique numero %d dans la ligne numero %d, il faut commencer par le mot \"debut\" a la place de le mot '%s'\n", k, nlig, temp->Tab);
		k++;
		verif_debut = 1;
		error =1;
	} else if(strcmp(temp->Tab, "debut") == 0) {
		verif_debut = 1;
	}

	else if (strcmp(temp->Tab, "Declaration") != 0 && verif_declaration ==0 && error ==0) {
		fprintf(s, "Erreur syntaxique numero %d dans la ligne numero %d, il faut commencer par le mot \"Declaration\" a la place de le mot '%s'\n", k, nlig, temp->Tab);
		k++;
		verif_declaration = 1;
		error =1;
	} else if(strcmp(temp->Tab, "Declaration") == 0) {
		verif_declaration = 1;
	}


// verifier si fin. existe

	temp=debut;
	while(temp->next->next !=NULL) {
		temp = temp->next;
	}
	if (strcmp(temp->Tab,"fin")==0 || strcmp(temp->Tab,"fin.\0")==0) verif_fin =1;

	temp=debut;
	while(temp->next !=NULL) {
		if(strcmp(temp->Tab,"=\0")==0) existe_affectation = 1;
		temp = temp->next;
	}

	//verifier si les mots se terminent par un ';'
	temp =debut;
	while(temp->next!=NULL) {
		temp = temp->next;
	}
	if(strcmp(temp->Tab,semicolon)!=0) {
		fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, ';' est attendu apres le mot '%s'\n",k,nlig,temp->Tab);
		k++;
	}



	temp=debut;
	if(strcmp(temp->Tab,"si") !=0) {
		if(pass_si ==0) {
			while(temp->next!=NULL) {
				temp=temp->next;
				if((strcmp(temp->Tab,"variable") == 0)||(strcmp(temp->Tab,"var") == 0)) {
					fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, la declaration de chaque variable doit se faire sur une nouvelle ligne. erreur: le mot '%s'\n",k,nlig,temp->Tab);
					k++;
				}
				if(strcmp(temp->Tab,"lire") == 0) {
					fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, chaque lecture doit se faire sur une nouvelle ligne. erreur: le mot '%s'\n",k,nlig,temp->Tab);
					k++;
				}
				if(strcmp(temp->Tab,"ecrire") == 0) {
					fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, chaque ecriture doit se faire sur une nouvelle ligne. erreur: le mot '%s'\n",k,nlig,temp->Tab);
					k++;
				}
			}
		}
	}
	temp = debut;
	if((strcmp(temp->Tab,"variable") == 0)||(strcmp(temp->Tab,"var") == 0)) {
		temp=temp->next;
		if(strlen(temp->Tab)!=1) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, l'identificateur doit comporter un seul caractere. mot errone:'%s'\n",k,nlig,temp->Tab);
			k++;
		}
		temp=temp->next;
		if((strcmp(temp->Tab,deux_pts) != 0)) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, ':' est attendue. mot errone:'%s'\n",k,nlig,temp->Tab);
			k++;
		}
		temp=temp->next;
		if((strcmp(temp->Tab,"entier") == 0) || (strcmp(temp->Tab,"reel") == 0) || (strcmp(temp->Tab,"chaine") == 0) ) {}
		else {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, type de variable inconnu. mot errone:'%s'\n",k,nlig,temp->Tab);
			k++;
		}
	}
	if(strcmp(temp->Tab,"lire") == 0) {
		temp=debut;
		temp=temp->next;
		if((strcmp(temp->Tab,"(\0") != 0)) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, '(' est attendue. mot errone:'%s'\n",k,nlig,temp->Tab);
			k++;
		}
		temp=temp->next;
		temp=temp->next;
		if((strcmp(temp->Tab,")\0") != 0)) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, ')' est attendue. mot errone:'%s' (la lecture doit se faire variable par variable)\n",k,nlig,temp->Tab);
			k++;
		}
	}
	temp=debut;
	if(strcmp(temp->Tab,"ecrire") == 0) {
		temp=temp->next;
		if((strcmp(temp->Tab,"(\0") != 0)) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, '(' est attendue. mot errone:'%s'\n",k,nlig,temp->Tab);
			k++;
		} else if(strcmp(temp->next->Tab,"\"\0") == 0) {
			temp=temp->next;
			while(temp->next !=NULL) {
				temp=temp->next;
				if(strcmp(temp->Tab,"\"\0") == 0) {
					verif_apostrophe =1;
					temp=temp->next;
					if(strcmp(temp->Tab,")\0") != 0) {
						fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, ')' est attendue. mot errone:'%s'\n",k,nlig,temp->Tab);
						k++;
					}
				}
			}
			if(verif_apostrophe ==0) {
				fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, \" est attendue. mot errone:'%s'\n",k,nlig,temp->Tab);
				k++;
			}
		} else if(!(temp->next->Tab[0] >= 'a' && temp->next->Tab[0] <= 'z')) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, une variable (a - z) est attendue. mot errone:'%s'\n",k,nlig,temp->Tab);
			k++;
		} else if(strcmp(temp->next->next->Tab,")\0")!=0) {
			temp=temp->next;
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, ')' est attendue. mot errone:'%s' (l'ecriture doit se faire variable par variable)\n",k,nlig,temp->Tab);
			k++;
		}
	}
	//analyse de si sinon

	temp =debut;
	if(strcmp(temp->Tab,"si")==0) {
		mot* tempfinsi = debut;
		while(tempfinsi->next->next !=NULL) {
			tempfinsi = tempfinsi->next;
		}
		if(strcmp(tempfinsi->Tab,"finsi")!=0) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, \"finsi\" est attendue apres l'instruction \"si\". mot errone:'%s'\n",k,nlig,tempfinsi->Tab);
			k++;
		}
		mot* tempsi = NULL;
		compteur_si++;
		existe_si=1;
		temp=temp->next;
		if(strcmp(temp->Tab,"(\0")==0) {
			int verifparenthese = 0;
			temp=temp->next;
			mot* analyse_si = temp;
			mot* analyse_si_r = temp;
			while(strcmp(temp->Tab,")\0")!=0) {
				temp=temp->next;
				if(strcmp(temp->Tab,")\0")==0) {
					verifparenthese =1;
					tempsi = temp;
				}
			}

			if(verifparenthese == 1) {
				while(analyse_si->next != tempsi) {
					analyse_si= analyse_si->next;
				}
				analyse_si->next = NULL;
				analyse_si = analyse_si_r;
				InsererFin(&analyse_si,";\0");
				Analyse_syntaxique(analyse_si,s,nlig);
				temp = tempsi;
				temp=temp->next;
				if(strcmp(temp->Tab,"alors")!=0 && strcmp(temp->Tab,"si")!=0) {
					fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, 'alors' est attendue apres\"si\".\nmot errone:'%s'\n",k,nlig,temp->Tab);
					k++;
				} else {
					while(temp->next->next !=NULL) {
						temp=temp->next;
						if(strcmp(temp->Tab,";\0")==0) nlig++;
						pass_si = 1;
						Analyse_syntaxique(temp,s,nlig);
						pass_si = 0;
					}
				}
			} else {
				fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, ')' est attendue apres l'instruction \"si\". mot errone:'%s'\n",k,nlig,temp->Tab);
				k++;
			}
		} else {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, '(' est attendue apres l'instruction \"si\". mot errone:'%s'\n",k,nlig,temp->Tab);
			k++;
		}
		if(strcmp(temp->Tab,"sinon")==0 && strcmp(temp->next->Tab,"si")!=0 ) {
			temp=temp->next;
			pass_si = 1;
			Analyse_syntaxique(temp,s,nlig);
			pass_si = 0;
		}

	}
	// treat sinon si without previous si
	temp=debut;
	if(strcmp(temp->Tab,"sinon")==0 && strcmp(temp->next->Tab,"si")==0) {
		if(existe_si ==0) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, \"sinon si\" est utilisee sans l'instruction \"si\". mots errones:'%s %s'\n",k,nlig,temp->Tab,temp->next->Tab);
			k++;
		}
	}
	// traiter sinon doit etre après une si
	else if((strcmp(temp->Tab,"sinon")==0 && strcmp(temp->next->Tab,"si")!=0)) {
		if(existe_si ==0) {
			fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, \"sinon\" est utilisee sans l'instruction \"si\". mots errones:'%s %s'\n",k,nlig,temp->Tab,temp->next->Tab);
			k++;
		} else {
			//temp=temp->next;
			//Analyse_syntaxique(temp,s,nlig);
		}
	}
	//analyse de l'affectation;
	mot* tempdebut;
	temp=debut->next;
	tempdebut= debut;
	if (existe_affectation == 1) {
		existe_affectation = 0;
		char variable = tempdebut->Tab[0];
		if (!(variable >= 'a' && variable <= 'z')) {
			fprintf(s, "Erreur syntaxique numero %d en ligne numero %d : Affectation non validee, membre gauche de l'operateur'\n",k,nlig, variable);
			k++;
		} else if(strcmp(tempdebut->next->Tab,"=\0")!=0) {
			fprintf(s, "Erreur syntaxique numero %d en ligne numero %d : Affectation non validee, \"=\" est attendue a la place de %s\n",k,nlig, tempdebut->Tab);
			k++;
		} else {
			int err;
			err=0;
			temp = temp->next;
			if (!((temp->Tab[0] >= 'a' && temp->Tab[0] <= 'z') || (temp->Tab[0] >= '1' && temp->Tab[0] <= '9' ))) {
				err=1;
			}

			if(err == 1) {
				fprintf(s, "Erreur syntaxique numero %d en ligne numero %d : Affectation non validee, membre droit de l'operateur'\n",k,nlig, variable);
				k++;
			}
		}

	}
}
//fin de l'analyse syntaxique


void Analyse_semantique(mot* debut, FILE* sem, int nlig) {
	mot* temp = debut;
	mot* tempdebut = debut;

	//1 = entier 2=reel 3= chaine
	/*void stocker(mot *debut) {
	if (debut == NULL) {
	    printf("\n");
	    return;
	}
	stocker(debut->next);
	printf("%s ", debut->Tab);
	}*/
	// Vérification des déclarations de variables
	if (strcmp(temp->Tab, "variable") == 0 || strcmp(temp->Tab, "var") == 0) {
		temp=temp->next;
		char variable = temp->Tab[0];
		if (variable >= 'a' && variable <= 'z') {
			variables_declarees[variable - 'a'] = 1; // Marquer la variable comme déclarée
			temp = temp->next;
			temp = temp->next;
			if(strcmp(temp->Tab,"entier")==0) {
				variables_dec_type[variable - 'a'] = 1; // Marquer la variable comme déclarée entier
			}
			if(strcmp(temp->Tab,"reel")==0) {
				variables_dec_type[variable - 'a'] = 2; // Marquer la variable comme déclarée reel
			}
			if(strcmp(temp->Tab,"chaine")==0) {
				variables_dec_type[variable - 'a'] = 3; // Marquer la variable comme déclarée chaine
			}
		}
	}

	temp =debut->next;
	// Vérification de l'initialisation des variables
	if (strcmp(temp->Tab, "=\0") == 0) {
		char variable = tempdebut->Tab[0];
		if (variable >= 'a' && variable <= 'z') {
			variables_initialisees[variable - 'a'] = 1; // Marquer la variable comme initialisée
		}
	}

	// Vérification d'utilisation de variables non déclarées
	temp=debut->next;

	if (strcmp(temp->Tab, "=\0") == 0) {
		char variable = tempdebut->Tab[0];
		if (variable >= 'a' && variable <= 'z' && !variables_declarees[variable - 'a']) {
			fprintf(sem, "Erreur semantique numero %d en ligne numero %d : Affectation de variable '%c' non declaree\n",l,nlig, variable);
			l++;
		}
	}
	temp=debut->next;

	if (strcmp(temp->Tab, "(\0") == 0) {
		char variable = temp->next->Tab[0];
		if ((variable >= 'a' && variable <= 'z') && !variables_declarees[variable - 'a']) {
			fprintf(sem, "Erreur semantique numero %d en ligne numero %d : utilisation de variable '%c' non declaree\n",l,nlig, variable);
			l++;
		}
	}
	//Fin de Vérification d'utilisation de variables non déclarées
	// Vérification des variables non initialisées
	int c;
	if((strcmp(tempdebut->Tab,"ecrire")==0 && strcmp(tempdebut->next->next->Tab,"\"\0")!=0) || strcmp(tempdebut->Tab,"si")==0 || strcmp(tempdebut->next->Tab,"si")==0 ) {
		if(strcmp(tempdebut->next->Tab,"si")==0) c = tempdebut->next->next->next->Tab[0];
		else c = tempdebut->next->next->Tab[0];
		if (variables_declarees[c-'a'] && !variables_initialisees[c-'a']) {
			fprintf(sem, "Erreur semantique numero %d en ligne numero %d : Variable '%c' declaree mais non initialisee\n",l,nlig, c);
			l++;
		}
	}
//ici nouveau cas
}


int main() {
	FILE *e = fopen("erreurs_lexicales.txt", "w"); // ouverture du fichier erreurs lexicales
	FILE *s = fopen("erreurs_syntaxiques.txt", "w"); // ouverture du fichier erreurs syntaxiques
	FILE *sem = fopen("erreurs_semantiques.txt", "w"); // ouverture du fichier erreurs semantiques
	mot *debut = NULL;
	// Ouvrir le fichier algorithme
	FILE *f = fopen("algorithme.txt", "r");
	//vérifier si le fichier existe
	if (f == NULL) {
		printf("Impossible d'ouvrir le fichier.\n");
		return 1;
	}
	char c; // lecture de chaque caractere
	char Tab_Ligne[10];
	int i=0; //tableau des lignes avec son compteur i
	int espace=0; //variable pour résoudre le cas des espaces consecutifs multiples
	//parcourir les caractères
	//mot* resultat= (mot*)malloc(sizeof(mot));
	while ((c = fgetc(f)) != EOF) {
		if(c == '\n') {
			nlig++;
			i = 0;
			ncar=0;
		}
		if (c == ' ' || c=='\n') {
			if (espace == 0) {
				c = '\n';
				i = 0;
				Analyser_lix(Tab_Ligne,e);
				if(strcmp(Tab_Ligne,semicolon)!=0) {
					InsererFin(&debut,Tab_Ligne);
					if(strcmp(Tab_Ligne,"finsi")==0) {
						liste_finsi = 1;
						compteur_finsi++;
						nlig--;
					}
				} else {
					//traiter la liste chainée spéciale de si
					if (strcmp(debut->Tab,"si")==0 && liste_finsi == 0) {
						InsererFin(&debut,Tab_Ligne);
					} else {
						InsererFin(&debut,Tab_Ligne);
						Afficher(debut);
						//ici on doit commencer l'analyse syntaxique
						Analyse_syntaxique(debut,s,nlig);
						//ici on doit commencer l'analyse semantique
						Analyse_semantique(debut,sem,nlig);
						debut_copie =debut;
						debut =NULL;
					}
				}
			}
			espace++;
		} else {
			Tab_Ligne[i++] = c;
			ncar++;
			Tab_Ligne[i] = '\0';
			espace = 0;
		}
	}

	//pour traiter le cas si une ou plusieurs parties de l'algorithme n'est pas terminée par un ';'
	if(debut!=NULL) {
		Analyse_syntaxique(debut,s,nlig);
		Afficher(debut);
	}

	//traiter si le fichier finis par fin.
	if(verif_fin ==0) {
		mot* temp =debut_copie;
		while(temp->next!=NULL) {
			temp = temp->next;
		}
		nlig++;
		fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, il faut finir par le mot fin.  mot errone: %s\n",k,nlig,temp->Tab);
		k++;
	}
	//traiter le cas special de nbr de si et finsi après la fin de fichier
	if(compteur_si != compteur_finsi) {
		mot* temp =debut_copie;
		while(temp->next!=NULL) {
			temp = temp->next;
		}
		nlig++;
		fprintf(s,"Erreur syntaxique numero %d dans la ligne numero %d, le nombre des \"si\" doit etre egal au nombre des \"finsi\" dans l'algorithme'. mot errone:'%s'\n",k,nlig,temp->Tab);
		k++;
	}

	printf("\n");

	printf("\n");
	fclose(f);
	fclose(e); // Fermeture du fichier erreurs lexicales
	fclose(s); // Fermeture du fichier erreurs syntaxiques
	fclose(sem); // Fermeture du fichier erreurs semantiques
	printf("-------------------------------------------------------------------------------------------------------------------------\n");


	//maintenant on affiche les erreurs lexicales (s'il y a aucune erreur rien ne va être affiché)

	if (err_lex_existe ==1) {
		FILE *err_lix = fopen("erreurs_lexicales.txt", "r");
		if (err_lix == NULL) {
			printf("Impossible d'ouvrir le fichier des erreurs lexicales.\n");
			return 1;
		}
		char c_err;
		while ((c_err = fgetc(err_lix)) != EOF) {
			printf("%c", c_err);
		}
		fclose(err_lix); // Fermeture du fichier erreurs.txt

	}
	//else{
	printf("l'analyse des erreurs lexicales est terminee--------------------------------------------------------------\n\n");
	//ici affichage des erreurs syntaxiques
	FILE *err_syn = fopen("erreurs_syntaxiques.txt", "r");
	if (err_syn == NULL) {
		printf("Impossible d'ouvrir le fichier des erreurs syntaxiques.\n");
		return 1;
	}
	char c_err;
	while ((c_err = fgetc(err_syn)) != EOF) {
		printf("%c", c_err);
	}
	fclose(err_syn); // Fermeture du fichier erreurs.txt
	printf("\nl'analyse des erreurs syntaxiques est terminee------------------------------------------------------------\n\n");
	fclose(err_syn);//fermeture du fichier

	FILE *err_sem = fopen("erreurs_semantiques.txt", "r");
	if (err_sem == NULL) {
		printf("Impossible d'ouvrir le fichier des erreurs semantiques.\n");
		return 1;
	}
	char c_errsem;
	while ((c_errsem = fgetc(err_sem)) != EOF) {
		printf("%c", c_errsem);
	}
	fclose(err_sem); // Fermeture du fichier erreurs.txt
	printf("\nl'analyse des erreurs semantiques est terminee------------------------------------------------------------\n\n");

	//}
	free(debut);
	printf("nombre d'occurances de \"si\": %d \nnombre d'occurances de \"finsi\": %d \n",compteur_si,compteur_finsi);
	return 0;
}
