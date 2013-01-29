#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#define NONE 0
#define AI 1
#define HUMAN 2

using namespace cocos2d;
using namespace CocosDenshion;



CCScene* HelloWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();
	
	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::create();
    
	// add layer as a child to scene
	scene->addChild(layer);
    
	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	if ( !CCLayer::init() )
	{
		return false;
	}
    
    s = CCDirector::sharedDirector()->getWinSize();
    
    startNewGame();
    
	return true;
}




void HelloWorld::onEnter()
{
    CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    this->setKeypadEnabled(true);
}

void HelloWorld::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void HelloWorld::touchDelegateRetain()
{
	this->retain();
}

void HelloWorld::touchDelegateRelease()
{
	this->release();
}

bool HelloWorld::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
//    return true;
//}
//
//void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
//{
	// Choose one of the touches to work with
//	CCTouch* touch = (CCTouch*)( touches->anyObject() );
    
    if (winrarShown)
    {
        startNewGame();
        return false;
    }
    
    float colWidth = CCDirector::sharedDirector()->getWinSize().width / COLS;
    int column = pTouch->locationInView().x / colWidth;
    
    bool ok = addPointInColumnAndDraw(column, HUMAN);
    if (ok) {
        if (!isPositionWinrar(column, findFreeRow(column)-1)) {
            int aiMove = performAIMove();
            if (isPositionWinrar(aiMove, findFreeRow(aiMove)-1))
               showWinrar(AI);
        }
        else {
            showWinrar(HUMAN);
        }
    }
    return false;
}

void HelloWorld::startNewGame()
{
    winrarShown = false;
    
    for (int i = 0; i< COLS; i++)
        for (int j = 0; j< ROWS; j++)
            board[i][j] = NONE;
    
    removeAllChildrenWithCleanup(false);
    
    float wh = CCDirector::sharedDirector()->getWinSize().height;
    float ww = CCDirector::sharedDirector()->getWinSize().width;
    
    CCSprite *boardSpr = CCSprite::createWithSpriteFrameName("BOARD.png");
    boardSpr->setPosition(ccp( ww/2, wh/2 ));
    boardSpr->setRotation(90);
    addChild(boardSpr);
}

bool HelloWorld::insertPointInColumn(int column, int color)
{
    bool result = false;
    
    for (int j = 0; j< ROWS; j++) {
        if (board[column][j] == NONE) {
            board[column][j] = color;
            result = true;
            break;
        }
    }
    
    return result;
}

bool HelloWorld::removePointFromColumn(int column)
{
    bool result = false;
    
    for (int j = ROWS-1; j>=0; j--) {
        if (board[column][j] != NONE) {
            board[column][j] = NONE;
            result = true;
            break;
        }
    }
    
    return result;
}

int HelloWorld::performAIMove()
{
    int bestMove = 0;
    float min = 1e6;
    
    for (int i=0; i<COLS; i++)
    {
        if (!insertPointInColumn(i, AI))
            continue;
        
        if (isPositionWinrar(i, findFreeRow(i)-1))
        {
            bestMove=i;
            removePointFromColumn(i);
            break;
        }
        
        float max = -1e6;
        for (int k=0; k<COLS; k++)
        {
            if (!insertPointInColumn(k, HUMAN))
                continue;
            
            float s[3];
            getPositionStrength(s);
            
            if (isPositionWinrar(k, findFreeRow(k)-1))
                s[HUMAN] += 10000;
            
            float ds = s[HUMAN]-s[AI];
            if (ds>max) max = ds;
            removePointFromColumn(k);
        }
        if (max<min) { min=max; bestMove=i; }
        removePointFromColumn(i);
    }
     
    addPointInColumnAndDraw(bestMove, AI);
    return bestMove;
}

bool HelloWorld::isPositionWinrar(int i, int j)
{
    PositionData pd1, pd2;
    int color = board[i][j];
    float s[3] = {0,0,0};
    
    getStrengthInDirection(i, j, 1, 0, &pd1);
    getStrengthInDirection(i, j, -1, 0, &pd2);
    getSum(pd1, pd2, s);
    if (s[color]+1 >= 4) return true;
    
    getStrengthInDirection(i, j, 0, -1, &pd1);
    getStrengthInDirection(i, j, 0, 1, &pd2);
    getSum(pd1, pd2, s);
    if (s[color]+1 >= 4) return true;
    
    getStrengthInDirection(i, j, 1, 1, &pd1);
    getStrengthInDirection(i, j, -1, -1, &pd2);
    getSum(pd1, pd2, s);
    if (s[color]+1 >= 4) return true;
    
    getStrengthInDirection(i, j, -1, 1, &pd1);
    getStrengthInDirection(i, j, 1, -1, &pd2);
    getSum(pd1, pd2, s);
    if (s[color]+1 >= 4) return true;
    
    return false;
}

void HelloWorld::getSum(PositionData &pd1, PositionData &pd2, float s[])
{
    s[0] = s[1] = s[2] = 0;
    s[pd1.color]+=pd1.sum;
    s[pd2.color]+=pd2.sum;
}

void HelloWorld::getPositionStrength(float s[])
{
    s[0]=0; s[1]=0; s[2]=0;
    
    for (int i=0; i<COLS; i++)
    {
        int j = findFreeRow(i);
        getStrengthForCoordinate(i, j, s);
    }
}

int HelloWorld::findFreeRow(int i)
{
    int j=0;
    while (j<ROWS) {
        if (board[i][j] == NONE) break;
        j++;
    }
    return j;
}

void HelloWorld::getStrengthForCoordinate(int i, int j, float s[])
{
    PositionData pd1, pd2;
    
    getStrengthInDirection(i, j, 1, 0, &pd1);
    getStrengthInDirection(i, j, -1, 0, &pd2);
    calculateSum(pd1, pd2, s);
    
    getStrengthInDirection(i, j, 0, -1, &pd1);
    getStrengthInDirection(i, j, 0, 1, &pd2);
    calculateSum(pd1, pd2, s);
    
    getStrengthInDirection(i, j, 1, 1, &pd1);
    getStrengthInDirection(i, j, -1, -1, &pd2);
    calculateSum(pd1, pd2, s);
    
    getStrengthInDirection(i, j, -1, 1, &pd1);
    getStrengthInDirection(i, j, 1, -1, &pd2);
    calculateSum(pd1, pd2, s);
}

void HelloWorld::calculateSum(PositionData &pd1, PositionData &pd2, float s[])
{
    if (pd1.color == pd2.color)
    {
        int sum = pd1.sum + pd2.sum;
        if (pd1.blocked && pd2.blocked && sum >= WINLEN-1) // если блокировано сумма >= трех
            s[pd1.color] += 7.0 * sum;
        else if (!pd1.blocked && !pd2.blocked && sum >= WINLEN-1) // если оба неблокированные
            s[pd1.color] += 10.0 * sum;
        else if (!pd1.blocked && !pd2.blocked) // если оба неблокированные
            s[pd1.color] += 2.0 * sum;
        else if (pd1.blocked != pd2.blocked) // если блокировано с одной стороны
            s[pd1.color] += 1.2 * sum;
    }
    else
    {
        if (pd1.sum == WINLEN-1) // если трешка
            s[pd1.color] += 7.0 * pd1.sum;
        else if (!pd1.blocked && pd2.color == NONE) // если неблокированное с обеих сторон
            s[pd1.color] += 2.0 * pd1.sum;
        else if (pd1.blocked && pd2.color == NONE) // блокировано с одной стороны
            s[pd1.color] += pd1.sum;
        
        if (pd2.sum == WINLEN-1) // если трешка
            s[pd2.color] += 7.0 * pd2.sum;
        else if (!pd2.blocked && pd1.color == NONE) // если неблокированное с обеих сторон
            s[pd2.color] += 2.0 * pd2.sum;
        else if (pd2.blocked && pd1.color == NONE) // блокировано с одной стороны
            s[pd2.color] += pd2.sum;
    }
}

void HelloWorld::getStrengthInDirection(int i, int j, int di, int dj, PositionData *pd)
{
    pd->blocked = false;
    pd->color = NONE;
    pd->sum = 0;
    
    i += di; j += dj;
    
    if (i>=0 && i<COLS && j>=0 && j<ROWS)
        pd->color = board[i][j];
        
    if (pd->color == NONE)
        return;
    
    pd->sum++;
    i+=di; j+=dj;
    
    while (true)
    {
        if (i<0 || i==COLS || j<0 || j==ROWS)
        {
            pd->blocked = true;
            break;
        }
        
        if (board[i][j] == NONE)
            break;
        
        if (board[i][j] != pd->color)
        {
            pd->blocked = true;
            break;
        }
        
        pd->sum++;
        i+=di; j+=dj;
    }
}


bool HelloWorld::addPointInColumnAndDraw(int column, int color)
{
    if (!insertPointInColumn(column, color))
        return false;
    
    int row = ROWS-1;
    while (board[column][row] == NONE && row>=0) row--;
    
    float colWidth = s.width / COLS;
    float rowHeight = s.height / ROWS;
    
	CCSprite* pointSpr = CCSprite::createWithSpriteFrameName(color == AI ? "AI.png" : "HUMAN.png");
    pointSpr->setPosition(ccp( (column+0.5)*colWidth, (row+0.5)*rowHeight) );
    addChild(pointSpr);
    return true;
}

void HelloWorld::showWinrar(int color)
{
    winrarShown = true;
    
    CCLabelTTF *label = CCLabelTTF::create(color==AI ? "The Winrar is Hi" : "The Winrar is You" , "Thonburi", 60);
    addChild(label);
    label->setPosition(ccp(s.width/2, s.height/2));
    label->setColor(ccBLACK);
    
    CCLOG("The Winrar is %s", color==AI ? "Hi" : "You");
}