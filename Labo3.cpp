#include "Entete.h"
#pragma comment (lib,"GeneticDLL.lib")
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (GeneticDLL.dll et GeneticDLL.lib) doivent se trouver dans le répertoire courant du projet pour une exécution à l'aide du compilateur. 
//Indiquer les arguments du programme dans les propriétés du projet - débogage - arguements.
//Sinon, utiliser le répertoire execution.
//NB: le projet actuel doit etre compile dans le meme mode (DEBUG ou RELEASE) que les fichiers de DLL - par defaut en RELEASE

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION: Lecture du Fichier probleme et initialiation de la structure Problem
//NB: Construction des listes prédécesseurs/successeurs pour chaque tache
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem &unProb, TAlgo &unAlgo);

//DESCRIPTION: Affichage a l'ecran permettant de voir si les donnees du fichier (instance) probleme ont ete lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme (TProblem unProb);

//DESCRIPTION:	Evaluation de la fonction objectif (somme des retards ponderes) d'une solution et MAJ du compteur d'évaluation  + Validation de la solution 
//				Validation de la solution: verification des contraintes de preseance et de la presence de chaque tache une et une seule fois
extern "C" _declspec(dllimport) void EvaluerSolution(TSolution &uneSol, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION: Génération d'une population initiale de solutions valides, incluant l'évaluation et la validation
extern "C" _declspec(dllimport) void CreerPopInitialeAleaValide(std::vector<TSolution> &unePop, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION: Generation d'une population initiale de solutions valides, incluant leur evaluation et leur validation
extern "C" _declspec(dllimport) void CreerPopulationInitialeAleatoireValide(std::vector<TSolution> &unePop, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION: Affichage d'une solution a l'ecran (AVEC ou SANS details des calculs de la fonction objectif)
extern "C" _declspec(dllimport) void AfficherDetailsSolution(const TSolution uneSolution, TProblem unProb, bool AvecDetails);

//DESCRIPTION: Affichages des solutions dans la population courante
extern "C" _declspec(dllimport) void AfficherPopulation(std::vector<TSolution> unePop, int Iter, TProblem unProb);

//DESCRIPTION: Tri croissant des individus dans la population entre Debut et Fin-1 INCLUSIVEMENT 
extern "C" _declspec(dllimport) void TrierPopulation(std::vector<TSolution> &unePop, int Debut, int Fin);

//DESCRIPTION: Mutation (modification aleatoire) d'une solution. La solution mutue est evaluee avant d'etre retournee.
//- Si solution valide en entrée: une mutation valide est produite et retournee - swap adjacent
//- Si solution NON valide en entrée : une mutation(sans assurer la validite) est produite et retournee - swap aleatoire
extern "C" _declspec(dllimport) void Mutation(TSolution &Mutant, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION: Sélection d'un individu par tournoi (taille 2)
extern "C" _declspec(dllimport) int Selection (std::vector<TSolution> unePop, int _Taille, TProblem unProb);

//DESCRIPTION: Affichage a l'ecran des resultats de l'algorithme evolutionnaire: solution finale (avec etat de validite), nombre d'evaluations et parametres de l'algorithme
extern "C" _declspec(dllimport) void AfficherResultats(TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier texte des resultats de l'algorithme evolutionnaire: solution finale (avec etat de validite), nombre d'evaluations et parametres de l'algorithme
extern "C" _declspec(dllimport) void AfficherResultatsFichier(TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement
extern "C" _declspec(dllimport) void LibererMemoireFinPgm(std::vector<TSolution> &unePop, std::vector<TSolution> &unePopEnfant, TSolution &uneBest, TProblem &unProb, TAlgo unAlgo);

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************
TSolution Croisement(TSolution Parent1, TSolution Parent2, TProblem unProb, TAlgo &unAlgo);
void Remplacement(std::vector<TSolution> &Parents, std::vector<TSolution> Enfants, TProblem unProb, TAlgo unAlgo);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char* Param[])
{
	TProblem LeProb;					//**Definition de l'instance de probleme
	TAlgo LAlgo;						//**Definition des parametres de l'algorithme
	std::vector<TSolution> Pop;			//**Population composee de Taille_Pop solutions 
	std::vector<TSolution> PopEnfant;	//**Population d'enfant composee de (TaillePopEnfant = Taille_Pop * ProbCr) solutions
	TSolution Best;						//**Meilleure solution depuis le debut de l'algorithme

	int Pere, Mere;						//**Indices de solution des parents
	int i;
	double Alea;
	string NomFichier;

	//**Lecture des paramètres
	NomFichier.assign(Param[1]);
	LAlgo.TaillePop = atoi(Param[2]);
	LAlgo.ProbCr = atof(Param[3]);
	LAlgo.ProbMut = atof(Param[4]);
	LAlgo.NB_EVAL_MAX = atoi(Param[5]);
	LAlgo.TaillePopEnfant = (int)ceil(LAlgo.TaillePop * LAlgo.ProbCr);
	LAlgo.Gen = 0;

	srand((unsigned)time(NULL));					//**Precise un germe pour le generateur aleatoire

	//**Définition de la dimension des tableaux
	Pop.resize(LAlgo.TaillePop);				//**Le tableau utilise les indices de 0 à TaillePop-1.
	PopEnfant.resize(LAlgo.TaillePopEnfant);	//**Le tableau utilise les indices de 0 à TaillePopEnfant-1

	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	AfficherProbleme(LeProb);

	//**Initialisation de la population initiale NB: Initialisation de la population entraine des evaluation de solutions.
	//**CptEval est donc = TaillePop au retour de la fonction.
	CreerPopulationInitialeAleatoireValide(Pop, LeProb, LAlgo);
	//AfficherPopulation(Pop, LAlgo.Gen, LeProb);

	//**Tri de la population
	TrierPopulation(Pop, 0, LAlgo.TaillePop);
	//AfficherPopulation(Pop, LAlgo.Gen, LeProb);

	//**Initialisation de de la meilleure solution au debut de l'algorithme
	Best = Pop[0];
	LAlgo.FctObjSolDepart = Pop[0].FctObj;
	
	//**Boucle principale de l'algorithme evolutionnaire
	do 
	{
		LAlgo.Gen++;
		//**Selection, croisement et mutation
		for (i=0; i<LAlgo.TaillePopEnfant; i++)
		{
			//**SELECTION de deux parents (tournoi de taille = 2)
			Pere = Selection(Pop, LAlgo.TaillePop, LeProb);
			do
			{
				Mere = Selection(Pop, LAlgo.TaillePop, LeProb);
			} while (Mere == Pere); //Validation: Pere et Mere ne doivent pas avoir le même # de solution
						
			//**CROISEMENT entre les deux parents. Création de 1 enfant.
			PopEnfant[i] = Croisement(Pop[Pere], Pop[Mere], LeProb, LAlgo);

			//Vérification pour ne pas perdre une meilleure solution connue: si mutation OU remplacement non elistite
			if (PopEnfant[i].Valide && (PopEnfant[i].FctObj < Best.FctObj))	Best = PopEnfant[i];

			//AfficherDetailsSolution(Pop[Pere], LeProb, true);
			//AfficherDetailsSolution(Pop[Mere], LeProb, true);
			//AfficherDetailsSolution(PopEnfant[i], LeProb, true);

			//**MUTATION d'une solution
			Alea = double(rand()) / double(RAND_MAX);
			if (Alea < LAlgo.ProbMut)
			{
				Mutation(PopEnfant[i], LeProb, LAlgo);
			}
		}
		//AfficherPopulation(Pop, LAlgo.Gen, LeProb);
		//AfficherPopulation(PopEnfant, LAlgo.Gen, LeProb);
		
		//**REMPLACEMENT de la population pour la prochaine génération
		Remplacement(Pop, PopEnfant, LeProb, LAlgo);
		TrierPopulation(Pop, 0, LAlgo.TaillePop);
		
		//**Conservation de la meilleure solution
		if (Pop[0].Valide && (Pop[0].FctObj < Best.FctObj)) Best = Pop[0];																					
		std::cout << "(Gen#" << setw(3) << LAlgo.Gen << ") Best OBJ: " << Best.FctObj; (!Best.Valide) ? std::cout << "***NON Valide\n": std::cout << endl;  
	}while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX);	//**NE PAS ENLEVER

	AfficherResultats (Best, LeProb, LAlgo);																												//**NE PAS ENLEVER
	AfficherResultatsFichier (Best, LeProb, LAlgo, "Resultats.txt");
	
	LibererMemoireFinPgm(Pop, PopEnfant, Best, LeProb, LAlgo);
	//system("PAUSE");
	return 0;
}
//******************************************************************************************************
//**Fonction qui réalise le CROISEMENT (échange de genes) entre deux parents. Retourne l'enfant produit.
//******************************************************************************************************
//**A DÉFINIR PAR L'ÉTUDIANT****************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAMÈTRES AINSI QUE LE RETOUR DE LA FONCTION
//****************************************************************************************************** 
TSolution Croisement(TSolution Parent1, TSolution Parent2, TProblem unProb, TAlgo &unAlgo)
{
	//**INDICE: Le sous-programme rand() génère aléatoirement un nombre entier entre 0 et RAND_MAX (i.e., 32767) inclusivement.
	//**Pour tirer un nombre aléatoire entier entre 0 et MAX-1 inclusivement, il suffit d'utiliser l'instruction suivante : NombreAleatoire = rand() % MAX;
	
	int j;
	TSolution Enfant;
	Enfant = Parent1;
	int restants = unProb.NbJob;
	Enfant.Seq = std::vector<int>(unProb.NbJob);
	for (int i = 0; i < unProb.NbJob; i += 1) {
		bool to_add = false;
		if (Parent1.Seq[i] == Parent2.Seq[i]) {
			to_add = true;
			if (!(std::find(Enfant.Seq.begin(), Enfant.Seq.end(), Parent1.Seq[i]) != Enfant.Seq.end())) {
				for (int& j : unProb.Pred[Parent1.Seq[i]]) {
					if (!(std::find(Enfant.Seq.begin(), Enfant.Seq.end(), j) != Enfant.Seq.end())) {
						to_add = false;
					}
				}
			}
		}
		if (to_add) {
			Enfant.Seq[i] = Parent1.Seq[i];
			restants -= 1;
		}
		else {
			Enfant.Seq[i] = -1;
		}
	}
	while (restants > 0) {
		int index = 0;
		std::vector<int> placed;
		std::vector<int> placeable;
		while (Enfant.Seq[index] != -1) {
			placed.push_back(Enfant.Seq[index]);
			index += 1;
		}
		for (int i = 0; i < unProb.NbJob; i += 1) {
			if (!(std::find(Enfant.Seq.begin(), Enfant.Seq.end(), i) != Enfant.Seq.end())) {
				bool to_add = true;
				for (int &j : unProb.Pred[i]) {
					if (!(std::find(placed.begin(), placed.end(), j) != placed.end())) {
						to_add = false;
					}
				}
				if (to_add) {
					placeable.push_back(i);
				}
			}
		}
		Enfant.Seq[index] = placeable[rand() % placeable.size()];
		restants -= 1;
	}

	//METHODE BIDON: Recopie l'es genes du'enfant est une copie du Parent1

	//**NE PAS ENLEVER
	EvaluerSolution(Enfant, unProb, unAlgo);
	if (Enfant.FctObj == 2147483647) {
		std::cout << "Solution invalide generee" << std::endl;
		for (int& i : Parent1.Seq) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
		for (int& i : Parent2.Seq) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
		for (int& i : Enfant.Seq) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
		std::cout << "----------------------------------------" << std::endl;
	}
	return (Enfant);
}


//*******************************************************************************************************
//Fonction qui réalise le REMPLACEMENT de la population pour la prochaine generation. Cette fonction doit
//prendre les TaillePop solutions de la population "Parents" et les TaillePopEnfant solutions de la 
//population "Enfants" et determiner les solutions a retenir pour la prochaine generation.  
//Les TaillePop solutions retenues doivent être placees dans la population "Parents" (qui est retournee). 
//*******************************************************************************************************
//**A DEFINIR PAR L'ETUDIANT*****************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAMETRES
//******************************************************************************************************* 
void Remplacement(std::vector<TSolution> &Parents, std::vector<TSolution> Enfants, TProblem unProb, TAlgo unAlgo)
{
	//METHODE BIDON: La population Parent demeure inchangée

	//INFOS pour définir votre methode de remplacement...

	int i;

	//**Declaration et dimension dynamique d'une population temporaire pour contenir tous les parents et les enfants
	std::vector<TSolution> Temporaire;
	for (i = 0; i < Parents.size(); i++)
	{
		Temporaire.push_back(Parents[i]);
	}
	for (i = 0; i < Enfants.size(); i++)
	{
		Temporaire.push_back(Enfants[i]);
	}


	//**Pour trier toute la population temporaire, il suffit de faire l'appel suivant: 
	
	TrierPopulation(Temporaire, 0, unAlgo.TaillePop+ unAlgo.TaillePopEnfant);

	int parentsize = Parents.size();

	float RandRatio = 1 - std::min(1.0f, unAlgo.CptEval * unAlgo.CptEval / (float)(unAlgo.NB_EVAL_MAX * unAlgo.NB_EVAL_MAX));
	RandRatio *= 0.75 * parentsize;
	RandRatio = (int)RandRatio;
	
	Parents.clear();

	for (i = 0; i < parentsize - RandRatio; i++)
	{
		Parents.push_back(Temporaire[i]);
	}
	
	Temporaire.erase(Temporaire.begin(), Temporaire.begin() + i);

	for (i = 0; i < RandRatio; i++)
	{
		int j = rand() % Temporaire.size();

		Parents.push_back(Temporaire[j]);
		Temporaire.erase(Temporaire.begin() + j);
	}

	
	
	//std::cout << unAlgo.CptEval * unAlgo.CptEval / (float)(unAlgo.NB_EVAL_MAX* unAlgo.NB_EVAL_MAX) << " " << parentsize << " "<< parentsize - RandRatio << " " << RandRatio << "\n";

	//... a definir

	//**A LA FIN: Liberation de la population temporaire
	
	for (i = 0; i < Temporaire.size(); i++)
		Temporaire[i].Seq.clear();
	Temporaire.clear();
}
