/*----------------------------------------------------------------*/
/*                      PROJET DJIVAN                             */
/*----------------------------------------------------------------*/
#define DEBUG 1
#include "Eleve.h"

#pragma warning( disable : 4996 )

#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
// #include <map>
#include <unordered_map>

class Image {
private:
    int nTextureId;
    V2 vImageSize;
    V2 vImageAnchor;

public:
    Image(const std::string& sFilePath, Transparency T, V2 vImageSize, V2 vImageAnchor) {
        nTextureId = G2D::ExtractTextureFromPNG(sFilePath, T);
        if (nTextureId < 0) {
            std::cerr <<"Image: Error loading texture from " <<sFilePath <<std::endl;
        }
        this->vImageSize = vImageSize;
        this->vImageAnchor = vImageAnchor;
    }

    Image(const std::string& sFilePath, V2 vImageSize, V2 vImageAnchor) :
    Image(sFilePath, Transparency::BottomLeft, vImageSize, vImageAnchor) {
    }

    Image(const std::string& sFilePath, V2 vImageAnchor) :
    Image(sFilePath, V2(96, 96), vImageAnchor) {
    }

    void setSize(const V2& vNewSize) {
        vImageSize = vNewSize;
    }

    void render(const V2& vPos) const {
        V2 vImagePos = vPos +V2(0, -vImageSize.y) +V2(-vImageAnchor.x, +vImageAnchor.y);
        G2D::drawRectWithTexture(nTextureId, vImagePos, vImageSize);
        // G2D::drawRectWithTexture(nTextureId, vImagePos, vImageSize, 40);
    }
};


class Grid {
private:
    const int nWidth = 6;
    const int nHeight = 12;
    std::vector<std::vector<char>> llGrid;
    V2 vPosition;
    const int nSize = 20;

public:
    Grid(V2 vPosition) :
    vPosition(vPosition) {
        llGrid.resize(nHeight, std::vector<char>(nWidth, '.'));
    }

    void moveGrid(V2 vDirection) {
        this->vPosition = this->vPosition + vDirection;
    }

    bool placePuyo(const int x, const int y, const char color) {
        if (x < 0 || x >= this->nWidth || y < 0 || y >= this->nHeight) {
            if (DEBUG) std::cout <<"Grid.placePuyo: OOB position: " <<x <<',' <<y <<std::endl;
            return false;
        } else if (llGrid[y][x] != '.') {
            if (DEBUG) std::cout <<"Grid.placePuyo: Occupied position: " <<x <<',' <<y <<std::endl;
            return false;
        }
        llGrid[y][x] = color; // Place the Puyo
        return true;
    }

    void coutDisplay() const {
        int x,y;
        for (y=0; y < this->nHeight; ++y) {
            for (x=0; x < this->nWidth; ++x) {
                std::cout << llGrid[y][x] << ' ';
            }
            std::cout << std::endl;
        }
    }

    Color drawPuyo(const char color) const {
        std::unordered_map<char, Color> umColors;
        umColors['.'] = Color::Gray;
        umColors['R'] = Color::Red;
        umColors['G'] = Color::Green;
        umColors['B'] = Color::Blue;

        return umColors[color];
    }

    void G2DDisplay() const {
        int x, y;
        V2 vPuyoPos, vSize;
        vSize = V2(this->nSize, this->nSize);
        for (y=0; y < this->nHeight; ++y) {
            for (x=0; x < this->nWidth; ++x) {
                // G2D::drawRectangle(V2 P1, V2 Size, Color c, bool fill);
                vPuyoPos = vPosition + V2(this->nSize*x, this->nSize*y);

                G2D::drawRectangle(vPuyoPos, vSize, drawPuyo(llGrid[y][x]), true);
                G2D::drawRectangle(vPuyoPos, vSize, Color::White, false);
            }
        }
    }
};



enum /*class*/ Direction {
    UP = 0,
    DOWN = 1,
    RIGHT = 2,
    LEFT = 3,
    OK = 4,
    NO = 5,
};

class Player {
public:
    V2 vPosition;
    Color cColor;
    std::unordered_map<int, Key> keybinds;

    Player(const V2 vPosition) : cColor(ColorFromHex(0xF43B9A)) {
        this->vPosition = vPosition;
        // this->cColor = ColorFromHex(0xF43B9A);
    }

    void move(const int nDirection);

    void setKeys(std::unordered_map<int, Key> keyMap) {
        for (std::pair<int, Key> K : keyMap) {
            keybinds[K.first] = K.second;
        }
    }
};

// ON TOP
// struct GameData
class GameData {
public:
    int nID;
    int nFrame;
    int nWidth, nHeight;
    int nScore;
    Player pPlayer;
    Grid gGrid;

    GameData(const int nID, Player pPlayer, Grid gGrid)
        // : nID(nID), nFrame(0), nHeight(1280/2), nWidth(720/2), nScore(0), pPlayer(pPlayer) {
        : nID(nID), nFrame(0), nWidth(1280), nHeight(720), nScore(0), pPlayer(pPlayer), gGrid(gGrid) {
        // constructor body (if needed)
    }

    void /*GameData::*/function1(const int nValue);

};

void render(const GameData& G);

void Logic(GameData& G);


//  P  : mets en pause
// ESC : ferme la fenêtre et quitte le jeu

/*----------------------------------------------------------------*/
void GameData::function1(const int nValue) {
    std::cout <<"GameData" <<nID <<": function1(" <<nValue <<") speaking here." <<" \n";
}

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
	if ( G2D::isOnPause() ) {
        G2D::clearScreen(Color::Black);

        G2D::drawStringFontMono(V2(100+4, (G.nHeight/2)-4),
                                std::string("Paused."), 50, 5,
                                ColorFromHex(0x0000FF) // #0000FF
        );
        G2D::drawStringFontMono(V2(100, G.nHeight / 2),
                                std::string("Paused."), 50, 5,
                                // ColorFrom255(0, 133, 255)
                                ColorFromHex(0xeafaff) // #eafaff
        );
        G2D::Show();
        return;
    }

    G2D::clearScreen(Color::White);

    int x, y;
    G2D::getMousePos(x, y);
    V2 vMousePos = V2(x, y);



    // V2 vImageSize = V2(96, 96);
    V2 vImageCursorAnchor = V2(12, 12);
    Image cursor = Image("M/default2.png", vImageCursorAnchor);
    // cursor.render(vMousePos);

    V2 vImagePointerAnchor = V2(43, 15);
    Image pointer = Image("M/pointer2.png", vImagePointerAnchor);
    // pointer.render(vMousePos);

    Color cIsLMB;

    if (G2D::isMouseLeftButtonPressed()) {
        pointer.render(vMousePos);
        cIsLMB = Color::Red;
    } else {
        cursor.render(vMousePos);
        cIsLMB = Color::Black;
    } // if
    G2D::drawStringFontMono(V2(0, 10), std::string(vMousePos), 16, 2, cIsLMB);
    G2D::setPixel(vMousePos, Color::Red);



    // G2D::drawCircle(G.pPlayer.vPosition, 10, G.pPlayer.cColor, true);
    Image cursor_B = Image("M/default2.png", vImageCursorAnchor);
    Image pointer_B = Image("M/pointer2.png", vImagePointerAnchor);
    Color cIsSPACE;
    if (G2D::isKeyPressed(Key::SPACE)) {
        pointer_B.render(G.pPlayer.vPosition);
        cIsSPACE = Color::Red;
    } else {
        cursor_B.render(G.pPlayer.vPosition);
        cIsSPACE = Color::Black;
    } // if
    G2D::drawStringFontMono(V2(0, 30), std::string(G.pPlayer.vPosition), 16, 2, cIsSPACE);
    G2D::setPixel(G.pPlayer.vPosition, Color::Red);







    G.gGrid.G2DDisplay();






    G2D::drawStringFontMono(V2(80 +4, G.nHeight -70 -4), std::string("test"), 50, 5, ColorFrom255(178, 178, 178));
    G2D::drawStringFontMono(V2(80, G.nHeight - 70), std::string("test"), 50, 5, Color::Black);

	G2D::Show();
}


/*----------------------------------------------------------------*/
//
//      Gestion de la logique du jeu - reçoit en paramètre les données du jeu par référence

void Logic(GameData& G)
{
    if ( G2D::isOnPause() ) {
        return;
    }

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

    G.gGrid.placePuyo(2, 5, 'R');
    G.gGrid.placePuyo(3, 5, 'G');
    G.gGrid.placePuyo(2, 6, 'R');
    // G.gGrid.G2DDisplay();

    // G.gGrid.vPosition = G.gGrid.vPosition + V2(1, 0);
    G.gGrid.moveGrid(V2(1, 0));


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
        int argv1 = std::stod(argv[1]);


        nSpeed = argv1;
    }



    Player P1 = Player(V2(20, 80));

    std::unordered_map<int, Key> keyMap;
    keyMap[UP] = Key::E;
    keyMap[LEFT] = Key::S;
    keyMap[DOWN] = Key::D;
    keyMap[RIGHT] = Key::F;
    keyMap[OK] = Key::J;
    keyMap[NO] = Key::K;
    P1.setKeys(keyMap);

    // for (std::pair<int, Key> K : P1.keybinds) {
    //     std::cout <<K.first <<" " <<K.second <<std::endl;
    // }


    //EXAMPLE
    Grid grid001(V2(110, 140));
    // grid001.placePuyo(2, 5, 'R');
    // grid001.placePuyo(3, 5, 'G');
    // grid001.placePuyo(2, 6, 'R');
    // grid001.coutDisplay();




	GameData G1 = GameData(1, P1, grid001);
	G2D::initWindow(V2(G1.nWidth, G1.nHeight), V2(200, 100), std::string("test1"), std::string("test1"));

	int callToLogicPerSec = 240*nSpeed;
    if (DEBUG) std::cout <<(int)callToLogicPerSec <<" \n";
	G2D::Run(Logic, render, G1, callToLogicPerSec, true);

	std::cout <<"\n";
	return 0;
}

