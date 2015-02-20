//
//  CCPlaySE.h
//  nyan
//
//  Created by 川口 樹 on 2015/02/19.
//
//

#ifndef __nyan__CCPlaySE__
#define __nyan__CCPlaySE__

#include <cocos2d.h>

class CCPlaySE : public cocos2d::CCActionInstant
{
public:
    CCPlaySE(std::string sound);
    virtual ~CCPlaySE(){}
    
    virtual void update(float time); // update関数に処理を記述する
    virtual CCFiniteTimeAction* reverse(void);
    virtual CCObject* copyWithZone(cocos2d::CCZone* pZone);
    
public:
    static CCPlaySE* create(std::string sound); // 効果音のファイル名を指定するcreate関数
    std::string m_sound;
    
};

#endif /* defined(__nyan__CCPlaySE__) */
