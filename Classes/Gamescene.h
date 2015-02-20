//
//  Gamescene.h
//  nyan
//
//  Created by 川口 樹 on 2015/02/10.
//
//

#ifndef __nyan__Gamescene__
#define __nyan__Gamescene__

#include <cocos2d.h>
#include "config.h"

#define MAX_BLOCK_X 8
#define MAX_BLOCK_Y 8
#define REMOVING_TIME 0.1f // コマの消滅速度
#define MOVING_TIME 0.2f // コマの移動速度

#define PNG_BACKGROUND "background.png"
#define MP3_REMOVE_BLOCK "removeBlock.mp3"
#define PNG_GAMEOVER "gameover.png"

#define FONT_RED "redFont.fnt"
#define FONT_BLUE "blueFont.fnt"
#define FONT_GREEN "greenFont.fnt"
#define FONT_GRAY "grayFont.fnt"
#define FONT_YELLOW "yellowFont.fnt"
#define FONT_WHITE "whiteFont.fnt"


#define KEY_HIGHSCORE "HighScore"
#define PNG_RESET "reset1.png"

class GameScene : public cocos2d::CCLayer
{
public:
    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(GameScene);

    virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);

    // アンドロイド物理キー
    virtual void keyBackClicked();
    virtual void keyMenuClicked();
    
protected:
    enum kTag
    {
        kTagBackground = 1,
        kTagRedlabel,
        kTagBluelabel,
        kTagYellowlabel,
        kTagGreenlabel,
        kTagGraylabel,
        kTagScoreLabel,
        kTagGameOver,
        kTagHiScoreLabel,
        kTagBaseBlock = 10000, // コマの基準タグ TODO
    };
    enum kZOrder
    {
        kZOrderBackground,
        kZOrderLabel,
        kZOrderBlock, // コマの基準タグ TODO
        kZOrderGameOver,
    };
    
    struct PositionIndex
    {
        PositionIndex(int x1, int y1)
        {
            x = x1;
            y = y1;
        }
        int x;
        int y;
    };
    
    // 2-2-3
    cocos2d::CCSprite* m_background;
    void showBackground();
    
    // 2-2-4
    float m_blockSize; // コマの一辺のサイズを記録
    std::map<kBlock, std::list<int> > m_blockTags; // コマの種類ごとのグループ
    void initForVariables();
    void showBlock();
    cocos2d::CCPoint getPosition(int posIndexX, int posIndexY);
    int getTag(int posIndexX, int posIndexY);

    // 2-2-5
    void getTouchBlocktag(cocos2d::CCPoint touchPoint, int &tag, kBlock &blockType);
    std::list<int> getSameColorBlockTags(int baseTag, kBlock blockType);
    void removeBlock(std::list<int> blocktags, kBlock blockType);
    bool hasSameColorBlock(std::list<int> blockTagList, int searchBlockTag);

    // 2-3-1
    void removingBlock(cocos2d::CCNode* block);

    // 2-3-2
    std::vector<kBlock> blockTypes;
    PositionIndex getPositionIndex(int tag);
    void setNewPosition1(int tag, PositionIndex posIndex);
    void searchNewPosition1(std::list<int> blocks);
    void moveBlock();
    void movingBlocksAnimation1(std::list<int> blocks);
    
    // 2-3-3
    bool m_animating;
    void movedBlocks();
    
    std::map<int, bool> getExistsBlockColumn();
    void searchNewPostion2();
    void setNewPosition2(int tag, PositionIndex posIndex);
    void movingBlocksAnimation2();
    // 2-4-1
    void showLabel();
    // 2-4-2
    int m_score;
    // 2-4-3
    bool exitsSameBlock();
    // 2-4-4
    void saveHighScore();
    void showHighScoreLabel();
    // 2-4-5
    void menuResetCallback(cocos2d::CCObject* pSender);
    void showResetButton();
};
#endif /* defined(__nyan__Gamescene__) */
