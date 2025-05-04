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
#include <cmath>  // M_PI

class Image2 {
private:
    std::string sFilePath;
    Transparency T;
    V2 vImageSize;  // Transparency:: None, BottomLeft, BottomRight, UpperLeft, UpperRight
    V2 vImageAnchor;
    float fAngle;

public:
    Image2(const std::string& sPath,
           V2 vAnchor,
           Transparency T = Transparency::BottomLeft,
           V2 vSize = V2(96, 96),
           float alpha = 0.0f) :
        sFilePath(sPath), vImageAnchor(vAnchor), T(T), vImageSize(vSize), fAngle(alpha) {

    }

    void setSize(const V2& vNewSize) {
        vImageSize = vNewSize;
    }

    void addAngleDeg(const float fAngleToAdd) {
        this->fAngle += fAngleToAdd;
    }

    void addAngleRad(const float fAngleToAdd) {
        this->addAngleDeg(fAngleToAdd*(180.0f/M_PI));
    }

    void render(const V2& vPos) const {
        int nTextureId;
        nTextureId = G2D::ExtractTextureFromPNG(this->sFilePath, this->T);
        if (nTextureId <= 0) {
            if (DEBUG) std::cout <<nTextureId <<std::endl;
            std::cerr <<"Image: Error loading texture from " <<sFilePath <<std::endl;
        } // if

        V2 vImagePos = vPos +V2(0, -vImageSize.y) +V2(-vImageAnchor.x, +vImageAnchor.y);

        // V2 vImagePos = vPos;
        V2 vNewPos= vPos -vImageSize/2;;

        G2D::drawRectWithTexture(nTextureId, vImagePos, vImageSize, fAngle);
        // G2D::drawRectWithTexture(nTextureId, vNewPos, vImageSize, fAngle);
        G2D::setPixel(vNewPos, Color::Blue);
    }
};

class Cursor {
private:
    Image2 cursorImage, pointerImage;

public:
    Cursor(const std::string& cursorPath, const std::string& pointerPath,
           V2 cursorAnchor, V2 pointerAnchor) :
        // cursorImage(cursorPath, cursorAnchor/*, Transparency::None*/), pointerImage(pointerPath, pointerAnchor/*, Transparency::None*/) {
        cursorImage(cursorPath, cursorAnchor), pointerImage(pointerPath, pointerAnchor) {
        // cursorImage(cursorPath, cursorAnchor, Transparency::None), pointerImage(pointerPath, pointerAnchor, Transparency::None) {

    }

    void addAngleDeg(const float fAngleToAdd) {
        cursorImage.addAngleDeg(fAngleToAdd);
        pointerImage.addAngleDeg(fAngleToAdd);
    }

    void addAngleRad(const float fAngleToAdd) {
        this->addAngleDeg(fAngleToAdd*(180.0f/M_PI));
    }

    void renderPosition(const V2& vPosition, const V2& vCoordinates, bool bSelectCondition) const {
        G2D::drawStringFontMono(vPosition,
                                std::string(vCoordinates), 16, 2,
                                bSelectCondition ? Color::Red : Color::Black);
    }

    void render(const V2& vPosition, bool bSelectCondition) const {
        if (bSelectCondition) {
            pointerImage.render(vPosition);
        } else {
            cursorImage.render(vPosition);
        }
        if (DEBUG) G2D::setPixel(vPosition, Color::Red);
    }
};

class Puyo {
private:

public:
    V2 vPosition;
    char cColor;// = '.';

    Puyo(V2 P, char C) : vPosition(P), cColor(C) {}
    // Puyo(V2 P) : vPosition(P), cColor('.') {}

    Color charToColor() const {
        std::unordered_map<char, Color> umColors;
        umColors['.'] = Color::Gray;
        umColors['R'] = Color::Red;
        umColors['G'] = Color::Green;
        umColors['B'] = Color::Blue;

        return umColors[cColor];
    }

    void render(V2 vSize) const {
        // if (DEBUG && cColor != '.') std::cout <<"Puyo.render: current pos is " <<vPosition.x <<", " <<vPosition.y <<std::endl;
        Color C = charToColor();
        G2D::drawRectangle(vPosition, vSize, C, true);
    }

    V2 getPosition() { return vPosition; }
    char getColor() { return cColor; }

    void setPosition(V2 P) { vPosition = P; }
    void addPosition(V2 P) {
        // if (DEBUG) std::cout <<"Puyo.setPosition: current pos is " <<vPosition <<std::endl;
        vPosition = vPosition +P;
        // if (DEBUG) std::cout <<"Puyo.setPosition: current pos is " <<vPosition <<std::endl;
    }

};

class PuyoPair {
private:


public:
    Puyo xPuyoOne;
    Puyo xPuyoTwo;
    int nRotationState;

    PuyoPair(V2 vPos1, char c1, char c2, int nPuyoSize)
    : xPuyoOne(vPos1, c1), xPuyoTwo(vPos1 +V2(0, nPuyoSize), c2), nRotationState(0) {}

    void rotateClockwise() {
        nRotationState = (nRotationState+1)%4;
    }

    void move(V2 vDirection) {

        if (DEBUG) {
            std::cout << "PuyoPair.move: PuyoPair moved: "
            << "Puyo1 at " << std::string(xPuyoOne.getPosition()) << ", "
            << "Puyo2 at " << std::string(xPuyoTwo.getPosition()) << std::endl
            ;
        }
        xPuyoOne.addPosition(vDirection);
        xPuyoTwo.addPosition(vDirection);
    }

    void render(V2 vPuyoSize) const {
        // if (DEBUG) {
        //     std::cout <<"PuyoPair.render: Rendering PuyoPair: "
        //     <<"Puyo1 at " <<xPuyoOne.vPosition.x << ", " <<xPuyoOne.vPosition.y <<" and "
        //     <<"Puyo2 at " <<xPuyoTwo.vPosition.x << ", " <<xPuyoTwo.vPosition.y <<std::endl;
        // }
        xPuyoOne.render(vPuyoSize);
        xPuyoTwo.render(vPuyoSize);
    }
};

class Grid {
private:
    const int nWidth = 6;
    const int nHeight = 12;
    std::vector<std::vector<char>> llChar;
    V2 vGridPosition;
    const int nPuyoSize = 40;
    const V2 vPuyoSize = V2(nPuyoSize, nPuyoSize);
    PuyoPair xPiece;

    PuyoPair initialPiece() {
        V2 vLocation = vGridPosition +V2(nPuyoSize*3, nPuyoSize*(nHeight-3));
        return PuyoPair(vLocation, 'B', 'R', nPuyoSize);
    }

public:
    // Grid(V2 P) : vGridPosition(P), xPiece(initialPiece()) {
    Grid(V2 P) : vGridPosition(P), xPiece(PuyoPair(vGridPosition +V2(nPuyoSize*3, nPuyoSize*(nHeight-3)), 'B', 'R', nPuyoSize)) {
        llChar.resize(nWidth, std::vector<char>(nHeight, '.'));
    }

    int getPuyoSize() { return nPuyoSize; }

    void moveGrid(V2 vDirection) {
        this->vGridPosition = this->vGridPosition + vDirection;
    }

    bool isPositionValid(const V2 vP) const {
        int nActualWidth = vGridPosition.x +nPuyoSize*nWidth;
        int nActualHeight = vGridPosition.y +nPuyoSize*nHeight;

        if (vP.x < vGridPosition.x || vP.x >= nActualWidth || vP.y < vGridPosition.y || vP.y >= nActualHeight) {
            return false;
        }

        return (llChar[vP.y][vP.x] == '.');
    }

    bool placePuyo(Puyo P) {
        V2 vP = P.getPosition();

        if (vP.x < 0 || vP.x >= this->nWidth || vP.y < 0 || vP.y >= this->nHeight) {
            if (DEBUG) std::cout << "Grid.placePuyo: OOB position: " << vP << std::endl;
            return false;
        } else if (llChar[vP.y][vP.x] != '.') {
            if (DEBUG) std::cout << "Grid.placePuyo: Occupied position: " << vP << std::endl;
            return false;
        }
        // llChar[vP.y][vP.x] = charToColor();
        llChar[vP.y/nPuyoSize][vP.x/nPuyoSize] = P.getColor();
        return true;
    }

    bool movePiece(V2 vDirection) {
        // V2 pos1 = xPiece.xPuyoOne.getPosition();
        // V2 pos2 = xPiece.xPuyoTwo.getPosition();
        //
        // V2 newPos1 = pos1 + vDirection;
        // V2 newPos2 = pos2 + vDirection;

        // if (isPositionValid(newPos1) && isPositionValid(newPos2)) {
        if (1) {
            if (DEBUG) { std::cout <<"Moving to " <<vDirection <<std::endl; }
            xPiece.move(vDirection);
            return true;
        }

        return false;
    }




    void coutDisplay() const {
        int x,y;
        for (y=0; y < this->nHeight; ++y) {
            for (x=0; x < this->nWidth; ++x) {
                std::cout << llChar[x][y] << ' ';
            }
            std::cout << std::endl;
        }
    }

    void G2DDisplay() const {
        int x, y;
        V2 vPuyoPos;
        for (y = 0; y < this->nHeight; ++y) {
            for (x = 0; x < this->nWidth; ++x) {
                vPuyoPos = vGridPosition + V2(this->nPuyoSize * x, this->nPuyoSize * y);
                Puyo P(vPuyoPos, llChar[x][y]);
                P.render(vPuyoSize);
                G2D::drawRectangle(vPuyoPos, vPuyoSize, Color::Red, false);
            }
        }
        xPiece.render(vPuyoSize);
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
private:
    std::unordered_map<int, Key> keybinds;

public:
    std::vector<Grid> lGrids;

    Player() {}

    void addGrid(Grid& G) {
        this->lGrids.push_back(G);
    }

    void sendInput() {
        if (lGrids.empty()) return;

        for (std::pair<int, Key> P : keybinds) {
            if (G2D::keyHasBeenHit(P.second)) {
                if (DEBUG) std::cout << "Key pressed: " << P.first << std::endl;

                for (Grid& G : lGrids) {
                    switch (P.first) {
                        case UP:
                            // G.xPiece.rotateClockwise();
                            G.movePiece(V2(0, +G.getPuyoSize()));
                            break;
                        case DOWN:
                            G.movePiece(V2(0, -G.getPuyoSize()));
                            break;
                        case LEFT:
                            G.movePiece(V2(-G.getPuyoSize(), 0));
                            break;
                        case RIGHT:
                            G.movePiece(V2(G.getPuyoSize(), 0));
                            break;
                        case OK:
                            break;
                        case NO:
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    void setKeys(std::unordered_map<int, Key> keyMap) {
        for (std::pair<int, Key> K : keyMap) {
            keybinds[K.first] = K.second;
        }
    }
};

// ON TOP
// struct GameData
class GameData {
private:
    int nID;

public:
    int nFrame;
    int nWidth, nHeight;
    int nScore;
    Player xPlayer;
    std::vector<Cursor> lCursors;
    // std::vector<Grid> lGrids;

    GameData(const int ID, Player P)
    // : nID(ID), nFrame(0), nHeight(1280/2), nWidth(720/2), nScore(0), xPlayer(P) {
    : nID(ID), nFrame(0), nWidth(1280), nHeight(720), nScore(0), xPlayer(P) {
    }

    void function1(const int nValue);

    void addCursor(Cursor C) {
        this->lCursors.push_back(C);
    }

    // void addGrid(Grid& G) {
    //     this->lGrids.push_back(G);
    // }

    void render() const {
        for (Grid G : xPlayer.lGrids) {
            G.G2DDisplay();
            // G.coutDisplay();
        }
    }
};

void render(const GameData& G);

void Logic(GameData& G);


//  P  : mets en pause
// ESC : ferme la fenêtre et quitte le jeu

/*----------------------------------------------------------------*/
void GameData::function1(const int nValue) {
    std::cout <<"GameData" <<nID <<": function1(" <<nValue <<") speaking here." <<" \n";
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

    // Cursor cursorA("M/default2.png", "M/pointer2.png", V2(12, 12), V2(43, 15));
    // Cursor cursorB("M/default2.png", "M/pointer2.png", V2(12, 12), V2(43, 15));


    // G.gGrid.G2DDisplay();
    G.render();






    G2D::drawStringFontMono(V2(80 +4, G.nHeight -70 -4), std::string("test"), 50, 5, ColorFrom255(178, 178, 178));
    G2D::drawStringFontMono(V2(80, G.nHeight - 70), std::string("test"), 50, 5, Color::Black);




    G.lCursors[0].render(vMousePos, G2D::isMouseLeftButtonPressed());
    G.lCursors[0].renderPosition(V2(0, 00), vMousePos, G2D::isMouseLeftButtonPressed());

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
    if (G2D::keyHasBeenHit(Key::B)) {
        G.GameData::function1(55);
    }

    if (G2D::isKeyPressed(Key::Z)) {
        G.lCursors[0].addAngleRad(M_PI/180.0f);
    }
    if (G2D::isKeyPressed(Key::R)) {
        G.lCursors[0].addAngleDeg(-1);
    }

    G.xPlayer.sendInput();



    // G.gGrid.vPosition = G.gGrid.vPosition + V2(1, 0);
    // G.gGrid.moveGrid(V2(1, 0));



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
        double argv1 = std::stod(argv[1]);


        nSpeed = argv1;
    }

    Grid grid001 = Grid(V2(110, 140));
    Grid grid002 = Grid(V2(410, 140));

    // Player P1 = Player(V2(20, 80));
    Player P1 = Player();
    P1.addGrid(grid001);
    P1.addGrid(grid002);

    std::unordered_map<int, Key> keyMap;
    keyMap[UP] = Key::E;
    keyMap[LEFT] = Key::S;
    keyMap[DOWN] = Key::D;
    keyMap[RIGHT] = Key::F;
    keyMap[OK] = Key::J;
    keyMap[NO] = Key::K;
    P1.setKeys(keyMap);


    GameData G1 = GameData(1, P1);

    Cursor cursorA = Cursor("M/default2.png", "M/pointer2.png", V2(12, 12), V2(43, 15));
    Cursor cursorB = Cursor("M/default2.png", "M/pointer2.png", V2(12, 12), V2(43, 15));

    G1.addCursor(cursorA);
    G1.addCursor(cursorB);
    // G1.lCursors.emplace_back(Cursor("M/default2.png", "M/pointer2.png", V2(12, 12), V2(43, 15)));
    // G1.lCursors.emplace_back(Cursor("M/default2.png", "M/pointer2.png", V2(12, 12), V2(43, 15)));

    // G1.addGrid(grid001);

    G2D::initWindow(V2(G1.nWidth, G1.nHeight), V2(200, 100), std::string("test1"), std::string("le test1"));

    int callToLogicPerSec = 240*nSpeed;
    if (DEBUG) std::cout <<"callToLogicPerSec: " <<callToLogicPerSec <<" \n";
    G2D::Run(Logic, render, G1, callToLogicPerSec, true);

    std::cout <<"\n";
    return 0;
}

