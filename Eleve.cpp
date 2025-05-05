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
#include <cstdlib>  // std::rand and std::srand
#include <ctime>  // std::time
#include <unordered_set>  // std::std::unordered_set

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
        // G2D::setPixel(vNewPos, Color::Blue);
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

enum PuyoState {
    IDLE = 0,
    FALLING = 1,
    DISAPPEARING = 2,
    FLOATING = 3
};

class Puyo {
private:
    V2 vPosition;
    char cColor;// = '.';
    int nState;  // PuyoState

public:
    Puyo(V2 P, char C) : vPosition(P), cColor(C), nState(IDLE) {}
    Puyo(V2 P, char C, int S) : vPosition(P), cColor(C), nState(S) {}

    Color charToColor() const {
        std::unordered_map<char, Color> umColors;
        umColors['.'] = ColorFromHex(0xc8c8c8); // #c8c8c8;
        umColors['R'] = Color::Red;
        umColors['G'] = Color::Green;
        umColors['B'] = Color::Blue;

        return umColors[cColor];
    }

    void render(V2 vSize) const {
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
    int nFloatSpeed;
    int nFloatTime;

public:
    Puyo xPuyoOne;
    Puyo xPuyoTwo;
    int nRotationState;

    std::vector<V2> lRotations = {
        V2( 0,  1),
        V2( 1,  0),
        V2( 0, -1),
        V2(-1,  0)
    };


    PuyoPair(V2 vPos1, char c1, char c2, int nPuyoSize)
    : xPuyoOne(vPos1, c1), xPuyoTwo(vPos1 +V2(0, nPuyoSize), c2), nRotationState(0) {}

    void move(V2 vDirection) {
        // if (DEBUG) {
        //     std::cout << "PuyoPair.move: PuyoPair moved: "
        //     << "Puyo1 at " << std::string(xPuyoOne.getPosition()) << ", "
        //     << "Puyo2 at " << std::string(xPuyoTwo.getPosition()) << std::endl
        //     ;
        // }
        xPuyoOne.addPosition(vDirection);
        xPuyoTwo.addPosition(vDirection);
    }

    void movePuyoTwo(int nRotation, int nSize) {
        nRotationState = (nRotationState +nRotation +4)%4;
        // if (DEBUG) std::cout <<"PuyoPair.movePuyoTwo: nRotation=" <<nRotation <<", nRotationState=" <<nRotationState <<" \n";
        xPuyoTwo.setPosition(xPuyoOne.getPosition());
        xPuyoTwo.addPosition(nSize*lRotations[nRotationState]);
    }

    // void addToGrid

    void render(V2 vPuyoSize) const {
        xPuyoOne.render(vPuyoSize);
        xPuyoTwo.render(vPuyoSize);
    }
};

struct V2Hash {
    std::size_t operator()(const V2& v) const {
        return std::hash<int>()(v.x) ^ std::hash<int>()(v.y); // Combine the hashes of x and y
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


public:
    PuyoPair xPiece;

    PuyoPair initialPiece() {
        V2 vLocation = vGridPosition +V2(nPuyoSize*3, nPuyoSize*(nHeight-3));
        std::vector<char> lColors({'R', 'G', 'B'});
        char C1, C2;
        C1 = lColors[std::rand() % 3];
        C2 = lColors[std::rand() % 3];
        return PuyoPair(vLocation, C1, C2, nPuyoSize);
    }


    Grid(V2 P) : vGridPosition(P), xPiece(initialPiece()) {
        llChar.resize(nHeight, std::vector<char>(nWidth, '.'));
    }

    int getPuyoSize() { return nPuyoSize; }

    void moveGrid(V2 vDirection) {
        this->vGridPosition = this->vGridPosition + vDirection;
    }

    bool isPositionValid(const V2 vP) const {
        V2 vP2 = (vP - vGridPosition)/nPuyoSize;
        int nActualWidth = vGridPosition.x +nPuyoSize*nWidth;
        int nActualHeight = vGridPosition.y +nPuyoSize*nHeight;

        if (vP2.x < 0 || vP2.x > nWidth-1 || vP2.y < 0 || vP2.y > nHeight-1) {
            return false;
        }

        return (llChar[vP2.y][vP2.x] == '.');
    }

    V2 placePuyo(Puyo P) {
        V2 vP = P.getPosition();

        V2 vP2 = (vP - vGridPosition)/nPuyoSize;

        if (vP2.x < 0 || vP2.x >= this->nWidth || vP2.y < 0 || vP2.y >= this->nHeight) {
            if (DEBUG) std::cout << "Grid.placePuyo: OOB position: " << vP << std::endl;
            // return;
        } else if (llChar[vP2.y][vP2.x] != '.') {
            if (DEBUG) std::cout << "Grid.placePuyo: Occupied position: " << vP << std::endl;
            // return;
        }

        llChar[vP2.y][vP2.x] = P.getColor();
        return vP2;
    }

    bool movePiece(V2 vDirection) {
        V2 pos1 = xPiece.xPuyoOne.getPosition();
        V2 pos2 = xPiece.xPuyoTwo.getPosition();

        V2 newPos1 = pos1 + vDirection;
        V2 newPos2 = pos2 + vDirection;

        if (isPositionValid(newPos1) && isPositionValid(newPos2)) {
            // if (DEBUG) { std::cout <<"Moving to " <<vDirection <<std::endl; }
            xPiece.move(vDirection);
            return true;
        }

        return false;
    }

    bool rotatePiece(int nRotation) {
        V2 vAddition = nPuyoSize*xPiece.lRotations[(xPiece.nRotationState +nRotation +4)%4];

        V2 pos1 = xPiece.xPuyoOne.getPosition();
        // V2 pos2 = xPiece.xPuyoTwo.getPosition();

        V2 newPos2 = pos1 +vAddition;

        if (isPositionValid(newPos2)) {
            xPiece.movePuyoTwo(nRotation, nPuyoSize);
            return true;
        }

        return false;
    }

    bool hasReachedFloor() {
        V2 pos1 = xPiece.xPuyoOne.getPosition();
        V2 pos2 = xPiece.xPuyoTwo.getPosition();

        // Check if either Puyo is at the bottom of the grid
        if (pos1.y <= vGridPosition.y || pos2.y <= vGridPosition.y) {
            return true;
        }

        // Check if either Puyo is resting on another Puyo
        if (!isPositionValid(pos1 + V2(0, -nPuyoSize)) ||
            !isPositionValid(pos2 + V2(0, -nPuyoSize))) {
            return true;
            }

        return false;
    }

    void movePuyosDownIfEmpty() {
        for (int x = 0; x < nWidth; ++x) {
            for (int y = 1; y < nHeight - 1; ++y) {  // dont check bottom row
                if (llChar[y][x] != '.') {
                    if (llChar[y - 1][x] == '.') {  // if spot below is empty
                        // Move the Puyo down
                        llChar[y - 1][x] = llChar[y][x];  // move down
                        llChar[y][x] = '.';  // clear up
                    }
                }
            }
        }
    }

    int findConnectedPuyos(char C, int x, int y, std::unordered_set<V2, V2Hash>& connectedPuyos) {
        if (x < 0 || x > nWidth -1 || y < 0 || y > nHeight -1) {
            return 0;
        }

        // if current position is empty or already visited
        if (llChar[y][x] != C || connectedPuyos.count(V2(x, y))) {
            return 0;
        }

        connectedPuyos.insert(V2(x, y));

        int nU = findConnectedPuyos(C, x   , y +1, connectedPuyos); // Up
        int nD = findConnectedPuyos(C, x   , y -1, connectedPuyos); // Down
        int nL = findConnectedPuyos(C, x -1, y   , connectedPuyos); // Left
        int nR = findConnectedPuyos(C, x +1, y   , connectedPuyos); // Right

        return 1 +nU +nD +nL +nR;
    }

    void checkConnectedPuyos() {
        for (int x = 0; x < nWidth; ++x) {
            for (int y = 1; y < nHeight - 1; ++y) {  // dont check bottom row
                if (llChar[y][x] == '.') {
                    continue;
                }
                std::unordered_set<V2, V2Hash> lConnectedPuyos;
                int nAmount = findConnectedPuyos(llChar[y][x], x, y, lConnectedPuyos);

                if (nAmount < 4) {
                    continue;
                }

                for (V2 P : lConnectedPuyos) {
                    llChar[P.y][P.x] = '.';
                }
            }
        }
    }


    // ---

    void coutDisplay() const {
        int x,y;
        for (y=0; y < this->nHeight; ++y) {
            for (x=0; x < this->nWidth; ++x) {
                std::cout << llChar[y][x] << ' ';
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
                Puyo P(vPuyoPos, llChar[y][x]);
                P.render(vPuyoSize);
                G2D::drawRectangle(vPuyoPos, vPuyoSize, Color::Gray, false);
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
    CLOCKWISE = 4,
    COUNTERCW = 5,
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
                        case CLOCKWISE:
                            // G.xPiece.rotateClockwise();
                            // G.rotatePiece(V2(+G.getPuyoSize(), 0));
                            G.rotatePiece( 1);
                            break;
                        case COUNTERCW:
                            G.rotatePiece(-1);
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

    GameData(const int ID, Player P)
    : nID(ID), nFrame(0), nWidth(1280), nHeight(720), nScore(0), xPlayer(P) {
    }

    void function1(const int nValue);

    void addCursor(Cursor C) {
        this->lCursors.push_back(C);
    }

    void logic() {
        if (xPlayer.lGrids.empty()) { return; }

        for (Grid& G : xPlayer.lGrids) {
            // if (!G.movePiece(V2(0, -G.getPuyoSize()))) {
                // If the move down was not successful, check if it has reached the floor
                if (G.hasReachedFloor()) {
                    // Add the Puyos to the grid
                    V2 vPuyoOne = G.placePuyo(G.xPiece.xPuyoOne);
                    V2 vPuyoTwo = G.placePuyo(G.xPiece.xPuyoTwo);

                    G.movePuyosDownIfEmpty();

                    G.checkConnectedPuyos();



                    // Create a new PuyoPair for the next piece
                    G.xPiece = G.initialPiece(); // Create a new piece
                // }
            }
        }
    }

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
                                ColorFromHex(0xeafaff) // #eafaff
        );
        G2D::Show();
        return;
    }

    G2D::clearScreen(Color::White);

    int x, y;
    G2D::getMousePos(x, y);
    V2 vMousePos = V2(x, y);

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

    if (G2D::isKeyPressed(Key::Z)) {
        G.lCursors[0].addAngleRad(M_PI/180.0f);
    }
    if (G2D::isKeyPressed(Key::R)) {
        G.lCursors[0].addAngleDeg(-1);
    }

    G.xPlayer.sendInput();

    G.logic();



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
        double argv1 = std::stod(argv[1]);
        nSpeed = argv1;
    }

    Grid grid001 = Grid(V2(110, 140));
    Grid grid002 = Grid(V2(510, 140));
    Grid grid00さん = Grid(V2(910, 140));

    Player P1 = Player();
    P1.addGrid(grid001);
    P1.addGrid(grid002);
    P1.addGrid(grid00さん);

    std::unordered_map<int, Key> keyMap;
    keyMap[UP] = Key::E;
    keyMap[LEFT] = Key::S;
    keyMap[DOWN] = Key::D;
    keyMap[RIGHT] = Key::F;
    keyMap[CLOCKWISE] = Key::L;
    keyMap[COUNTERCW] = Key::J;
    P1.setKeys(keyMap);


    GameData G1 = GameData(1, P1);

    Cursor cursorA = Cursor("M/default2.png", "M/pointer2.png", V2(12, 12), V2(43, 15));

    G1.addCursor(cursorA);

    G2D::initWindow(V2(G1.nWidth, G1.nHeight), V2(200, 100), std::string("test1"), std::string("le test1"));

    int callToLogicPerSec = 240*nSpeed;
    if (DEBUG) std::cout <<"callToLogicPerSec: " <<callToLogicPerSec <<" \n";
    G2D::Run(Logic, render, G1, callToLogicPerSec, true);

    std::cout <<"\n";
    return 0;
}
