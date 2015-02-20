//
//  CCPlaySE.cpp
//  nyan
//
//  Created by 川口 樹 on 2015/02/19.
//
//

#include "CCPlaySE.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

// コンストラクタ
CCPlaySE::CCPlaySE(std::string sound)
{
    m_sound = sound;
}

// ファイル名を渡してオブジェクト作成
CCPlaySE* CCPlaySE::create(std::string sound)
{
    CCPlaySE* pRet = new CCPlaySE(sound);
    if (pRet) {
        pRet->autorelease();
    }
    return pRet;
}

// 再生処理
void CCPlaySE::update(float time)
{
    CC_UNUSED_PARAM(time);
    SimpleAudioEngine::sharedEngine()->playEffect(m_sound.c_str());
}


CCFiniteTimeAction* CCPlaySE::reverse()
{
    return (CCFiniteTimeAction*)(CCPlaySE::create(m_sound));
}

//
CCObject* CCPlaySE::copyWithZone(cocos2d::CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCPlaySE* pRet = NULL;
    if (pZone && pZone->m_pCopyObject) {
        pRet = (CCPlaySE*)(pZone->m_pCopyObject);
    }
    else {
        pRet = new CCPlaySE(m_sound);
        pZone = pNewZone = new CCZone(pRet);
    }
    CCActionInstant::copyWithZone(pZone);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}
