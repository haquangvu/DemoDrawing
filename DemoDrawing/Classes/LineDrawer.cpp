//
//  LineDrawer.cpp
//  DemoDrawing
//
//  Created by vuha on 4/24/14.
//
//

#include "LineDrawer.h"
#include "LinePoint.h"

#define Z_INDEX_1     0
#define Z_INDEX_2     1
#define Z_INDEX_3     2


#define GREEN       0
#define YELLOW      1
#define BROWN       2
#define RED         3
#define PURPLE      4
#define BLUEPURPLE  5
#define BLUE        6
#define WHITE       7
#define CAYCO       8

enum TYPE_ZOOM { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

GLfloat ccVertex2FDot(const ccVertex2F& v1, const ccVertex2F& v2)
{
    return v1.x*v2.x + v1.y*v2.y;
}

GLfloat ccVertex2FLengthSQ(const ccVertex2F& v)
{
    return ccVertex2FDot(v, v);
}

bool ccVertex2FFuzzyEqual(const ccVertex2F& a, const ccVertex2F& b, GLfloat var){
    if(a.x - var <= b.x && b.x <= a.x + var)
        if(a.y - var <= b.y && b.y <= a.y + var)
            return true;
    return false;
}

ccVertex2F ccVertex2FSub(const ccVertex2F& v1, const ccVertex2F& v2)
{
    ccVertex2F newPos1 = {v1.x - v2.x, v1.y - v2.y};
    return newPos1;
}

ccVertex2F ccVertex2FAdd(const ccVertex2F& v1, const ccVertex2F& v2)
{
    ccVertex2F newPos1 = {v1.x + v2.x, v1.y + v2.y};
    return newPos1;
}

ccVertex2F ccVertex2FPerp(const ccVertex2F& v)
{
    ccVertex2F newPos1 = {-v.y, v.x};
    return newPos1;
}

GLfloat ccVertex2FLength(const ccVertex2F& v)
{
    return sqrtf(ccVertex2FLengthSQ(v));
}

ccVertex2F ccVertex2FMult(const ccVertex2F& v, const GLfloat s)
{
    ccVertex2F newPos1 = {v.x*s, v.y*s};
    return newPos1;
}

ccVertex2F ccVertex2FNormalize(const ccVertex2F& v)
{
    
    return ccVertex2FMult(v, 1.0f/ccVertex2FLength(v));
}

GLfloat ccVertex2FDistance(const ccVertex2F& v1, const ccVertex2F& v2)
{
    return ccVertex2FLength(ccVertex2FSub(v1, v2));
}

CCScene* LineDrawer::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    LineDrawer *layer = LineDrawer::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

LineDrawer::LineDrawer(){
    
}


LineDrawer::~LineDrawer(){
    vector<LinePoint*>::iterator iter;
    for (iter = points->begin(); iter != points->end(); ++iter)
    {
        delete *iter;
    }
    delete points;
    
    for (iter = circlesPoints->begin(); iter != circlesPoints->end(); ++iter)
    {
        delete *iter;
    }
    delete circlesPoints;
}


bool LineDrawer::init(){
    if (!CCLayer::init()) {
        return false;
    }
    
    points = new vector<LinePoint*>();
    circlesPoints = new vector<LinePoint*>();
    
    m_pShaderProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionColor);
    overdraw = 3.0f;
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    //khiem nguyen
    scrollView = CCScrollView::create(winSize);
    scrollView->setPosition(ccp(0, 0));
    scrollView->setTouchEnabled(false);
    addChild(scrollView,0);
    
    // end khiem nguyen
    
    CCSprite *sprite1 = CCSprite::create("Images/drawing_original_1.png");
    sprite1->setPosition(ccp(0,0));
    scrollView->addChild(sprite1, Z_INDEX_1);
    
    sprite2 = CCSprite::create("Images/drawing_sketch_1.png");
    sprite2->setPosition(ccp(0,0));
    scrollView->addChild(sprite2, Z_INDEX_3);
    
    renderTexture = new CCRenderTexture();
    renderTexture->initWithWidthAndHeight((int)this->getContentSize().width,
                                          (int)this->getContentSize().height,
                                          kCCTexture2DPixelFormat_RGBA8888);
    renderTexture->setAnchorPoint(ccp(0, 0));
    renderTexture->setPosition(ccp(winSize.width/2, winSize.height/2));
    renderTexture->clear(1.0f, 1.0f, 1.0f, 1.0f);
    scrollView->addChild(renderTexture, Z_INDEX_2);
    renderTexture->release();
    
    this->setTouchMode(kCCTouchesOneByOne);
    this->setTouchEnabled(true);
    
    currentColor = ccc4f(ccBLUE4F.r/255.f, ccBLUE4F.g/255.f, ccBLUE4F.b/255.f, ccBLUE4F.a/255.f);
    currentRadius = RADIUS_MEDIUM;
    
    //create menu brush color
    //create menu
    CCMenuItemImage *pGreen = CCMenuItemImage::create(
                                                      "Images/mau 1_xanh la.png",
                                                      "Images/mau 1_xanh la_touch.png",
                                                      this,
                                                      menu_selector(LineDrawer::menuCloseCallback) );
    pGreen->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - pGreen->getContentSize().width, pGreen->getContentSize().height) );
    pGreen->setTag(GREEN);
    
    CCMenuItemImage *pYellow = CCMenuItemImage::create(
                                                       "Images/mau 3_vang.png",
                                                       "Images/mau 3_vang_touch.png",
                                                       this,
                                                       menu_selector(LineDrawer::menuCloseCallback) );
    pYellow->setPosition( ccp(pGreen->getPositionX() - pGreen->getContentSize().width, pGreen->getContentSize().height) );
    pYellow->setTag(YELLOW);
    
    CCMenuItemImage *pBrown = CCMenuItemImage::create(
                                                      "Images/mau 4_nau.png",
                                                      "Images/mau 4_nau_touch.png",
                                                      this,
                                                      menu_selector(LineDrawer::menuCloseCallback) );
    pBrown->setPosition( ccp(pYellow->getPositionX() - pYellow->getContentSize().width, pGreen->getContentSize().height) );
    pBrown->setTag(BROWN);
    
    CCMenuItemImage *pRed = CCMenuItemImage::create(
                                                    "Images/mau 5_do.png",
                                                    "Images/mau 5_do_touch.png",
                                                    this,
                                                    menu_selector(LineDrawer::menuCloseCallback) );
    pRed->setPosition( ccp(pBrown->getPositionX() - pYellow->getContentSize().width, pGreen->getContentSize().height) );
    pRed->setTag(RED);
    
    CCMenuItemImage *pPurple = CCMenuItemImage::create(
                                                       "Images/mau 6_tim.png",
                                                       "Images/mau 6_tim_touch.png",
                                                       this,
                                                       menu_selector(LineDrawer::menuCloseCallback) );
    pPurple->setPosition( ccp(pRed->getPositionX() - pYellow->getContentSize().width, pGreen->getContentSize().height) );
    pPurple->setTag(PURPLE);
    
    
    CCMenuItemImage *pBluePurple = CCMenuItemImage::create(
                                                           "Images/mau 7_tim xanh.png",
                                                           "Images/mau 7_tim xanh_touch.png",
                                                           this,
                                                           menu_selector(LineDrawer::menuCloseCallback) );
    pBluePurple->setPosition( ccp(pPurple->getPositionX() - pYellow->getContentSize().width, pGreen->getContentSize().height) );
    pBluePurple->setTag(BLUEPURPLE);
    
    CCMenuItemImage *pBlue = CCMenuItemImage::create(
                                                     "Images/mau 8_xanh.png",
                                                     "Images/mau 8_xanh_touch.png",
                                                     this,
                                                     menu_selector(LineDrawer::menuCloseCallback) );
    pBlue->setPosition( ccp(pBluePurple->getPositionX() - pYellow->getContentSize().width, pGreen->getContentSize().height) );
    pBlue->setTag(BLUE);
    
    CCMenuItemImage *pWhite = CCMenuItemImage::create(
                                                      "Images/mau 9_trang.png",
                                                      "Images/mau 9_trang_touch.png",
                                                      this,
                                                      menu_selector(LineDrawer::menuCloseCallback) );
    pWhite->setPosition( ccp(pBlue->getPositionX() - pYellow->getContentSize().width, pGreen->getContentSize().height) );
    pWhite->setTag(WHITE);
    
    CCMenuItemImage *pCayco = CCMenuItemImage::create(
                                                      "Images/button lay mau goc.png",
                                                      "Images/button lay mau goc - touch.png",
                                                      this,
                                                      menu_selector(LineDrawer::menuCloseCallback) );
    pCayco->setPosition( ccp(pWhite->getPositionX() - pCayco->getContentSize().width, pGreen->getContentSize().height) );
    pCayco->setTag(CAYCO);
    
    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pGreen, pYellow, pBrown, pRed, pPurple, pBluePurple, pBlue, pWhite, pCayco, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);
    
    this->createMenuZoom(CCPoint(pCayco->getPositionX() - pCayco->getContentSize().width, pCayco->getPosition().y));
    
    //create menu show/hide sketch
    CCMenuItemToggle *btnSketch = CCMenuItemToggle::createWithTarget(this,
                                                                     menu_selector(LineDrawer::menuSketch),
                                                                     CCMenuItemImage::create("Images/button bo net.png",
                                                                                             NULL),
                                                                     CCMenuItemImage::create("Images/button bo net - touch.png",
                                                                                             NULL),NULL);
    btnSketch->setPosition( CCPoint(pCayco->getPositionX() - pCayco->getContentSize().width*2, pCayco->getPosition().y) );
//    this->addChild(btnSketch, 1);
    
    CCMenuItemToggle *btnSave = CCMenuItemToggle::createWithTarget(this,
                                                                     menu_selector(LineDrawer::menuSave),
                                                                     CCMenuItemImage::create("Images/button save.png",
                                                                                             NULL),
                                                                     CCMenuItemImage::create("Images/button save - touch.png",
                                                                                             NULL),NULL);
    btnSave->setPosition( CCPoint(btnSketch->getPositionX() - btnSave->getContentSize().width*2, btnSketch->getPosition().y) );
    
    CCMenu *menuSketchxxx = CCMenu::create(btnSketch, btnSave, NULL);
    menuSketchxxx->setPosition(CCPointZero);
    this->addChild(menuSketchxxx);
    
    return true;
}

vector<LinePoint*>* LineDrawer::calculateSmoothLinePoints(){
    
    if (points->size() > 2) {
        vector<LinePoint*> *smoothedPoints = new vector<LinePoint*>();
        if (smoothedPoints->size() > 0) {
            smoothedPoints->erase(smoothedPoints->begin(), smoothedPoints->end());
        }
        for (int i=2; i<points->size(); ++i) {
            LinePoint *prev2    = (LinePoint*)points->at(i-2);
            LinePoint *prev1    = (LinePoint*)points->at(i-1);
            LinePoint *cur      = (LinePoint*)points->at(i);
            
            ccVertex2F midPoint1 =  ccVertex2FMult(ccVertex2FAdd(prev1->getPos() , prev2->getPos()), 0.5f);
            ccVertex2F midPoint2 =  ccVertex2FMult(ccVertex2FAdd(cur->getPos()   , prev1->getPos()), 0.5f);
            
            int segmentDistance = 2;
            
            GLfloat distance = ccVertex2FDistance(midPoint1, midPoint2);
            int numberOfSegments = MIN(128, MAX(floorf(distance / segmentDistance), 32));
            
            GLfloat t = 0.0f;
            GLfloat step = 1.0f / numberOfSegments;
            
            for (int j=0 ; j<numberOfSegments; j++) {
                LinePoint *newPoint = new LinePoint();
                ccVertex2F newPos = ccVertex2FAdd(ccVertex2FAdd(ccVertex2FMult(midPoint1, powf(1 - t, 2)),
                                                                ccVertex2FMult(prev1->getPos() , 2.0f * (1 - t) * t)),
                                                  ccVertex2FMult(midPoint2, t * t));
                GLfloat w =  powf(1 - t, 2) * ((prev1->getWidth() + prev2->getWidth()) * 0.5f) + 2.0f * (1 - t) * t * prev1->getWidth() + t * t * ((cur->getWidth() + prev1->getWidth()) * 0.5f);
                ccVertex2F newPos1 = {newPos.x ,newPos.y};
                
                newPoint->setPos(newPos1);
                newPoint->setWidth(w);
                smoothedPoints->push_back(newPoint);
                t+=step;
//                newPoint->release();
            }
            
            LinePoint *finalPoint = new LinePoint();
            ccVertex2F newPos1 = {midPoint2.x ,midPoint2.y};
            finalPoint->setPos(newPos1);
            finalPoint->setWidth((cur->getWidth() + prev1->getWidth()) * 0.5f);
            smoothedPoints->push_back(finalPoint);
//            finalPoint->release();
            
        }
        
        
        LinePoint *finalPoint1 = (LinePoint*)points->at(points->size()-2);
        LinePoint *finalPoint2 = (LinePoint*)points->at(points->size()-1);
        
        LinePoint *tmp1 = new LinePoint();
        tmp1->setPos(finalPoint1->getPos());
        tmp1->setWidth(finalPoint1->getWidth());
        
        LinePoint *tmp2 = new LinePoint();
        tmp2->setPos(finalPoint2->getPos());
        tmp2->setWidth(finalPoint2->getWidth());
        
        vector<LinePoint*>::iterator iter;
        for (iter = points->begin(); iter != points->end(); ++iter)
        {
            
            delete *iter;
        }
        points->erase(points->begin(), points->end());
        points->push_back(tmp1);
        points->push_back(tmp2);
        
        
        return smoothedPoints;
    }
    
    return NULL;
}

#pragma mark - Drawing
#define ADD_TRIANGLE(A, B, C, Z) vertices[index].pos = A, vertices[index++].z = Z, vertices[index].pos = B, vertices[index++].z = Z, vertices[index].pos = C, vertices[index++].z = Z

void LineDrawer::drawLines(vector<LinePoint*> *linePoints, ccColor4F color){
    unsigned int    numberOfVertices    = (linePoints->size() - 1) * 18;
    LineVertex      *vertices           = (LineVertex*)calloc(sizeof(LineVertex), numberOfVertices);
    
    ccVertex2F prevPoint   = ((LinePoint*)linePoints->at(0))->getPos();
    GLfloat   prevValue   = ((LinePoint*)linePoints->at(0))->getWidth();
    GLfloat curValue;
    int index = 0;
    
    for (int i=1 ; i<linePoints->size() ; ++i) {
        LinePoint *pointValue   = (LinePoint*)linePoints->at(i);
        ccVertex2F curPoint     = pointValue->getPos();
        curValue                = pointValue->getWidth();
        
        if (ccVertex2FFuzzyEqual(curPoint, prevPoint, 0.0001f)) {
            continue;
        }
        
        ccVertex2F dir      = ccVertex2FSub(curPoint, prevPoint);
        ccVertex2F dirNor   = ccVertex2FNormalize(ccVertex2FPerp(dir));
        ccVertex2F perpendicular = {dirNor.x ,dirNor.y};
        
        ccVertex2F perMult = ccVertex2FMult(perpendicular, prevValue / 2);
        ccVertex2F curMult = ccVertex2FMult(perpendicular, curValue / 2);
        
        ccVertex2F A = ccVertex2FAdd(prevPoint, perMult);
        ccVertex2F B = ccVertex2FSub(prevPoint, perMult);
        ccVertex2F C = ccVertex2FAdd(curPoint, curMult);
        ccVertex2F D = ccVertex2FSub(curPoint, curMult);
        
        //! continuing line
        if (connectingLine || index > 0) {
            A = prevC;
            B = prevD;
        } else if (index == 0) {
            //! circle at start of line, revert direction
            circlesPoints->push_back(pointValue);
            circlesPoints->push_back(linePoints->at(i-1));
        }
        
        ADD_TRIANGLE(A, B, C, 1.0f);
        ADD_TRIANGLE(B, C, D, 1.0f);
        
        prevD = D;
        prevC = C;
        if (finishingLine && (i == (linePoints->size() - 1))) {
            circlesPoints->push_back(linePoints->at(i - 1));
            circlesPoints->push_back(pointValue);
            finishingLine = false;
        }
        prevPoint = curPoint;
        prevValue = curValue;
        
        //! Add overdraw
        ccVertex2F overValue = ccVertex2FMult(perpendicular, overdraw);
        ccVertex2F F = ccVertex2FAdd(A, overValue);
        ccVertex2F G = ccVertex2FAdd(C, overValue);
        ccVertex2F H = ccVertex2FSub(B, overValue);
        ccVertex2F I = ccVertex2FSub(D, overValue);
        
        
        //! end vertices of last line are the start of this one, also for the overdraw
        if (connectingLine || index > 6) {
            F = prevG;
            H = prevI;
        }
        
        prevG = G;
        prevI = I;
        
        ADD_TRIANGLE(F, A, G, 2.0f);
        ADD_TRIANGLE(A, G, C, 2.0f);
        ADD_TRIANGLE(B, H, D, 2.0f);
        ADD_TRIANGLE(H, D, I, 2.0f);
    }
    
    this->fillLineTriangles(vertices, index, color);
    
    if (index > 0) {
        connectingLine = true;
    }
    free(vertices);
}

void LineDrawer::fillLineTriangles(LineVertex *vertices, int count, ccColor4F color){

    m_pShaderProgram->use();
    m_pShaderProgram->setUniformsForBuiltins();
    
    ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color);
    
    ccColor4F fullColor = color;
    ccColor4F fadeOutColor = color;
    fadeOutColor.a = 0;
    
    for (int i = 0; i < count / 18; ++i) {
        for (int j = 0; j < 6; ++j) {
            vertices[i * 18 + j].color = color;
        }
        
        //! FAG
        vertices[i * 18 + 6].color = fadeOutColor;
        vertices[i * 18 + 7].color = fullColor;
        vertices[i * 18 + 8].color = fadeOutColor;
        
        //! AGD
        vertices[i * 18 + 9].color = fullColor;
        vertices[i * 18 + 10].color = fadeOutColor;
        vertices[i * 18 + 11].color = fullColor;
        
        //! BHC
        vertices[i * 18 + 12].color = fullColor;
        vertices[i * 18 + 13].color = fadeOutColor;
        vertices[i * 18 + 14].color = fullColor;
        
        //! HCI
        vertices[i * 18 + 15].color = fadeOutColor;
        vertices[i * 18 + 16].color = fullColor;
        vertices[i * 18 + 17].color = fadeOutColor;
    }
    
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), &vertices[0].pos);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), &vertices[0].color);
    
    if (currentColor.a == 0) {
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA );
    }
    else{
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)count);
    
    CC_INCREMENT_GL_DRAWS(1);
    
    for (unsigned int i = 0; i < (circlesPoints->size() / 2);   ++i) {
        LinePoint *prevPoint = (LinePoint *)circlesPoints->at(i * 2);
        LinePoint *curPoint = (LinePoint *)circlesPoints->at(i * 2 + 1);
        ccVertex2F dirVector = ccVertex2FNormalize(ccVertex2FSub(curPoint->getPos(), prevPoint->getPos()));
        this->fillLineEndPointAt(curPoint->getPos(), dirVector, curPoint->getWidth() * 0.5f, color);
    }
    circlesPoints->erase(circlesPoints->begin(), circlesPoints->end());
}

void LineDrawer::fillLineEndPointAt(ccVertex2F center, ccVertex2F aLineDir, GLfloat radius, ccColor4F color){

    int numberOfSegments = 32;
    LineVertex *vertices = (LineVertex*)malloc(sizeof(LineVertex) * numberOfSegments * 9);
    float anglePerSegment = (float)(M_PI / (numberOfSegments - 1));
    
    //! we need to cover M_PI from this, dot product of normalized vectors is equal to cos angle between them... and if you include rightVec dot you get to know the correct direction :)
    ccVertex2F perpendicular = ccVertex2FPerp(aLineDir);
    ccVertex2F tmp = {0, 1};
    ccVertex2F tmp1 = {1, 0};
    float angle = acosf(ccVertex2FDot(perpendicular, tmp));
    float rightDot = ccVertex2FDot(perpendicular, tmp1);
    if (rightDot < 0.0f) {
        angle *= -1;
    }
    
    ccVertex2F prevPoint = center;
    ccVertex2F prevDir = {sinf(0), cosf(0)};
    for (unsigned int i = 0; i < numberOfSegments; ++i) {
        ccVertex2F dir = {sinf(angle), cosf(angle)};
        ccVertex2F curPoint = {center.x + radius * dir.x, center.y + radius * dir.y};
        vertices[i * 9 + 0].pos = center;
        vertices[i * 9 + 1].pos = prevPoint;
        vertices[i * 9 + 2].pos = curPoint;
        
        //! fill rest of vertex data
        for (unsigned int j = 0; j < 9; ++j) {
            vertices[i * 9 + j].z = j < 3 ? 1.0f : 2.0f;
            vertices[i * 9 + j].color = color;
        }
        
        //! add overdraw
        vertices[i * 9 + 3].pos = ccVertex2FAdd(prevPoint, ccVertex2FMult(prevDir, overdraw));
        vertices[i * 9 + 3].color.a = 0;
        vertices[i * 9 + 4].pos = prevPoint;
        vertices[i * 9 + 5].pos = ccVertex2FAdd(curPoint, ccVertex2FMult(dir, overdraw));
        vertices[i * 9 + 5].color.a = 0;
        
        vertices[i * 9 + 6].pos = prevPoint;
        vertices[i * 9 + 7].pos = curPoint;
        vertices[i * 9 + 8].pos = ccVertex2FAdd(curPoint, ccVertex2FMult(dir, overdraw));
        vertices[i * 9 + 8].color.a = 0;
        
        prevPoint = curPoint;
        prevDir = dir;
        angle += anglePerSegment;
    }
    
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), &vertices[0].pos);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), &vertices[0].color);
    glDrawArrays(GL_TRIANGLES, 0, numberOfSegments * 9);
    
    free(vertices);
}


void LineDrawer::draw(){
//    CCLog("%f,%f,%f,%f", currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    
    ccColor4F color;
    
    if (currentColor.a == 0) {
        color = ccc4f(0, 0, 0, 1);
    } else {
        color = currentColor;
    }
    
    renderTexture->begin();
    
    vector<LinePoint*> *smoothedPoints = new vector<LinePoint*>();
    smoothedPoints = this->calculateSmoothLinePoints();
    if (smoothedPoints) {
        this->drawLines(smoothedPoints, color);
    }
    renderTexture->end();
}

bool LineDrawer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
    CCPoint pos = pTouch->getLocation();
    pos =scrollView->convertToNodeSpace(pos);
    
    points->erase(points->begin(), points->end());
    GLfloat size = currentRadius;
    this->startNewLineFrom(pos, size);
    
    this->addPoint(pos, size);
    this->addPoint(pos, size);
    
    return true;
}

void LineDrawer::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
    CCPoint pos = pTouch->getLocation();
    pos =scrollView->convertToNodeSpace(pos);
    
    //! skip points that are too close
    GLfloat eps = 1.5f;
    if (points->size() > 0) {
        LinePoint *lastPoint = (LinePoint*)points->at(points->size()-1);
        ccVertex2F lastPoint1 = {lastPoint->getPos().x, lastPoint->getPos().y};
        ccVertex2F pos1 = {pos.x, pos.y};
        GLfloat length = ccVertex2FLength(ccVertex2FSub(lastPoint1, pos1));
        
        if (length < eps) {
//            CCLOG("return");
            return;
        }
    }
    
    this->addPoint(pos, currentRadius);
}

void LineDrawer::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
    CCPoint pos = pTouch->getLocation();
    pos =scrollView->convertToNodeSpace(pos);
    
    this->endLineAt(pos, currentRadius);
}

#pragma mark - Handling points

void LineDrawer::addPoint(cocos2d::CCPoint newPoint, GLfloat size){
    LinePoint *point = new LinePoint();
    ccVertex2F newPoint1 = {newPoint.x, newPoint.y};
    point->setPos(newPoint1);
    point->setWidth(size);
    points->push_back(point);
//    point->release();
}

void LineDrawer::startNewLineFrom(cocos2d::CCPoint newPoint, GLfloat size){
    connectingLine = false;
    this->addPoint(newPoint, size);
}

void LineDrawer::endLineAt(cocos2d::CCPoint aEndPoint, GLfloat size){
    this->addPoint(aEndPoint, size);
    finishingLine = true;
}

#pragma mark - set color bursh
void LineDrawer::menuCloseCallback(CCObject* pSender)
{
    
    CCMenuItemImage *item = (CCMenuItemImage*)pSender;
    
    switch (item->getTag()) {
        case GREEN:
        {
            this->setColor(ccGREEN4F);
            break;
        }
        case YELLOW:
        {
            this->setColor(ccYELLOW4F);
            break;
        }
        case BROWN:
        {
            this->setColor(ccBROWN4F);
            break;
        }
        case RED:
        {
            this->setColor(ccRED4F);
            break;
        }
        case PURPLE:
        {
            this->setColor(ccPURPLE4F);
            break;
        }
        case BLUEPURPLE:
        {
            this->setColor(ccBLUEPURPLE4F);
            break;
        }
        case BLUE:
        {
            this->setColor(ccBLUE4F);
            break;
        }
        case WHITE:
        {
            this->setColor(ccWHITE4F);
            break;
        }
        case CAYCO:
        {
            this->setColor(ccTRANSPARENT4F);
            break;
        }
            
        default:
            break;
    }
}

void LineDrawer::setColor(ccColor4F _color){
    currentColor = ccc4f(_color.r/255.f,_color.g/255.f,_color.b/255.f,_color.a/255.f);
}

void LineDrawer::createMenuZoom(CCPoint _posMenu){
    CCSprite *sprite3 = CCSprite::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 0.png");
    sprite3->setPosition(_posMenu);
    addChild(sprite3,Z_INDEX_3);
    
    CCMenuItemToggle *btnBottomLeft = CCMenuItemToggle::createWithTarget(this,
                                                                         menu_selector(LineDrawer::menuZoom),
                                                                         CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 3 default.png",
                                                                                                 NULL),
                                                                         CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 3.png",
                                                                                                 NULL),NULL);
    btnBottomLeft->setTag(BOTTOM_LEFT);
    btnBottomLeft->setPosition(ccp(btnBottomLeft->getContentSize().width/2, btnBottomLeft->getContentSize().height/2));
    
    CCMenuItemToggle *btnBottomRight = CCMenuItemToggle::createWithTarget(this,
                                                                          menu_selector(LineDrawer::menuZoom),
                                                                          CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 4 default.png",
                                                                                                  NULL),
                                                                          CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 4.png",
                                                                                                  NULL),NULL);
    btnBottomRight->setTag(BOTTOM_RIGHT);
    btnBottomRight->setPosition(ccp(btnBottomRight->getContentSize().width*3/2, btnBottomRight->getContentSize().height/2));
    
    CCMenuItemToggle *btnTopLeft = CCMenuItemToggle::createWithTarget(this,
                                                                      menu_selector(LineDrawer::menuZoom),
                                                                      CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 1 default.png",
                                                                                              NULL),
                                                                      CCMenuItemImage::create("Images/btnDrawing/Drawing Buttons 50%/button zoom 1.png",
                                                                                              NULL),NULL);
    btnTopLeft->setTag(TOP_LEFT);
    btnTopLeft->setPosition(ccp(btnTopLeft->getContentSize().width/2, btnTopLeft->getContentSize().height*3/2));
    
    CCMenuItemToggle *btnTopRight = CCMenuItemToggle::createWithTarget(this,
                                                                       menu_selector(LineDrawer::menuZoom),
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

void LineDrawer::menuZoom(CCMenuItemToggle *_item){

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

void LineDrawer::zoomInWithTag(int _tag){
    
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
    
    scrollView->stopAllActions();
    scrollView->runAction(CCSpawn::create(CCScaleTo::create(0.5, 1.8),CCMoveTo::create(0.5, newPos),NULL));

    currentRadius = RADIUS_SMALL;
}

void LineDrawer::zoomOut(){
    scrollView->stopAllActions();
    scrollView->runAction(CCSpawn::create(CCScaleTo::create(0.5, 1),CCMoveTo::create(0.5, ccp(0, 0)),NULL));
    currentRadius = RADIUS_MEDIUM;
}

void LineDrawer::menuSave(cocos2d::CCMenuItemToggle *_item){
    if (!isShowMenu) {
        isShowMenu = true;
        PopupMenu *menu = PopupMenu::create();
        menu->setPosition(ccp(0, 0));
        this->addChild(menu, 100);
    }
}

void LineDrawer::menuSketch(cocos2d::CCMenuItemToggle *_item){
    if (_item->getSelectedIndex() == 1) {
        sprite2->stopAllActions();
        sprite2->runAction(CCFadeOut::create(0.5f));
    }else{
        sprite2->stopAllActions();
        sprite2->runAction(CCFadeIn::create(0.5f));
    }
}



#pragma mark ----------------- PopupMenu ---------------------

PopupMenu::PopupMenu(){
    
}

PopupMenu::~PopupMenu(){
    
}

CCScene* PopupMenu::scene()
{
    CCScene *scene = CCScene::create();
    PopupMenu *layer = PopupMenu::create();
    scene->addChild(layer);
    return scene;
}


bool PopupMenu::init(){
    if (!CCLayer::init()) {
        return false;
    }
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    addChild(CCLayerColor::create(ccc4(122, 144, 0, 255), visibleSize.width, visibleSize.height));
    
    //this is the layer that we want to "cut"
    CCLayer *layer = CCLayer::create();
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    layer->addChild(pSprite, 0);
    
    //we need to create a ccnode, which will be a stencil for ccclipingnode, draw node is a good choice for that
    CCDrawNode * stecil = CCDrawNode::create();
    stecil->drawDot(ccp(visibleSize.width/2 + origin.x - 100, visibleSize.height/2 + origin.y), 30, ccc4f(0, 0, 0, 255));
    stecil->drawSegment(ccp(0, 0), ccp(visibleSize.width, visibleSize.height), 20, ccc4f(0, 0, 0, 255));
    
    //CCClipingNode show the intersection of stencil and theirs children
    CCClippingNode *cliper = CCClippingNode::create(stecil);
    //you want to hide intersection so we setInverted to true
    cliper->setInverted(true);
    cliper->addChild(layer);
    addChild(cliper);
    
    return true;
}




















