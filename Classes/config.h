//
//  config.h
//  nyan
//
//  Created by 川口 樹 on 2015/02/16.
//
//

#ifndef __CONFIG_H__
#define __CONFIG_H__

enum kBlock
{
    kBlockRed,
    kBlockBlue,
    kBlockYellow,
    kBlockGreen,
    kBlockGray,
    kBlockCount,
};

#define ccsf(...) CCString::createWithFormat(__VA_ARGS__)->getCString()

#endif
