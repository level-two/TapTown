#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#define COLS 7
#define ROWS 7
#define WINLEN 4

using namespace cocos2d;

struct PositionData {
    int color;
    int sum;
    bool blocked;
};

class HelloWorld : public CCLayer
{
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);
    
    
    virtual void onEnter();
	virtual void onExit();
    virtual void touchDelegateRetain();
	virtual void touchDelegateRelease();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
//	void registerWithTouchDispatcher();
//    void ccTouchesEnded(CCSet* touches, CCEvent* event);
    
private:
    int board[COLS][ROWS];
    bool winrarShown;
    CCSize s;
    
private:
    void startNewGame();
    bool insertPointInColumn(int column, int color);
    int performAIMove();
    int  findFreeRow(int i);
    void getPositionStrength(float s[]);
    void getStrengthForCoordinate(int i, int j, float s[]);
    void getStrengthInDirection(int i, int j, int di, int dj, PositionData *pd);
    void calculateSum(PositionData &pd1, PositionData &pd2, float s[]);
    int  predictStrengthOfTheMove(int column, int color);
    bool removePointFromColumn(int column);
    bool addPointInColumnAndDraw(int column, int color);
    void showWinrar(int color);
    bool isPositionWinrar(int i, int j);
    void getSum(PositionData &pd1, PositionData &pd2, float s[]);
};

#endif // __HELLOWORLD_SCENE_H__
