#pragma warning( disable : 4996 )

#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
using namespace std;

#define DEBUG 1
int PAUSE = 0;

// touche P   : mets en pause
// touche ESC : ferme la fenêtre et quitte le jeu

struct GameData;  // ON TOP
/*----------------------------------------------------------------*/
enum CibleEtat {
	DESACTIVE = 0,
	ACTIVE = 1,
	TOUCHE = 2
};

struct Cible {
	V2 A, B;
	int nEtat;
	int nFamille;
	int nHitTime;

	Cible(V2 A, V2 B, int nFamille) {
		this->A = A;
		this->B = B;
		this->nEtat = ACTIVE;
		this->nFamille = nFamille;
		this->nHitTime = -1;
	}

	void cibleRefresh() {
		if (nEtat == TOUCHE) {
			nHitTime--;
		}
		if (nHitTime == 0) {
			nEtat = DESACTIVE;
		}
	}

	void cibleHit(GameData& G); /*{
		this->nHitTime = 60;
		this->nEtat = TOUCHE;
        G.nScore += 500;
	} //*/

	void cibleReactivate() {
		this->nHitTime = -1;
		this->nEtat = ACTIVE;
	}
};

struct Rangee {
	V2 pPosition;
	int nLongueur;
	int nEspacement;
	V2  vDirection;
	vector<Cible> lCibles;
	int nQuantite;
	int nFamille;
	int nDownTime;
	int nEtat;

	Rangee(V2 pPosition, int nLongueur, int nEspacement, V2 vDirection, int nFamille) {
		this->pPosition = pPosition;
		this->nLongueur = nLongueur;
		this->nEspacement = nEspacement;
		this->vDirection = vDirection.GetNormalized();
		this->nQuantite = 0;
		this->nFamille = nFamille;
		this->nDownTime = -1;
		this->nEtat = ACTIVE;
	}

	void addCible() {
		V2 A = pPosition + (nQuantite*nEspacement)*vDirection;
		V2 B = pPosition + (nQuantite*nEspacement +nLongueur)*vDirection;
		lCibles.push_back( Cible(A, B, nFamille) );
		nQuantite++;
	}

	void addPlusieursCible(const int n) {
		if (n>0) {
			for (int i=0; i<n; i++) {
				this->addCible();
			}
		}
	}

	void rangeeActivate() {
		nDownTime = -1;
		for (Cible& C : lCibles) {
			C.cibleReactivate();
		}
		this->nEtat = ACTIVE;
		if (DEBUG) cout <<"Rangee #" <<nFamille <<" has been activated" <<". \n";
	}

	void rangeeDeactivate(GameData& G); /*{
		this->nDownTime = 300;
		this->nEtat = DESACTIVE;
        G.nScore += 1111;
	} //*/

	void rangeeRefresh() {
		if (this->nEtat != DESACTIVE) {
			return;
		}
		if (nDownTime >= 0) {
			nDownTime--;
		} else {
			rangeeActivate();
		}
	}

	void rangeeHit(GameData& G); /*{
		int i = 0;
		for (Cible& C : lCibles) {
			if (C.nEtat == ACTIVE) {
				if (DEBUG) cout <<"rangeeHit: Rangee #" <<nFamille <<" remains active, Cible #" <<i <<" is still active" <<". \n";
				return;
			}
			i++;
		}
		rangeeDeactivate(G);
	} //*/
};

struct Bumper {
	V2 A;
    int nRayon;
    double nRayonFlash;
    double nFlashUnsize = 4.;
	int nEtat;
	int nHitTime;

	Bumper(V2 A, int nRayon) {
		this->A = A;
        this->nRayon = nRayon;
        this->nRayonFlash = 0.;
		this->nEtat = ACTIVE;
		this->nHitTime = -1;
	}

	void bumperRefresh() {
        if (nEtat == TOUCHE) {
            nHitTime--;
            if (this->nRayonFlash >= 0) this->nRayonFlash -= 1/nFlashUnsize;
        }
        if (nHitTime == 0) {
            this->nRayonFlash = 0;
            nEtat = ACTIVE;
        }
    }

    void bumperHit(GameData& G); /* {
        this->nHitTime = 240;
        this->nRayonFlash = this->nHitTime/nFlashUnsize;
        this->nEtat = TOUCHE;
        G.nScore += 100;
    } //*/

    void bumperReactivate() {
        this->nHitTime = -1;
        this->nEtat = ACTIVE;
    }
};

/*----------------------------------------------------------------*/
//
//    Données du jeu - structure instanciée dans le main
struct GameData {
	int idFrame    = 0;
	int HeightPix  = 800;
	int WidthPix   = 600;

		// Ball position initiale
	int BallRadius = 15;
	V2  BallPos    = V2(100, 100);
	// V2  BallMove;
    V2  BallMove = V2(0, 1);
	vector<V2> PreviousPos;

		// Bords du flipper
	vector<V2> LP{
		V2(595, 550), V2(585, 596), V2(542, 638), V2(476, 671), V2(392, 692),
		V2(300, 700), V2(207, 692), V2(123, 671), V2(057, 638), V2(014, 596),
		V2(005, 550), V2(005, 005), V2(595, 005), V2(595, 550),
	};

	vector<vector<V2>> Lines{
		{BallPos+V2(   0, 200), BallPos+V2( 100, 200) },
		{BallPos+V2(-100, 250), BallPos+V2( 100, 250) },
		{BallPos+V2(-100, 300), BallPos+V2(   0, 300) },
	};

	vector<Rangee> lRangee{
		Rangee(V2(          020, 500), (BallRadius*2)+2, (BallRadius*2)+6,  V2( 3, 4), 66),
		Rangee(V2(          350, 680), (BallRadius*2)+1, (BallRadius*2)+10, V2( 4,-1), 98),
		Rangee(V2(WidthPix -020, 500), (BallRadius*2)+2, (BallRadius*2)+6,  V2(-3, 4), 67),
		Rangee(V2(WidthPix -350, 680), (BallRadius*2)+1, (BallRadius*2)+10, V2(-4,-1), 99),
	};

    vector<Bumper> lBumpers{
        Bumper(V2(200, 400), 40),
        Bumper(V2(400, 400), 40),
        Bumper(V2(300, 550), 40),
    };

    int nScore = 0;

	GameData() {
		PreviousPos.resize(100);  // stocke les x dernières positions connues
		// BallMove = V2(0, 1);

		lRangee[0].addPlusieursCible(6);
		lRangee[1].addPlusieursCible(5);

		lRangee[2].addPlusieursCible(6);
		lRangee[3].addPlusieursCible(5);
	}
};


void Cible::cibleHit(GameData& G) {
    this->nHitTime = 60;
    this->nEtat = TOUCHE;
    G.nScore += 500;
}

void Rangee::rangeeDeactivate(GameData& G) {
    this->nDownTime = 300;
    this->nEtat = DESACTIVE;
    G.nScore += 1111;
}

void Rangee::rangeeHit(GameData& G) {
    int i = 0;
    for (Cible& C : lCibles) {
        if (C.nEtat == ACTIVE) {
            if (DEBUG) cout <<"rangeeHit: Rangee #" <<nFamille <<" remains active, Cible #" <<i <<" is still active" <<". \n";
            return;
        }
        i++;
    }
    rangeeDeactivate(G);
}

void Bumper::bumperHit(GameData& G) {
    this->nHitTime = 240;
    this->nRayonFlash = this->nHitTime / nFlashUnsize;
    this->nEtat = TOUCHE;
    G.nScore += 100;
}




// 0 pas d'intersection
// 1/2/3 intersection entre le segment AB et le cercle de rayon r
int CollisionSegCir(V2 A, V2 B, float r, V2 C)
{
	V2 AB = B - A;
	V2 T = AB;
	T.normalize();
	float d = prodScal(T, C - A);
	if (d > 0 && d < AB.norm())
	{
		V2 P = A + d * T; // proj de C sur [AB]
		V2 PC = C - P;
		if (PC.norm() < r) return 2;
		else               return 0;
	}
	if ((C - A).norm() < r) return 1;
	if ((C - B).norm() < r) return 3;
	return 0;
}

V2 Rebond(V2 V, V2 N) {
	N.normalize();
	V2 T  = V2(N.y, -N.x);  // rotation de 90° du vecteur n sens horaire
	float vt = prodScal(V, T);     // produit scalaire, vt est un nombre
	float vn = prodScal(V, N);     // produit scalaire, vn est un nombre
	V2 R = vt*T - vn*N; // * entre un flottant et un V2
	return R;
}

void verification() {
	V2 R, N;
	N = V2(-1,1);
	cout <<"Rebond sur un mur avec un vecteur normal N = " <<N <<". \n";
	// V=V2( 1, 0);  // -> R = ( 0, 1)
	// V=V2( 1,-1);  // -> R = (-1, 1)
	// V=V2(-1, 1);  // -> R = ( 1,-1)
	// V=V2( 0, 1);  // -> R = ( 1, 0)
	vector<V2> listeV_A{ V2( 1, 0), V2( 1,-1), V2(-1, 1), V2( 0, 1)};
	for (V2 V : listeV_A) {
		R = Rebond(V, N);
		cout <<V <<" -> " <<R << " \n";
	}

	cout <<"\n";
	N = V2(0,1);
	cout <<"Rebond sur un mur avec un vecteur normal N = " <<N <<". \n";
	// V=V2( 1,-1);  // -> R = ( 1, 1)
	// V=V2( 0,-1);  // -> R = ( 0, 1)
	// V=V2(-1, 1);  // -> R = (-1,-1)
	// V=V2(-1,-1);  // -> R = (-1, 1)
	vector<V2> listeV_B{ V2( 1,-1), V2( 0,-1), V2(-1, 1), V2(-1,-1)};
	for (V2 V : listeV_B) {
		R = Rebond(V, N);
		cout <<V <<" -> " <<R << " \n";
	}
}

int DoCollisionSegCir(GameData& G, V2 A, V2 B) {
	int nTypeCollision;
	nTypeCollision = CollisionSegCir(A, B, G.BallRadius, G.BallPos);

	if (nTypeCollision <= 0 || nTypeCollision > 3) return 0;

	if (DEBUG) {
		cout <<"DoCollisionSegCir: " <<G.BallPos <<" car ";
		if (nTypeCollision == 1) cout <<"point A" <<A;
		else if (nTypeCollision == 2) cout <<"ligne AB" <<A <<B;
		else if (nTypeCollision == 3) cout <<"point B" <<B;
		cout <<". \n";
	}

	return nTypeCollision;
}

int DoRebond(GameData& G, V2 A, V2 B, V2& newBallMove) {
    V2 AB = B - A;
    V2 N = V2(AB.y, -AB.x);
    int col = DoCollisionSegCir(G, A, B);
    if (col) {
        newBallMove = Rebond(G.BallMove, N);
    }
    return col;
}

bool CollisionCirCir(float r1, V2 C1, float r2, V2 C2) {
    V2 N = C1 - C2;
    return (N.norm() < r1 + r2);
}

/*----------------------------------------------------------------*/
//
//     fonction de rendu - reçoit en paramètre les données du jeu par référence
void render(const GameData& G) {
		// fond pas noir
	G2D::clearScreen(Color::White);

		// Titre en haut
	// G2D::drawStringFontMono(V2(80, G.HeightPix - 70), string("Super Flipper"), 50, 5, Color::Blue);
    G2D::drawStringFontMono(V2(80 +4, G.HeightPix -70 -4), string("ULTRAFLIPP120"), 50, 5, ColorFrom255(178, 178, 178));
    G2D::drawStringFontMono(V2(80, G.HeightPix - 70), string("ULTRAFLIPP120"), 50, 5, Color::Black);

		// les positions précédentes
	int nPreviousDotsSkip = 0;
	double cIntensity = 0./10;
	for (auto P = G.PreviousPos.rbegin(); P != G.PreviousPos.rend(); ++P) {
		nPreviousDotsSkip = nPreviousDotsSkip % 10;
		if (nPreviousDotsSkip == 0) {
			// G2D::setPixel(P, Color::Black);
			G2D::drawCircle(*P, 4, Color(cIntensity,cIntensity,cIntensity), false);
			cIntensity += 1./10;
			if (cIntensity > 1.0) cIntensity = 1.0; // Clamp to 1.0 to avoid overflow
		}
		nPreviousDotsSkip++;
	}

		// Cibles !
	for (Rangee R : G.lRangee) {
		for (Cible C : R.lCibles) {
			Color cNotActive = ColorFrom255(246, 208, 200);
			Color cActive = ColorFrom255(246, 38, 20);
			Color cHit = ColorFrom255(180, 60, 240);

			Color cEtat = cNotActive;
			if (C.nEtat == ACTIVE) { cEtat = cActive; }
			else if (C.nEtat == TOUCHE) { cEtat = cHit; }

			G2D::drawLine(C.A +V2(0,1), C.B+V2(0,1), cEtat);
			G2D::drawLine(C.A, C.B, cEtat);
			if (C.nEtat == TOUCHE) {
				G2D::drawLine(C.A +V2(0, 2), C.B +V2(0, 2), cEtat);
				G2D::drawLine(C.A +V2(0,-1), C.B +V2(0,-1), cEtat);
			}
		}
	}

        // 3 bumpers
	// Color cBumper = ColorFrom255(10, 139, 255);
	Color cBumper = ColorFrom255(155, 110, 238);
    Color cBumperFlash = ColorFrom255(224, 135, 255);

    for (Bumper B : G.lBumpers) {
        G2D::drawCircle(B.A, B.nRayon +B.nRayonFlash, cBumperFlash, true);
    }
    for (Bumper B : G.lBumpers) {
        // G2D::drawCircle(B.A, B.nRayon +B.nRayonFlash, cBumperFlash, true);
        G2D::drawCircle(B.A, B.nRayon, cBumper, true);
    }

		// La bille
	G2D::drawCircle(G.BallPos, G.BallRadius  , ColorFrom255(248, 119, 39), true);
	G2D::drawCircle(G.BallPos, G.BallRadius-6, ColorFrom255(254, 224, 49), true);


		// Les bords
	for (int i = 0; i < G.LP.size()-1; i++) {
		G2D::drawLine(G.LP[i],G.LP[i+1], ColorFrom255(136, 108, 188));
	}

        // QUATRE LIGNES DE TEST
	for (vector<V2> L : G.Lines) {
        break;
		G2D::drawLine(L[0], L[1], ColorFrom255(59, 107, 48));
	}

		// Préciser la mise en pause
	if ( G2D::isOnPause() ) {
        PAUSE = 1;
        G2D::drawStringFontMono(V2(100+4, (G.HeightPix/2)-4), string("Paused."), 50, 5, ColorFrom255(234, 250, 255));
        G2D::drawStringFontMono(V2(100, G.HeightPix / 2), string("Paused."), 50, 5, ColorFrom255(0, 133, 255));
    } else if (PAUSE == 1) {
        PAUSE = 0;
    }

        // SCORE
    G2D::drawStringFontMono(V2(100+4, (G.HeightPix -100)-4), string(to_string(G.nScore)), 30, 2, ColorFrom255(234, 250, 255));
    G2D::drawStringFontMono(V2(100, G.HeightPix -100), string(to_string(G.nScore)), 30, 2, ColorFrom255(0, 133, 255));


		// Envoyer les tracés à l'écran
	G2D::Show();
}


/*----------------------------------------------------------------*/
//
//      Gestion de la logique du jeu - reçoit en paramètre les données du jeu par référence

void Logic(GameData& G) {
    if (PAUSE == 1) {
        return;
    }
    if (G2D::isKeyPressed(Key::A)) {
		return;
	}
    G.idFrame += 1;
	G.BallPos = G.BallPos + G.BallMove;

	V2 newBallMove = G.BallMove;

        // QUATRE LIGNES DE TEST
	for (vector<V2> L : G.Lines) {
		break;
        DoRebond(G, L[0], L[1], newBallMove);
	}

        // BORDS DU FLIPPER
	for (int i = 0; i < G.LP.size()-1; i++) {
        DoRebond(G, G.LP[i], G.LP[i+1], newBallMove);
	}

        // LES CIBLES
	for (Rangee& R : G.lRangee) {
        R.rangeeRefresh();
        for (Cible& C : R.lCibles) {
            C.cibleRefresh();
            if (C.nEtat==ACTIVE && DoRebond(G, C.A, C.B, newBallMove)) {
                C.cibleHit(G);
                if (DEBUG) cout <<"Logic: Cible hit at position: " <<G.BallPos <<". \n";
                R.rangeeHit(G);
            }
        }
    }

        // LES BUMPERS
    for (Bumper& B : G.lBumpers) {
        // break;
        B.bumperRefresh();
        if (CollisionCirCir(B.nRayon, B.A, G.BallRadius, G.BallPos)) {
            V2 N = B.A - G.BallPos;
            newBallMove = Rebond(G.BallMove, N);
            B.bumperHit(G);
            if (DEBUG) cout <<"Logic: Bumper hit at position: " <<G.BallPos <<". \n";
        }
    }

	G.BallMove = newBallMove;

	G.PreviousPos.push_back(G.BallPos);
	G.PreviousPos.erase(G.PreviousPos.begin());
}

/*----------------------------------------------------------------*/
//
//        Démarrage de l'application
int main(int argc, char* argv[])
{
	float nSpeed;
	if (argc < 1 || argc > 2) {
		cout <<"What. \n";
		return 1;
	} else if (argc == 1) {
		nSpeed = 1;
	} else {
		nSpeed = atoi(argv[1]);
	}

	GameData G;  // instanciation de l'unique objet GameData qui sera passé aux fonctions render et logic

	G2D::initWindow(V2(G.WidthPix, G.HeightPix), V2(000, 1080*2), string("ULTRAFLIPP120"));

	int callToLogicPerSec = 240*nSpeed;
	// callToLogicPerSec = 10;

	G2D::Run(Logic, render, G, callToLogicPerSec,true);
	// verification();

	cout <<"\n";
	return 0;
}
