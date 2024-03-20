#include "Entete.h"
#pragma comment (lib,"GeneticDLL.lib")
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (GeneticDLL.dll et GeneticDLL.lib) doivent se trouver dans le r�pertoire courant du projet pour une ex�cution � l'aide du compilateur. 
//Indiquer les arguments du programme dans les propri�t�s du projet - d�bogage - arguements.
//Sinon, utiliser le r�pertoire execution.
//NB: le projet actuel doit etre compile dans le meme mode (DEBUG ou RELEASE) que les fichiers de DLL - par defaut en RELEASE

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION: Lecture du Fichier probleme et initialiation de la structure Problem
//NB: Construction des listes pr�d�cesseurs/successeurs pour chaque tache
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem &unProb, TAlgo &unAlgo);

//DESCRIPTION: Affichage a l'ecran permettant de voir si les donnees du fichier (instance) probleme ont ete lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme (TProblem unProb);

//DESCRIPTION:	Evaluation de la fonction objectif (somme des retards ponderes) d'une solution et MAJ du compteur d'�valuation  + Validation de la solution 
//				Validation de la solution: verification des contraintes de preseance et de la presence de chaque tache une et une seule fois
extern "C" _declspec(dllimport) void EvaluerSolution(TSolution &uneSol, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION: G�n�ration d'une population initiale de solutions valides, incluant l'�valuation et la validation
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
//- Si solution valide en entr�e: une mutation valide est produite et retournee - swap adjacent
//- Si solution NON valide en entr�e : une mutation(sans assurer la validite) est produite et retournee - swap aleatoire
extern "C" _declspec(dllimport) void Mutation(TSolution &Mutant, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION: S�lection d'un individu par tournoi (taille 2)
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

	//**Lecture des param�tres
	NomFichier.assign(Param[1]);
	LAlgo.TaillePop = atoi(Param[2]);
	LAlgo.ProbCr = atof(Param[3]);
	LAlgo.ProbMut = atof(Param[4]);
	LAlgo.NB_EVAL_MAX = atoi(Param[5]);
	LAlgo.TaillePopEnfant = (int)ceil(LAlgo.TaillePop * LAlgo.ProbCr);
	LAlgo.Gen = 0;

	srand((unsigned)time(NULL));					//**Precise un germe pour le generateur aleatoire

	//**D�finition de la dimension des tableaux
	Pop.resize(LAlgo.TaillePop);				//**Le tableau utilise les indices de 0 � TaillePop-1.
	PopEnfant.resize(LAlgo.TaillePopEnfant);	//**Le tableau utilise les indices de 0 � TaillePopEnfant-1

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
			} while (Mere == Pere); //Validation: Pere et Mere ne doivent pas avoir le m�me # de solution
						
			//**CROISEMENT entre les deux parents. Cr�ation de 1 enfant.
			PopEnfant[i] = Croisement(Pop[Pere], Pop[Mere], LeProb, LAlgo);

			//V�rification pour ne pas perdre une meilleure solution connue: si mutation OU remplacement non elistite
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
		
		//**REMPLACEMENT de la population pour la prochaine g�n�ration
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


//Ajoute; a PossibleJobs, les sucesseur de l'élément en paramètre qui peuvent etre placés
void AddSuccToPossibleJobs(std::vector<int>& PossibleJobs, int Element, int index, std::vector<int> Enfant, TProblem unProb)
{
	/*
	std::cout << "____\nElement " << Element << "\n";

	for (int i = 0; i < Enfant.size(); i++)
	{
		if (Enfant[i] != -1)
			std::cout << Enfant[i] << " ";
		else
			std::cout << ". ";
	}
	std::cout << "\n";

	for (int i = 0; i < PossibleJobs.size(); i++)
	{
			std::cout << PossibleJobs[i] << " ";
	}
	std::cout << "\n\n";
	*/

	//std::cout << "Enfant ";
	for (int& i : Enfant) {
		if (i == -1)
		{
			//std::cout << ". ";
		}
		else
		{
			//std::cout << i << " ";
		}

	}
	//std::cout << std::endl;
	//std::cout << "Pj\t";
	for (int i = 0; i < PossibleJobs.size(); i++)
	{
		//std::cout << PossibleJobs[i] << "\t";
	}
	//std::cout << "\n";

	//std::cout << Element << "Succ\t";
	for (int i = 0; i < unProb.Succ[Element].size(); i++)
	{
		int CurJob = unProb.Succ[Element][i];
		//std::cout << CurJob << " pred ";


		//on verifie que tout les prédécésseur sont placé en avant dans Enfant.Seq
		int IsPossible = true;
		for (int j = 0; j < unProb.Pred[CurJob].size(); j++)
		{
			int CurPred = unProb.Pred[CurJob][j];
			//std::cout << CurPred << " ";



			int Pos = -1;
			int x = 0;
			while (x <= index)
			{
				if (Enfant[x] == CurPred)
				{
					Pos = x;
					break;
				}
				x++;
			}

			//si le prédécessuer n'a pas été trouver, on quitte la boucle et on n'ajoutera pas ce sucesseur a PossibleJob
			if (Pos == -1)
			{
				IsPossible = false;
				break;
			}

		}

		//si tout les predecesseur ont été trouvé, qu'il n'est pas dans Enfant.Seq ou dans PossibleJobs, on ajoute le sucesseur.
		if (IsPossible && (std::find(PossibleJobs.begin(), PossibleJobs.end(), CurJob) == PossibleJobs.end()) && (std::find(Enfant.begin(), Enfant.end(), CurJob) == Enfant.end()))
		{
			PossibleJobs.push_back(CurJob);
		}

		//if(IsPossible)
		//	std::cout << "I\t";
		//else
			//std::cout << "IN\t";
	}
	//std::cout << "\n\n";
}


//******************************************************************************************************
//**Fonction qui r�alise le CROISEMENT (�change de genes) entre deux parents. Retourne l'enfant produit.
//******************************************************************************************************
//**A D�FINIR PAR L'�TUDIANT****************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAM�TRES AINSI QUE LE RETOUR DE LA FONCTION
//****************************************************************************************************** 
TSolution Croisement(TSolution Parent1, TSolution Parent2, TProblem unProb, TAlgo &unAlgo)
{
	//**INDICE: Le sous-programme rand() g�n�re al�atoirement un nombre entier entre 0 et RAND_MAX (i.e., 32767) inclusivement.
	//**Pour tirer un nombre al�atoire entier entre 0 et MAX-1 inclusivement, il suffit d'utiliser l'instruction suivante : NombreAleatoire = rand() % MAX;
	
	//std::cout << "__________________________\nStart Croisement\n";
	
	TSolution Enfant;
	

	Enfant.Fin.resize(unProb.NbJob);
	Enfant.Retard.resize(unProb.NbJob);
	Enfant.wRetard.resize(unProb.NbJob);

	for (int i = 0; i < unProb.NbJob; i++)
	{
		Enfant.Seq.push_back(-1);
	}


	//On commence par ajouté tout les éléments qui sont dans les deux parents et qui sont placeable.
	for (int i = 0; i < unProb.NbJob; i += 1) {
		bool to_add = false;

		if (Parent1.Seq[i] == Parent2.Seq[i]) {
			to_add = true;
		

			if (!(std::find(Enfant.Seq.begin(), Enfant.Seq.end(), Parent1.Seq[i]) != Enfant.Seq.end())) {
			
				//std::cout << Parent1.Seq[i] << " Predecesseurs\t";
				for (int& j : unProb.Pred[Parent1.Seq[i]]) {

					std::vector<int>::iterator it;
					it = std::find(Enfant.Seq.begin(), Enfant.Seq.end(), j);
					if (it != Enfant.Seq.end())
					{
						//std::cout << j << " found at position : ";
						//std::cout << it - Enfant.Seq.begin() << "|\t";
					}
					else {
						//std::cout << j << "not found\t";
						to_add = false;
						break;
					}
				}
				//std::cout << "\n";
			}
		}
		if (to_add) {
			Enfant.Seq[i] = Parent1.Seq[i];
		}
		else {
			Enfant.Seq[i] = -1;
		}
	}


	//std::cout << "Enfant ";
	for (int& i : Enfant.Seq) {
		if(i == -1)
		{
			//std::cout << ". ";
		}
		else
		{
			//std::cout << i << " ";
		}
		
	}
	//std::cout << std::endl;






	std::vector<int> PossibleJobs;
	for (int i = 0; i < unProb.NbJob; i++)
	{
		if (unProb.Pred[i].size() == 0 && (std::find(Enfant.Seq.begin(), Enfant.Seq.end(), i) == Enfant.Seq.end()))
		{
			PossibleJobs.push_back(i);
		}
	}




	for (int i = 0; i < unProb.NbJob; i++)
	{

		if (Enfant.Seq[i] == -1)
		{

			/*
			
			On test l'élément qui est dans les parent a cet emplacement.
			- si c'est le même dans les deux parent et qu'il est dans possible job, on ajoute cet élément
			- sinon, si un des parents a un élément qui est dans PossibleJob, on l'ajoute, choisi aléatoirement si c'est le cas pour les deux parent.
			- sinon, on choisi un élément dans PossibleJobs aléatoirement.
			
			*/

			std::vector<int>::iterator it1;
			it1 = std::find(PossibleJobs.begin(), PossibleJobs.end(), Parent1.Seq[i]);

			std::vector<int>::iterator it2;
			it2 = std::find(PossibleJobs.begin(), PossibleJobs.end(), Parent2.Seq[i]);


			if (Parent1.Seq[i] == Parent2.Seq[i] && (it2 != PossibleJobs.end()))
			{
				Enfant.Seq[i] = PossibleJobs[it1 - PossibleJobs.begin()];
				PossibleJobs.erase(it1);
			}
			else if (it1 != PossibleJobs.end() && it2 != PossibleJobs.end())
			{
				if (rand() % 2 < 1)
				{
					Enfant.Seq[i] = PossibleJobs[it1 - PossibleJobs.begin()];
					PossibleJobs.erase(it1);
				}
				else
				{
					Enfant.Seq[i] = PossibleJobs[it2 - PossibleJobs.begin()];
					PossibleJobs.erase(it2);
				}
			}
			else if (it1 != PossibleJobs.end())
			{
				Enfant.Seq[i] = PossibleJobs[it1 - PossibleJobs.begin()];
				PossibleJobs.erase(it1);
			}
			else if (it2 != PossibleJobs.end())
			{
				Enfant.Seq[i] = PossibleJobs[it2 - PossibleJobs.begin()];
				PossibleJobs.erase(it2);
			}
			else
			{
				int randint = rand() % PossibleJobs.size();
				Enfant.Seq[i] = PossibleJobs[randint];
				PossibleJobs.erase(PossibleJobs.begin() + randint);
			}

		}

		AddSuccToPossibleJobs(PossibleJobs, Enfant.Seq[i], i, Enfant.Seq, unProb);
	}



	//**NE PAS ENLEVER
	EvaluerSolution(Enfant, unProb, unAlgo);
	
	if (!Enfant.Valide) {
		std::cout << "Solution invalide generee" << std::endl;
		for (int& i : Parent1.Seq) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
		
		for (int& i : Parent2.Seq) {
			std::cout << i << " ";
		}
		std::cout << std::endl;

		std::cout << "Enfant ";
		for (int& i : Enfant.Seq) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
		std::cout << "----------------------------------------" << std::endl;
	}
	
	return (Enfant);
}


//*******************************************************************************************************
//Fonction qui r�alise le REMPLACEMENT de la population pour la prochaine generation. Cette fonction doit
//prendre les TaillePop solutions de la population "Parents" et les TaillePopEnfant solutions de la 
//population "Enfants" et determiner les solutions a retenir pour la prochaine generation.  
//Les TaillePop solutions retenues doivent �tre placees dans la population "Parents" (qui est retournee). 
//*******************************************************************************************************
//**A DEFINIR PAR L'ETUDIANT*****************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAMETRES
//******************************************************************************************************* 
void Remplacement(std::vector<TSolution>& Parents, std::vector<TSolution> Enfants, TProblem unProb, TAlgo unAlgo)
{
	//METHODE BIDON: La population Parent demeure inchang�e

	//INFOS pour d�finir votre methode de remplacement...

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

	TrierPopulation(Temporaire, 0, unAlgo.TaillePop + unAlgo.TaillePopEnfant);

	int parentsize = Parents.size();


	//afin d'explorer l'ensemble des solutions, on portion des solutions gard�es pour la population suivante seront choisi al�atoirement et l'autre de façon déterministe. 
	float RandRatio = (int)0.8 * parentsize;

	Parents.clear();

	//choisi les meilleurs individus de la population
	for (i = 0; i < parentsize - RandRatio; i++)
	{
		Parents.push_back(Temporaire[i]);
	}

	Temporaire.erase(Temporaire.begin(), Temporaire.begin() + i);


	//choisi un nombre d'individu al�atoirement
	for (i = 0; i < RandRatio; i++)
	{
		int j = rand() % Temporaire.size();

		Parents.push_back(Temporaire[j]);
		Temporaire.erase(Temporaire.begin() + j);
	}


	for (i = 0; i < Temporaire.size(); i++)
		Temporaire[i].Seq.clear();
	Temporaire.clear();
}
