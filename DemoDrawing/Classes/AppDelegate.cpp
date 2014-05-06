//
//  DemoDrawingAppDelegate.cpp
//  DemoDrawing
//
//  Created by vuha on 4/16/14.
//  Copyright __MyCompanyName__ 2014. All rights reserved.
//

#include "AppDelegate.h"

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "LineDrawer.h"

USING_NS_CC;
using namespace CocosDenshion;


typedef struct tagResource
{
    cocos2d::CCSize size;
    char directory[100];
}Resource;

static Resource smallResource  =  { cocos2d::CCSizeMake(480, 320),   "iphone" };
static Resource small4Resource  =  { cocos2d::CCSizeMake(1136, 640),   "iphone4inch" };
static Resource mediumResource  =  { cocos2d::CCSizeMake(1024, 768),   "ipad" };
static Resource largeResource  =  { cocos2d::CCSizeMake(2048, 1536),   "ipadhd" };


AppDelegate::AppDelegate()
{

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());

    static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(1024,768);
    
    
    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
    pEGLView->setDesignResolutionSize( designResolutionSize.width, designResolutionSize.height,  kResolutionExactFit);
    
    CCSize frameSize = pEGLView->getFrameSize();
    if (fmod(frameSize.width, small4Resource.size.width)== 0.f){
        pDirector->setContentScaleFactor(small4Resource.size.width/designResolutionSize.width);
    }
    else if ((frameSize.width > mediumResource.size.width))
    {
        pDirector->setContentScaleFactor(largeResource.size.width/designResolutionSize.width);
    }
    // if the frame's height is larger than the height of small resource size, select medium resource.
    else if ((frameSize.width > smallResource.size.width))
    {
        pDirector->setContentScaleFactor(mediumResource.size.width/designResolutionSize.width);
        
    }
    // if the frame's height is smaller than the height of medium resource size, select small resource.
    else
    {
        pDirector->setContentScaleFactor(smallResource.size.width/designResolutionSize.width);
        
    }
    
    CCDirector::sharedDirector()->setContentScaleFactor(1.f);
    

    
    // turn on display FPS
    pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);
    
    
    // create a scene. it's an autorelease object
    CCScene *pScene = LineDrawer::scene();
    
    // run
    pDirector->runWithScene(pScene);
    
    
    return true;
}



// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}
