#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "AppMacros.h"

using namespace cocos2d;
using namespace CocosDenshion;

enum TYPE_ZOOM { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

HelloWorld::HelloWorld(){
    
}

HelloWorld::~HelloWorld(){
    currentCircle->release();
}

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
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
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    this->setTouchMode(kCCTouchesOneByOne);
    this->setTouchEnabled(true);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Thonburi", 34);

    // ask director the window size
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    // position the label on the center of the screen
    pLabel->setPosition( ccp(size.width / 2, size.height - 20) );

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

//    // add "HelloWorld" splash screen"
//    CCSprite* pSprite = CCSprite::create("HelloWorld.png");
//
//    // position the sprite on the center of the screen
//    pSprite->setPosition( ccp(size.width/2, size.height/2) );
//
//    // add the sprite as a child to this layer
//    this->addChild(pSprite, 0);
    
//    CCSprite *sprite1 = CCSprite::create("Images/drawing_original_1.png");
//    sprite1->setPosition(ccp(0,0));
//    this->addChild(sprite1, 0);
    
    //khiem nguyen
    scrollView = CCScrollView::create(size);
    scrollView->setPosition(ccp(0, 0));
    scrollView->setTouchEnabled(false);
    addChild(scrollView,0);
    
    target = CCRenderTexture::create(size.width, size.height, kCCTexture2DPixelFormat_RGBA8888);
    target->setPosition(ccp(size.width / 2, size.height / 2));
    target->clear(1.0f, 1.0f, 1.0f, 1.0f);
    scrollView->addChild(target,1);
    
    this->createCircle(25.0f, ccBLUE4F);
    
    this->createMenuZoom();
    
    return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    this->setScale(2.0f);
}

bool HelloWorld::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
    CCPoint start = pTouch->getLocation();
    start =scrollView->convertTouchToNodeSpace(pTouch);
    
    CCSprite *tmp = CCSprite::createWithTexture(currentCircle->getTexture());
    tmp->setPosition(start);    
    ccBlendFunc blend2 = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA};
    tmp->setBlendFunc(blend2);
    
    target->begin();
    tmp->visit();
    target->end();
    
    return true;
}

void HelloWorld::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
    CCPoint start = pTouch->getLocation();
    start = scrollView->convertToNodeSpace(start);
    CCPoint end = pTouch->getPreviousLocation();
    end = scrollView->convertToNodeSpace(end);
    
    CCSprite *tmp = CCSprite::createWithTexture(currentCircle->getTexture());
    tmp->setPosition(start);
    ccBlendFunc blend2 = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA};
    tmp->setBlendFunc(blend2);
    
    target->begin();
    
    float distance = ccpDistance(start, end);
    
    for (int i = 0; i < distance; i++)
    {
        float difx = end.x - start.x;
        float dify = end.y - start.y;
        float delta = (float)i / distance;
        tmp->setPosition(
                         ccp(start.x + (difx * delta),
                             start.y + (dify * delta)));
        tmp->visit();
    }

    target->end();
}


void HelloWorld::createCircle(float radius, ccColor4F color){
    CCDrawNode *circle = CCDrawNode::create();
    circle->drawDot(ccp(radius, radius), radius, ccc4f(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f));
    
    CCRenderTexture *mRenderTexture = CCRenderTexture::create(radius*2.0f, radius*2.0f, kCCTexture2DPixelFormat_RGBA8888);
    mRenderTexture->begin();
    circle->visit();
    mRenderTexture->end();
    
    auto finalTexture = mRenderTexture->getSprite()->getTexture();
    finalTexture->setAntiAliasTexParameters();
    currentCircle = new CCSprite();
    currentCircle->initWithTexture(finalTexture);
    currentCircle->setTextureRect(CCRectMake(0, 0, finalTexture->getContentSize().width, finalTexture->getContentSize().height));
    currentCircle->setContentSize(finalTexture->getContentSize());
}


void HelloWorld::draw(){
    
}

void HelloWorld::createMenuZoom(){
    CCSprite *sprite3 = CCSprite::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 0.png");
    sprite3->setPosition(ccp(240, sprite3->getContentSize().height / 2));
    addChild(sprite3,3);
    
    CCMenuItemToggle *btnBottomLeft = CCMenuItemToggle::createWithTarget(this,
                                                                         menu_selector(HelloWorld::menuZoom),
                                                                         CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 3 default.png",
                                                                                                 NULL),
                                                                         CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 3.png",
                                                                                                 NULL),NULL);
    btnBottomLeft->setTag(BOTTOM_LEFT);
    btnBottomLeft->setPosition(ccp(btnBottomLeft->getContentSize().width/2, btnBottomLeft->getContentSize().height/2));
    
    CCMenuItemToggle *btnBottomRight = CCMenuItemToggle::createWithTarget(this,
                                                                          menu_selector(HelloWorld::menuZoom),
                                                                          CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 4 default.png",
                                                                                                  NULL),
                                                                          CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 4.png",
                                                                                                  NULL),NULL);
    btnBottomRight->setTag(BOTTOM_RIGHT);
    btnBottomRight->setPosition(ccp(btnBottomRight->getContentSize().width*3/2, btnBottomRight->getContentSize().height/2));
    
    CCMenuItemToggle *btnTopLeft = CCMenuItemToggle::createWithTarget(this,
                                                                      menu_selector(HelloWorld::menuZoom),
                                                                      CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 1 default.png",
                                                                                              NULL),
                                                                      CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 1.png",
                                                                                              NULL),NULL);
    btnTopLeft->setTag(TOP_LEFT);
    btnTopLeft->setPosition(ccp(btnTopLeft->getContentSize().width/2, btnTopLeft->getContentSize().height*3/2));
    
    CCMenuItemToggle *btnTopRight = CCMenuItemToggle::createWithTarget(this,
                                                                       menu_selector(HelloWorld::menuZoom),
                                                                       CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 2 default.png",
                                                                                               NULL),
                                                                       CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 2.png",
                                                                                               NULL),NULL);
    btnTopRight->setTag(TOP_RIGHT);
    btnTopRight->setPosition(ccp(btnTopRight->getContentSize().width*3/2, btnTopRight->getContentSize().height*3/2));
    
    arrayMenuItem = CCArray::create(btnBottomLeft, btnBottomRight,btnTopLeft,btnTopRight,NULL);
    arrayMenuItem->retain();
    menuZoomImage = CCMenu::createWithArray(arrayMenuItem);
    
    menuZoomImage->retain();
    menuZoomImage->setPosition(ccp(3, 6));
    
    sprite3->addChild(menuZoomImage, 5);
}

void HelloWorld::menuZoom(CCMenuItemToggle *_item){
    printf("zoom\n");
    CCObject *object;
    CCARRAY_FOREACH(arrayMenuItem, object){
        CCMenuItemToggle *itemToggle = (CCMenuItemToggle*)object;
        if (itemToggle->getTag() != _item->getTag()) {
            itemToggle->setSelectedIndex(0);
        }
        else{
            if ( itemToggle->getSelectedIndex() == 1) { // zoom in
                itemToggle->setSelectedIndex(1);
                this->zoomInWithTag(_item->getTag());
            }
            else{ //zoom out
                itemToggle->setSelectedIndex(0);
                this->zoomOut();
            }
            
        }
    }
}

void HelloWorld::zoomInWithTag(int _tag){
    
    CCPoint newPos;
    switch (_tag) {
        case TOP_LEFT:
        {
            newPos = ccp((1024*1.8/4.0 - 50), (-768*1.8/4.0 + 40));
        }
            break;
        case TOP_RIGHT:
        {
            newPos = ccp(-(1024*1.8/4.0 - 50), (-768*1.8/4.0 + 40));
        }
            break;
        case BOTTOM_LEFT:
        {
            newPos = ccp(1024*1.8/4.0 - 50, -(-768*1.8/4.0 + 40));
        }
            break;
        case BOTTOM_RIGHT:
        {
            newPos = ccp(-(1024*1.8/4.0 - 50), -(-768*1.8/4.0 + 40));
        }
            break;
        default:
            break;
    }
    
    if (scrollView->getScale() == 1) {
        
        scrollView->runAction(CCSpawn::create(CCScaleTo::create(0.5, 1.8),CCMoveTo::create(0.5, newPos),NULL));
    }
    else{
        scrollView->runAction(CCSpawn::create(CCMoveTo::create(0.5, newPos),NULL));
    }
    
    this->createCircle(12.5f, ccBLUE4F);
}

void HelloWorld::zoomOut(){
    
    if (scrollView->getScale() > 1) {
        scrollView->runAction(CCSpawn::create(CCScaleTo::create(0.5, 1),CCMoveTo::create(0.5, ccp(0, 0)),NULL));
    }
    this->createCircle(25.0f, ccBLUE4F);
}



