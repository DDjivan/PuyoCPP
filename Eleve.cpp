/*----------------------------------------------------------------*/
/*                      PROJET DJIVAN                             */
/*----------------------------------------------------------------*/
#define DEBUG 1
#include "Eleve.h"
int PAUSE = 0;

//  P  : mets en pause
// ESC : ferme la fenêtre et quitte le jeu

/*----------------------------------------------------------------*/
void GameData::function1(const int nValue) {
    std::cout <<"GameData" <<nID <<": function1(" <<nValue <<") speaking here." <<" \n";
}

// void Player::move(const int nDirection)
// {
//     if (nDirection == UP) {
//         this->vPosition = this->vPosition + V2( 0, 1);
//     } else if (nDirection == DOWN) {
//         this->vPosition = this->vPosition + V2( 0,-1);
//     } else if (nDirection == RIGHT) {
//         this->vPosition = this->vPosition + V2( 1, 0);
//     } else if (nDirection == LEFT) {
//         this->vPosition = this->vPosition + V2(-1, 0);
//     }
// }

void Player::move(const int nDirection) {
    switch (static_cast<Direction>(nDirection)) {
        case Direction::UP:
            this->vPosition = this->vPosition + V2(0, 1);
            break;
        case Direction::DOWN:
            this->vPosition = this->vPosition + V2(0, -1);
            break;
        case Direction::RIGHT:
            this->vPosition = this->vPosition + V2(1, 0);
            break;
        case Direction::LEFT:
            this->vPosition = this->vPosition + V2(-1, 0);
            break;
    }
}

/*----------------------------------------------------------------*/
void render(const GameData& G)
{
	G2D::clearScreen(Color::White);

    int x, y;
    G2D::getMousePos(x, y);
    V2 vMousePos = V2(x, y);



    G2D::drawCircle(G.pPlayer.vPosition, 10, G.pPlayer.cColor, true);
    G2D::setPixel(G.pPlayer.vPosition, Color::White);

    // G2D::LoadPNG("M/Breeze-Light-Cursor.png", ?, 96, 96);

    V2 vImageCursorSize = V2(96, 96);
    V2 vImageCursorAnchor = V2(12, 12);
    V2 vImageCursor = vMousePos +V2(0, -vImageCursorSize.y) +V2(-vImageCursorAnchor.x, +vImageCursorAnchor.y);
    // load texture (the bottom left corner is the transparent color)
    int nImageCursor = G2D::ExtractTextureFromPNG("M/default2.png", Transparency::BottomLeft);
    // G2D::drawRectWithTexture(nImageCursorID, G.pPlayer.vPosition, textureSize);
    G2D::drawRectWithTexture(nImageCursor, vImageCursor, vImageCursorSize);

    V2 vImagePointerSize = V2(96, 96);
    V2 vImagePointerAnchor = V2(43, 15);
    V2 vImagePointer = vMousePos +V2(0, -vImagePointerSize.y) +V2(-vImagePointerAnchor.x, +vImagePointerAnchor.y);
    int nImagePointer = G2D::ExtractTextureFromPNG("M/pointer2.png", Transparency::BottomLeft);
    G2D::drawRectWithTexture(nImagePointer, vImagePointer, vImagePointerSize);

    G2D::setPixel(vMousePos, Color::Red);



    G2D::drawStringFontMono(V2(80 +4, G.nHeight -70 -4), std::string("test"), 50, 5, ColorFrom255(178, 178, 178));
    G2D::drawStringFontMono(V2(80, G.nHeight - 70), std::string("test"), 50, 5, Color::Black);


	if ( G2D::isOnPause() ) {
        PAUSE = 1;
        G2D::drawStringFontMono(V2(100+4, (G.nHeight/2)-4), std::string("Paused."), 50, 5, ColorFrom255(234, 250, 255));
        G2D::drawStringFontMono(V2(100, G.nHeight / 2), std::string("Paused."), 50, 5, ColorFrom255(0, 133, 255));
    } else if (PAUSE == 1) {
        PAUSE = 0;
    }

	G2D::Show();
}


/*----------------------------------------------------------------*/
//
//      Gestion de la logique du jeu - reçoit en paramètre les données du jeu par référence

void Logic(GameData& G)
{
    // if (PAUSE == 1) {
    //     return;
    // }
    if (G2D::isKeyPressed(Key::A)) {
		G.GameData::function1(70);
	}
	if (G2D::keyHasBeenHit(Key::C)) {
        G.GameData::function1(55);
    }

    // if (G2D::isKeyPressed(Key::E)) {
    //     G.pPlayer.move(0);
    // }
//     std::map<int, Key> keyMap;
//
//     for (size_t i = 0; i < ESDF.size(); ++i) {
//         keyMap[i] = ESDF[i]; // Use the index as the key
//     }
    // std::map<int, Key> keyMap;
//     keyMap.insert({0, Key::E});
//     keyMap.insert({3, Key::S});
//     keyMap.insert({1, Key::D});
//     keyMap.insert({2, Key::F});

    // keyMap.insert({UP, Key::E});
    // keyMap.insert({LEFT, Key::S});
    // keyMap.insert({DOWN, Key::D});
    // keyMap.insert({RIGHT, Key::F});
    std::unordered_map<int, Key> keyMap;
    keyMap[UP] = Key::E;
    keyMap[LEFT] = Key::S;
    keyMap[DOWN] = Key::D;
    keyMap[RIGHT] = Key::F;

    for (std::pair<int, Key> P : keyMap) {
        if (G2D::isKeyPressed(P.second)) {
            G.pPlayer.move(P.first);
        }
    }

    G.nFrame += 1;
}

/*----------------------------------------------------------------*/
//
//        Démarrage de l'application
int main(int argc, char* argv[])
{
	double nSpeed;
	if (argc < 1 || argc > 2) {
		std::cout <<"What. \n";
		return 1;
	} else if (argc == 1) {
		nSpeed = 1;
	} else {
        // nSpeed = atoi(argv[1]);
        // nSpeed = std::stoi(argv[1]);
        nSpeed = std::stod(argv[1]);
    }

    Player P1 = Player(V2(20, 80));
	GameData G1 = GameData(1, P1);
	G2D::initWindow(V2(G1.nWidth, G1.nHeight), V2(200, 100), std::string("test1"), std::string("test1"));

	int callToLogicPerSec = 240*nSpeed;
    std::cout <<(int)callToLogicPerSec <<" \n";
	G2D::Run(Logic, render, G1, callToLogicPerSec, true);

	std::cout <<"\n";
	return 0;
}

