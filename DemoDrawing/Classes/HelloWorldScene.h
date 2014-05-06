#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class HelloWorld : public cocos2d::CCLayer
{
public:
    
    HelloWorld();
    ~HelloWorld();
    
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);
    
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    
    
    virtual void draw();
    
    
    // vu ha
    cocos2d::CCSprite *currentCircle;
    void createCircle(float radius, ccColor4F color);
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    
    CCRenderTexture *target;
    
    // end vu ha
    
    
    // kiem nguyen
    CCArray  *arrayMenuItem;
    CCMenu *menuZoomImage;
    CCScrollView *scrollView;
    
    void createMenuZoom();
    void menuZoom(CCMenuItemToggle *_item);
    void zoomInWithTag(int _tag);
    void zoomOut();
    
    //end khiem nguyen
};

#endif // __HELLOWORLD_SCENE_H__
