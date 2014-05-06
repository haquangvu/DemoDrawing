//
//  AppMacros.h
//  DemoDrawing
//
//  Created by vuha on 4/17/14.
//
//

#ifndef DemoDrawing_AppMacros_h
#define DemoDrawing_AppMacros_h

#include "cocos2d.h"

using namespace cocos2d;

static const ccColor4F ccWHITE4F ={255, 255, 255, 255};
static const ccColor4F ccGREEN4F ={40,222,61, 255};
static const ccColor4F ccYELLOW4F ={255,214,51, 255};
static const ccColor4F ccBROWN4F ={154, 82, 63, 255};
static const ccColor4F ccPURPLE4F ={206,60,243, 255};
static const ccColor4F ccBLUE4F ={60,189,243, 255};
static const ccColor4F ccRED4F ={255,71,52, 255};

static const ccColor4F ccBLUEPURPLE4F ={80,62,243, 255};
static const ccColor4F ccTRANSPARENT4F ={255,255,255, 0};

#define RADIUS_SMALL    13.0f
#define RADIUS_MEDIUM   25.0f
#define RADIUS_LARGE    10.0f
//
//typedef struct LineVertex {
//    CCPoint pos;
//    float z;
//    ccColor4F color;
//} LineVertex;

typedef struct _LineVertex
{
    ccVertex2F    pos;        // 8 bytes
    ccColor4F    color;        // 4 bytes
    GLfloat        z;        // 4 bytes
} LineVertex;

#endif
