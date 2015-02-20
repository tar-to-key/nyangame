//
//  BlockSprite.cpp
//  nyan
//
//  Created by 川口 樹 on 2015/02/16.
//
//

#include "BlockSprite.h"

BlockSprite::BlockSprite()
{
    initNextPos();
}

BlockSprite::~BlockSprite()
{
}

BlockSprite* BlockSprite::createWithBlockType(kBlock blockType)
{
    BlockSprite *pRet=new BlockSprite();
    if (pRet && pRet->initWithBlockType(blockType)) {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool BlockSprite::initWithBlockType(kBlock blockType)
{
    // コマ画像のファイル名の取得
    if (!CCSprite::initWithFile(getBlockImageFileNmae(blockType))) {
        return false;
    }
    m_blockType=blockType;
    return true;
}

const char* BlockSprite::getBlockImageFileNmae(kBlock blockType)
{
    switch (blockType) {
        case kBlockRed:
            return "red.png";
        case kBlockBlue:
            return "blue.png";
        case kBlockYellow:
            return "yellow.png";
        case kBlockGreen:
            return "green.png";
        case kBlockGray:
            return "gray.png";
        default:
            CCAssert(false, "invalid blockType");
            return "";
    }
}


// 移動先の初期化
void BlockSprite::initNextPos()
{
    m_nextPosX = -1;
    m_nextPosY = -1;
}

void BlockSprite::setNextPos(int nextPosX, int nextPosY)
{
    m_nextPosX = nextPosX;
    m_nextPosY = nextPosY;
}

