//
//  LineDrawer.h
//  DemoDrawing
//
//  Created by vuha on 4/24/14.
//
//

#ifndef __DemoDrawing__LineDrawer__
#define __DemoDrawing__LineDrawer__

#include <iostream>
#include "cocos2d.h"
#include "AppMacros.h"
#include "LinePoint.h"
#include "cocos-ext.h"
#include <vector>


USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class LineDrawer:public CCLayer {
    vector<LinePoint*> *points;
    vector<LinePoint*> *circlesPoints;
    
    vector<LinePoint*> *smoothedPoints;
    
    CCSprite *sprite1;
    CCSprite *sprite2;
    
    float overdraw;
    
    CCRenderTexture *renderTexture;
    CCSize winSize;
    
    bool connectingLine;
    bool finishingLine;
    
    void addPoint(CCPoint newPoint, float size);
    void startNewLineFrom(CCPoint newPoint, float size);
    void endLineAt(CCPoint aEndPoint, float size);
    
    vector<LinePoint*>* calculateSmoothLinePoints();
    void drawLines(vector<LinePoint*> *linePoints, ccColor4F color);
    void fillLineTriangles(LineVertex* vertices, int count, ccColor4F color);
    void fillLineEndPointAt(ccVertex2F center, ccVertex2F aLineDir, GLfloat radius, ccColor4F color);
    
    ccVertex2F prevC, prevD;
    ccVertex2F prevG;
    ccVertex2F prevI;
    
    ccColor4F currentColor;
    GLfloat currentRadius;
    
public:
    LineDrawer();
    ~LineDrawer();
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(LineDrawer);
    
    // default implements are used to call script callback if exist
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    // khiem nguyen
    CCArray  *arrayMenuItem;
    CCMenu *menuZoomImage;
    CCMenu* pMenuColor;
    CCMenu *menuSketchxxx;
    CCScrollView *scrollView;
    
    void createMenuZoom(CCPoint _posMenu);
    void menuCloseCallback(CCObject* pSender);
    void menuZoom(CCMenuItemToggle *_item);
    void zoomInWithTag(int _tag);
    void zoomOut();
    void menuSketch(CCMenuItemToggle *_item);
    //end khiem nguyen
    
    // vu ha
    void menuSave(CCMenuItemImage *_item);
    void removeMenuCallback();
    bool isShowMenu;
    // end vu ha
    
    void setColor(ccColor4F _color);
    
    virtual void draw();
    virtual void onEnter();
    virtual void onExit();
    
    void takeScreenShot();
};


class PopupMenu:public CCNode, public CCTargetedTouchDelegate {
    
public:
    PopupMenu();
    ~PopupMenu();
    
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(PopupMenu);
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuCallback(CCMenuItemFont *_item);
    
    virtual void onEnter();
    virtual void onExit();
};

#endif /* defined(__DemoDrawing__LineDrawer__) */
