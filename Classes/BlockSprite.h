//
//  BlockSprite.h
//  nyan
//
//  Created by 川口 樹 on 2015/02/16.
//
//

#ifndef __nyan__BlockSprite__
#define __nyan__BlockSprite__

#include "cocos2d.h"
#include "config.h"

class BlockSprite : public cocos2d::CCSprite
{
protected:
    const char* getBlockImageFileNmae(kBlock blockType);

public:
    CC_SYNTHESIZE_READONLY(kBlock, m_blockType, BlockType); // コマの種類を返すアクセサ
    CC_SYNTHESIZE_READONLY(int, m_nextPosX, NextPosX);
    CC_SYNTHESIZE_READONLY(int, m_nextPosY, NextPosY);
    
    BlockSprite();
    virtual ~BlockSprite();
    virtual bool initWithBlockType(kBlock blockType);
    static BlockSprite* createWithBlockType(kBlock blockType);
    
    void initNextPos();
    void setNextPos(int nextPosX, int nextPosY);
    
};

#endif /* defined(__nyan__BlockSprite__) */
