#ifndef __ENTETE_H_
#define __ENTETE_H_

#include <cstdio>
#include <cstdlib> 
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>  
#include <cmath>
#include <vector>
#include <functional>
#include <algorithm>
using namespace std;

//Ne pas modifier TProblem
struct TProblem									//**Definition du probleme:
{
	std::string Nom;							//**Nom du fichier de donnees
	int NbJob;									//**Taille de l'instance: Nombre de taches
	std::vector<int> Temps;						//**Temps de réalisation de la tache. NB: Tableau de 0 à NbJob-1.  
	std::vector<int> DateDue;					//**Date d'echeance de la tache. NB: Tableau de 0 à NbJob-1.
	std::vector<int> Poids;						//**Poids (importance) de la tache. NB: Tableau de 0 à NbJob-1.
	int NbContPrec;								//**Nombre de contraintes de preseance pour cette instance
	std::vector<std::vector <int> > Pred;		//**Pour chaque tache i: Liste des taches devant être réalisées avant i (predecesseur). 
												// NB: 2e dimension variable selon le nombre de prédécesseurs de i.
	std::vector<std::vector <int> > Succ;		//**Pour chaque tache i: Liste des taches devant être réalisées après i (successeur). Autrement dit: i est predecesseur de ces taches.
												// NB: 2e dimension variable selon le nombre de successeurs de i.
	std::vector<std::vector <bool> > MatPred;	//**Matrice de booléen entre chaque paire de taches indiquant si la tache en ligne est prédécesseur a la tache en colonne.
												// NB: Tableau de 0 à NbJob-1 x NbJob-1.
};

//Ne pas modifier TSolution
struct TSolution						//**Definition d'une solution: 
{
	std::vector<int> Seq;				//**Sequence de realisation des taches. NB: Tableau de 0 à NbJob-1.
	long FctObj;						//**Valeur de la fonction obj: Sommation des retards pondérés.					//Si invalide = INT_MAX
	bool Valide;						//**Etat de validite de la solution (presence de chaque tache une et une seule fois + respect des contraintes de preseance). Verifier lorsque la solution est evaluee.
	int NbEvaltoGet;					//**Nombre d'evalutions de solutions necessaires pour trouver cette solution... pour estimer convergence
	std::vector<int> Fin;				//**Temps de terminaison d'une tache dans la sequence.
	std::vector<int> Retard;			//**Nombre d'unites de temps de retard pour une tache: Max {0, Fin-DateDue}
	std::vector<int> wRetard;			//**Retard pondere pour une tache (Retard*Poids)	
};

//Ne pas modifier TAlgo
struct TAlgo
{
	int		TaillePop;					//**Taille de la population (nombre d'individus). NB: valeur minimale = 2
	int		TaillePopEnfant;			//**Taille de la populationEnfant (nombre d'enfants = TaillePop * ProbCr)
	double	ProbCr;						//**Probabilite de croisement [0%,100%]
	double	ProbMut;					//**Probabilite de mutation [0%,100%] 
	int		Gen;						//**Compteur du nombre de generations (iterations)

	int		CptEval;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	long	FctObjSolDepart;			//**Valeur de la fonction objectif de la meilleure solution lors de la 1ere generation (iteration)
};
#endif