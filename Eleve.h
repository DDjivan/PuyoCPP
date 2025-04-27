// #pragma warning( disable : 4996 )
//
// #include <cstdlib>
// #include <vector>
// #include <iostream>
// #include <string>
// #include "G2D.h"
// // #include <map>
// #include <unordered_map>
//
// class Image {
// private:
//     int nTextureId;
//     V2 vImageSize;
//     V2 vImageAnchor;
//
// public:
//     Image(const std::string& sFilePath, Transparency T, V2 vImageSize, V2 vImageAnchor) {
//         nTextureId = G2D::ExtractTextureFromPNG(sFilePath, T);
//         if (nTextureId < 0) {
//             std::cerr <<"Error loading texture from " <<sFilePath <<std::endl;
//         }
//         this->vImageSize = vImageSize;
//         this->vImageAnchor = vImageAnchor;
//     }
//
//     Image(const std::string& sFilePath, V2 vImageSize, V2 vImageAnchor) :
//         Image(sFilePath, Transparency::BottomLeft, vImageSize, vImageAnchor) {
//     }
//
//     Image(const std::string& sFilePath, V2 vImageAnchor) :
//         Image(sFilePath, V2(96, 96), vImageAnchor) {
//     }
//
//     void setSize(const V2& vNewSize) {
//         vImageSize = vNewSize;
//     }
//
//     void render(const V2& vPos) const {
//         V2 vImagePos = vPos +V2(0, -vImageSize.y) +V2(-vImageAnchor.x, +vImageAnchor.y);
//         G2D::drawRectWithTexture(nTextureId, vImagePos, vImageSize);
//         // G2D::drawRectWithTexture(nTextureId, vImagePos, vImageSize, 40);
//     }
// };
//
//
//
//
// enum /*class*/ Direction {
//     UP = 0,
//     DOWN = 1,
//     RIGHT = 2,
//     LEFT = 3,
//     OK = 4,
//     NO = 5,
// };
//
// class Player {
// public:
//     V2 vPosition;
//     Color cColor;
//     std::unordered_map<int, Key> keybinds;
//
//     Player(const V2 vPosition) : cColor(ColorFromHex(0xF43B9A)) {
//         this->vPosition = vPosition;
//         // this->cColor = ColorFromHex(0xF43B9A);
//     }
//
//     void move(const int nDirection);
//
//     void setKeys(std::unordered_map<int, Key> keyMap) {
//         for (std::pair<int, Key> K : keyMap) {
//             keybinds[K.first] = K.second;
//         }
//     }
// };
//
// // ON TOP
// // struct GameData
// class GameData {
// public:
//     int nID;
//     int nFrame;
//     int nWidth, nHeight;
//     int nScore;
//     Player pPlayer;
//
//     GameData(const int nID, Player pPlayer)
//     // : nID(nID), nFrame(0), nHeight(1280/2), nWidth(720/2), nScore(0), pPlayer(pPlayer) {
//     : nID(nID), nFrame(0), nWidth(1280), nHeight(720), nScore(0), pPlayer(pPlayer) {
//             // constructor body (if needed)
//     }
//
//     void /*GameData::*/function1(const int nValue);
//
// };
//
// void render(const GameData& G);
//
// void Logic(GameData& G);
