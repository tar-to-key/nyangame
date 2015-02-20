//
//  Gamescene.cpp
//  nyan
//
//  Created by 川口 樹 on 2015/02/10.
//
//

#include "Gamescene.h"
#include "SimpleAudioEngine.h"
#include "BlockSprite.h"
#include "CCPlaySE.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

CCScene* GameScene::scene()
{
    CCScene* scene = CCScene::create();
    GameScene* layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

// 初期化
bool GameScene::init()
{
    if (!CCLayer::init())
    {
        return false;
    }
    // タップイベントを取得する
    setTouchEnabled(true); // タップイベント有効
    setTouchMode(kCCTouchesOneByOne); //シングルタップのイベント

    // バックキー・メニューキーイベントを取得する
    
    // 変数初期化
    initForVariables();
    
    // 背景を表示
    showBackground();
    // コマを表示
    showBlock();
    // ラベル作成
    showLabel();
    // リセットボタンの作成
    showResetButton();
    
    // 交換の事前読み込み
    SimpleAudioEngine::sharedEngine()->preloadEffect(MP3_REMOVE_BLOCK);
    
    return true;
}

void GameScene::showBackground()
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    // 背景を生成
    m_background=CCSprite::create(PNG_BACKGROUND);
    m_background->setPosition(ccp(winSize.width/2, winSize.height/2));
    addChild(m_background, kZOrderBackground, kTagBackground);
}

// 変数の初期化
void GameScene::initForVariables()
{
    // 乱数の初期化
    srand((unsigned)time(NULL));
    // コマの一辺の長さを取得
    BlockSprite* pBlock = BlockSprite::createWithBlockType(kBlockRed);
    m_blockSize = pBlock->getContentSize().height;
    
    // コマ種類の配列生成
    blockTypes.push_back(kBlockRed);
    blockTypes.push_back(kBlockBlue);
    blockTypes.push_back(kBlockYellow);
    blockTypes.push_back(kBlockGreen);
    blockTypes.push_back(kBlockGray);
    
    // アニメーション状態変数を初期化
    m_animating = false;
    
    m_score = 0;
}

// ブロックのポジションを取得
CCPoint GameScene::getPosition(int posIndexX, int posIndexY)
{
    float offsetX = m_background->getContentSize().width * 0.168;
    float offsetY = m_background->getContentSize().height * 0.029;
    return CCPoint((posIndexX+0.5)*m_blockSize+offsetX,(posIndexY+0.5)*m_blockSize+offsetY);
}

int GameScene::getTag(int posIndexX, int posIndexY)
{
    return kTagBaseBlock + posIndexX * 100 + posIndexY;
}

void GameScene::showBlock()
{
    // 8x8のコマを作成する
    for (int x=0; x < MAX_BLOCK_X; x++)
    {
        for (int y = 0; y < MAX_BLOCK_Y; y++) {
            // ランダムでコマを作成
            kBlock blockType = (kBlock)(rand() % kBlockCount);
            // 対応するコマ配列にタグを追加
            int tag = getTag(x, y);
            m_blockTags[blockType].push_back(tag);
            
            //コマを作成
            BlockSprite* pBlock = BlockSprite::createWithBlockType(blockType);
            pBlock->setPosition(getPosition(x, y));
            m_background->addChild(pBlock, kZOrderBlock, tag);
        }
    }
        
    
}

// タッチ開始イベント
bool GameScene::ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent *pEvent)
{
    return !m_animating;
}

// タッチ終了イベント
void GameScene::ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent *pEvent)
{
    // タップポイント取得
    CCPoint touchPoint = m_background->convertTouchToNodeSpace(pTouch);
    // タップしたコマのTagとコマの種類を取得
    int tag = 0;
    kBlock blockType;
    // 座標からタグとブロックの色を取得(参照渡しなので注意)
    getTouchBlocktag(touchPoint, tag, blockType);
    
    if (tag !=0) {
        // 隣接するコマを検索する
        list<int> sameColorBlockTags = getSameColorBlockTags(tag, blockType);
        
        if (sameColorBlockTags.size() > 1) {
            m_score += pow(sameColorBlockTags.size() - 2, 2);
            // アニメーション開始
            m_animating = true;

            // 隣接するコマを削除する
            removeBlock(sameColorBlockTags, blockType);
            // コマ削除後のアニメーション
            movingBlocksAnimation1(sameColorBlockTags);
        }
    }
    
}

// タップされたコマのタグを取得
void GameScene::getTouchBlocktag(CCPoint touchPoint, int &tag, kBlock &blockType)
{
    // 全コマループ
    for (int x=0; x<MAX_BLOCK_X; x++) {
        for (int y=0; y<MAX_BLOCK_Y; y++) {
            int currentTag = getTag(x, y);
            // コマを取得(タグを元に取得)
            CCNode* node = m_background->getChildByTag(currentTag);
            // そのコマが存在し、なおかつタッチポイントがコマ範囲に含まれるか判定
            if (node && node->boundingBox().containsPoint(touchPoint)) {
                // 参照渡しなのでこれで値をセットしている
                tag = currentTag;
                blockType = ((BlockSprite*)node)->getBlockType(); // TODO
                return;
            }
        }
    }
}

// コマ配列にあるか検索
bool GameScene::hasSameColorBlock(std::list<int> blockTagList, int searchBlockTag)
{
    list<int>::iterator it;
    // blockTagListは存在するブロックのタグリストのハズ,タッチされたタグが本当に存在しているか判定？
    for (it=blockTagList.begin(); it != blockTagList.end(); ++it) {
        if (*it == searchBlockTag) {
            return true;
        }
    }
    return false;
}

// タップされたコマと同色、かつ接しているコマの配列を返す
list<int> GameScene::getSameColorBlockTags(int baseTag, kBlock blockType)
{
    // 同色のコマを格納する配列の初期化
    list<int> sameColorBlockTags;
    // 最初のタグ(タップされたコマのタグ)を追加
    sameColorBlockTags.push_back(baseTag);
    
    list<int>::iterator it = sameColorBlockTags.begin();
    // イテレータ終了まで回す
    while (it != sameColorBlockTags.end()) {
        int tags[] = {
            *it + 100,
            *it - 100,
            *it + 1,
            *it - 1,
        };
        
        // 隣接数分だけ回す(つまり上下左右)
        for (int i=0; i<sizeof(tags); i++) {
            // 既にリストにあるか検索(これが無いと無限ループ)
            if (!hasSameColorBlock(sameColorBlockTags, tags[i])) {
                // コマ配列にあるか検索 →よく理解できていない感(このタグと何を比較している？)
                // →ちょっと理解タグ[赤][array(1,2,3,4)]の様に同色の連想配列の中身を渡してこのタグと同一なものを探している
                // グルーピング内を全部検索しきっているだけ
                if (hasSameColorBlock(m_blockTags[blockType], tags[i])) {
                    // 同じカラーだった場合、リストへ追加(こうすることでこのタグについてもループ処理で隣接チェックが行われる)
                    sameColorBlockTags.push_back(tags[i]);
                }
            }
        }
        *it++;
    }
    // 隣接している同一カラーのコマのタグ一覧を配列で返す(つまりそのタップで削除されるコマ達のリスト)
    return sameColorBlockTags;
}

// 配列のコマを削除
void GameScene::removeBlock(list<int> blockTags, kBlock blockType)
{
    bool first = true;

    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end()) {
        // 既存配列から該当コマを削除(リストから消すだけ)
        m_blockTags[blockType].remove(*it);
        // 対象となるコマを取得(ここでビューから消す)
        CCNode* block = m_background->getChildByTag(*it);
        if (block) {
            // コマが消えるアニメーションを生成
            CCScaleTo* scale = CCScaleTo::create(REMOVING_TIME, 0);
            // コマを削除するアクションを生成
            CCCallFuncN* func = CCCallFuncN::create(this, callfuncN_selector(GameScene::removingBlock));
            // アクションをつなげる
            // 最後にNULLをつける(おそらくnullが入るまでのアクション全てを順番に再生するため)
            CCFiniteTimeAction* sequence = CCSequence::create(scale, func, NULL);
            
            // 実行アクション定義
            CCFiniteTimeAction* action;
            if (first) {
                // コマが消えるサウンドアクションを作成
                CCPlaySE* playSe = CCPlaySE::create(MP3_REMOVE_BLOCK);
                action = CCSpawn::create(sequence, playSe, NULL);// spawnってなんぞ？
                
                // 初回のみ鳴らす
                first = false;
            }
            else{
                action = sequence;
            }
            // アクションを実行
            block->runAction(action);
        }
        it++;
    }
    SimpleAudioEngine::sharedEngine()->playEffect(MP3_REMOVE_BLOCK);
}

// あえて引数が違うがちょっと実験(サジェストがただしいのかチェック) TODO
void GameScene::removingBlock(cocos2d::CCNode *block)
{
    block->removeFromParentAndCleanup(true);
}

// コマのインデックス取得
GameScene::PositionIndex GameScene::getPositionIndex(int tag)
{
    // タグの数値は位置情報を含むため位置情報は下記の計算式で取得できる
    int pos1_x = (tag - kTagBaseBlock) / 100;
    int pos1_y = (tag - kTagBaseBlock) % 100;
    return PositionIndex(pos1_x, pos1_y);
}

// 新しい位置をセット
// 削除されるコマの上のコマに対して実行される
// 1つぶんだけを行うので注意(ループ文は呼び出し元で行うので１つ削除された処理のみ）
void GameScene::setNewPosition1(int tag, PositionIndex posIndex)
{
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosY = blockSprite->getNextPosY();
    // 次の位置情報が未定義の場合
    if (nextPosY == -1) {
        nextPosY = posIndex.y;
    }
    // 移動先の位置をセット(縦軸だけなのでY座標のみを減らす)
    blockSprite->setNextPos(posIndex.x, --nextPosY);
    
}

// 消えたコマを埋めるように新しい位置をセット
// TODO やってることも意味もわかるが脳内に展開できていない
// TODO 特にposition関連の認識が甘いので理解をすること
void GameScene::searchNewPosition1(list<int> blocks)
{
    // 消えるコマ数分のループ
    // blocksに入っているのは削除対象となるコマの"タグ"
    list<int>::iterator it1 = blocks.begin();

    while (it1 != blocks.end()) {
        // タグから消えるコマの座標(posindex)をとっておく
        PositionIndex posIndex1 = getPositionIndex(*it1);
        vector<kBlock>::iterator it2 = blockTypes.begin();
        // コマの種類(赤、青など)毎にループする
        while (it2 != blockTypes.end()) {
            // 各種類のコマ数分のループ
            // m_blockTags[*it2]というのは「残りの全コマから赤のみ」という感じ
            // 何故一括でやらない？ちょっと実装がよくわからない・・・
            list<int>::iterator it3 = m_blockTags[*it2].begin();

            while (it3 != m_blockTags[*it2].end()) {
                PositionIndex posIndex2 = getPositionIndex(*it3);
                if (posIndex1.x == posIndex2.x && posIndex1.y < posIndex2.y)
                {
                    // 消えるコマの上に位置するコマに対して移動先の位置をセットする
                    setNewPosition1(*it3, posIndex2);
                }
                it3++;
            }
            it2++;
        }
        it1++;
    }
}

void GameScene::moveBlock()
{
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(*it2);
            int nextPosX = blockSprite->getNextPosX();
            int nextPosY = blockSprite->getNextPosY();
            
            if (nextPosX != -1 || nextPosY != -1) {
                // 新しいタグをセットする
                int newTag = getTag(nextPosX, nextPosY);
                blockSprite->initNextPos();
                blockSprite->setTag(newTag);
                *it2 = newTag;
                // アニメーションの値も新しいタグに変更する
                // 元の位置情報がわからない気がするが・・・
                CCMoveTo* move = CCMoveTo::create(MOVING_TIME, getPosition(nextPosX, nextPosY));
                blockSprite->runAction(move);
            }
            it2++;
        }
        it1++;
    }
}

void GameScene::movingBlocksAnimation1(list<int> blocks)
{
    // コマの新しい位置をセットする
    searchNewPosition1(blocks);
    // 新たしい位置がセットされたコマのアニメーション
    moveBlock();
    
    // アニメーション終了時に次(X軸移動)のアニメーションを開始する
    scheduleOnce(schedule_selector(GameScene::movingBlocksAnimation2), MOVING_TIME);
}

// コマの移動完了
void GameScene::movedBlocks()
{
    // ラベル再表示
    showLabel();
    
    // ハイスコア表示
    showHighScoreLabel();

    // アニメーション終了
    m_animating = false;
    
    // ゲーム終了チェック
    if (!exitsSameBlock()) {
        // ハイスコア記録/表示
        saveHighScore();

        CCSize bgSize = m_background->getContentSize();
        // ゲーム終了表示
        CCSprite* gameOver = CCSprite::create(PNG_GAMEOVER);
        gameOver->setPosition(ccp(bgSize.width / 2, bgSize.height*0.8));
        m_background->addChild(gameOver, kZOrderGameOver,kTagGameOver);
        setTouchEnabled(false);
    }
}

// 新しい位置をセット
void GameScene::setNewPosition2(int tag, GameScene::PositionIndex posIndex)
{
    BlockSprite* blockSPrite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosX = blockSPrite->getNextPosX();
    if (nextPosX == -1) {
        nextPosX = posIndex.x;
    }
    // 移動先の位置をセット
    blockSPrite->setNextPos(--nextPosX, posIndex.y);
}

map<int, bool> GameScene::getExistsBlockColumn()
{
    // 検索配列の初期化
    map<int, bool> xBlock;
    for (int i = 0; i < MAX_BLOCK_X; i++) {
        xBlock[i] = false;
    }
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            // 存在するコマのx位置を記録
            xBlock[getPositionIndex(*it2).x] = true;
            it2++;
        }
        it1++;
    }
    return xBlock;
}

void GameScene::searchNewPostion2()
{
    // 存在する列を取得する
    map<int, bool> xBlock = getExistsBlockColumn();
    
    // コマが存在しない位置を埋める
    bool first = true;
    // コマが左方向に移動するので右端から順番に処理する
    for (int i = MAX_BLOCK_X - 1; i >=0; i--) {
        if (xBlock[i]) {
            // コマが存在する
            first = false;
            continue;
        }
        else
        {
            // コマが存在しない
            if (first) {
                continue;
            }
            else{
                // この位置より右側にあるコマを左に１つ寄せる
                
                // コマ種類のループ
                vector<kBlock>::iterator it1 = blockTypes.begin();
                while (it1 != blockTypes.end()) {
                    // 各種類のコマ数分のループ
                    list<int>::iterator it2 = m_blockTags[*it1].begin();
                    while (it2 != m_blockTags[*it1].end()) {
                        PositionIndex posIndex = getPositionIndex(*it2);
                        if (i < posIndex.x) {
                            // 移動先の位置をセットする
                            setNewPosition2(*it2, posIndex);
                        }
                        it2++;
                    }
                    it1++;
                }
            }
        }
    }
}

void GameScene::movingBlocksAnimation2()
{
    // コマの新しい位置をセットする
    searchNewPostion2();
    // 新しい市がセットされたコマのアニメーション
    moveBlock();
    // アニメーション終了時に次の処理を開始する
    scheduleOnce(schedule_selector(GameScene::movedBlocks), MOVING_TIME);

}

void GameScene::showLabel()
{
    CCSize bgSize = m_background->getContentSize();
    // 残数表示
    int tagsForlabel[] = {
        kTagRedlabel,
        kTagBluelabel,
        kTagYellowlabel,
        kTagGreenlabel,
        kTagGraylabel,
    };
    
    
    const char* fontNames[] = {
        FONT_RED,
        FONT_BLUE,
        FONT_YELLOW,
        FONT_GREEN,
        FONT_GRAY,
    };
    
    float heightRare[] = {0.61, 0.51, 0.41, 0.31, 0.21};
    
    // コマ種類のループ
    vector<kBlock>::iterator it = blockTypes.begin();
    while (it != blockTypes.end()) {
        // コマ残数表示
        int count = (int)m_blockTags[*it].size(); // 色ごとの個数
        const char* countStr = ccsf("%02d", count);
        CCLabelBMFont* label = (CCLabelBMFont*)m_background->getChildByTag(tagsForlabel[*it]);
        if (!label) {
            // コマ残数生成
            label = CCLabelBMFont::create(countStr, fontNames[*it]);
            label->setPosition(ccp(bgSize.width * 0.78, bgSize.height * heightRare[*it]));
            m_background->addChild(label, kZOrderLabel, tagsForlabel[*it]);
        }
        else{
            label->setString(countStr);
        }
        it++;
    }
    
    // スコア表示
    const char* scoreStr = ccsf("%d",m_score);
    CCLabelBMFont* scoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagScoreLabel);
    if (!scoreLabel) {
        // スコア生成
        scoreLabel = CCLabelBMFont::create(scoreStr, FONT_WHITE);
        scoreLabel->setPosition(ccp(bgSize.width*0.78, bgSize.height*0.75));
        m_background->addChild(scoreLabel, kZOrderLabel,kTagScoreLabel);
    }
    else{
        scoreLabel->setString(scoreStr);
    }
    
    
}

bool GameScene::exitsSameBlock()
{
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            if (getSameColorBlockTags(*it2, *it1).size() > 1)
            {
                // 隣り合うコマが存在する場合はtrueを返す
                return true;
            }
            it2++;
        }
        it1++;
    }
    // 隣り合うコマが存在しない場合はfalseを返す
    return false;
}

void GameScene::showHighScoreLabel()
{
    CCSize bgSize = m_background->getContentSize();
    
    // ハイスコアの表示
    int highScore = CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_HIGHSCORE, 0);
    
    const char* highScoreStr = ccsf("%d", highScore);
    CCLabelBMFont* highScoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagHiScoreLabel);
    if (!highScoreLabel) {
        // ハイスコア生成
        highScoreLabel = CCLabelBMFont::create(highScoreStr, FONT_WHITE);
        highScoreLabel->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.87));
        m_background->addChild(highScoreLabel, kZOrderLabel, kTagHiScoreLabel);
    }
    else{
        highScoreLabel->setString(highScoreStr);
    }
}

void GameScene::saveHighScore()
{
    CCUserDefault* userDefault = CCUserDefault::sharedUserDefault();
    
    // ハイスコアを取得する
    int oldHighScore = userDefault->getIntegerForKey(KEY_HIGHSCORE, 0);
    if (oldHighScore < m_score) {
        // ハイスコアを保持する
        userDefault->setIntegerForKey(KEY_HIGHSCORE, m_score);
        userDefault->flush();
        // ハイスコアを表示する
        showHighScoreLabel();
    }
}


void GameScene::menuResetCallback(cocos2d::CCObject *pSender)
{
    GameScene* scene = GameScene::create();
    CCDirector::sharedDirector()->replaceScene((CCScene*)scene);
}

void GameScene::showResetButton()
{
    CCSize bgSize = m_background->getContentSize();
    
    // リセットボタンの作成
    CCMenuItemImage* resetButton = CCMenuItemImage::create(PNG_RESET, PNG_RESET, this, menu_selector(GameScene::menuResetCallback));
    resetButton->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.1));
    
    // メニュー作成
    CCMenu* menu = CCMenu::create(resetButton, NULL);
    menu->setPosition(CCPointZero);
    m_background->addChild(menu);
}


// androidバックキーイベント
void GameScene::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}

// andoroidメニューキーイベント
void GameScene::keyMenuClicked()
{
    menuResetCallback(NULL);
}
