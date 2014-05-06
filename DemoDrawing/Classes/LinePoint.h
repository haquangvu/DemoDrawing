//
//  LinePoint.h
//  DemoDrawing
//
//  Created by vuha on 4/24/14.
//
//

#ifndef __DemoDrawing__LinePoint__
#define __DemoDrawing__LinePoint__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

class LinePoint:public CCObject {
    CC_SYNTHESIZE(GLfloat, width, Width);
    CC_SYNTHESIZE(ccVertex2F, pos, Pos);
public:

};


#endif /* defined(__DemoDrawing__LinePoint__) */
