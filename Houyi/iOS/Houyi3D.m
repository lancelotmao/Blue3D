//
//  Houyi3D.m
//  Houyi
//
//  Created by lancemao on 9/15/15.
//  Copyright (c) 2015 Lance Mao. All rights reserved.
//

#import "Houyi3D.h"

@interface HouyiRoot ()
{
    Root* _root;
}

@end


@implementation HouyiRoot
- (void)sRoot:(Root*)root {
    _root = root;
}
- (Root*)gRoot {
    return _root;
}
@end

@interface Houyi3D ()
{
}
@end

@implementation Houyi3D

- (HouyiRoot*)startEngine {
    HouyiRoot* r = [HouyiRoot new];
    Root* root = HouyiNew Root();
    root->init();
    World* world = HouyiNew World();
    world->create(root);
    root->setWorld(world);
    [r sRoot:root];
    NSLog(@"Houyi Engine initialized");
    return r;
}

- (void)render:(HouyiRoot*)root {
    [root gRoot]->render();
}

- (BOOL)isRenderRequested :(HouyiRoot*)root {
    return [root gRoot]->isRenderRequested();
}

- (void)requestRender:(HouyiRoot*)root :(BOOL)requested {
    [root gRoot]->requestRender(requested);
}

- (void)onWindowChanged:(HouyiRoot*)root :(NSNumber*)width :(NSNumber*)height {
    [root gRoot]->onWindowChanged([width intValue], [height intValue]);
}

- (void)loadDataSync:(HouyiRoot*)root :(NSString*)filePath {
    NSData* data = [HouyiUtils getDataFromFile:filePath];
    string strFilePath = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
    Loader* loader = Loader::getLoader(strFilePath);
    Scene* scene = loader->load((const char*)[data bytes], [data length]);
    [root gRoot]->getWorld()->addScene(scene);
    [root gRoot]->getWorld()->setFocusScene(scene);
}

- (void)touchesBegan:(HouyiRoot*)root :(NSSet *)touches withEvent:(UIEvent *)event :(UIView*)view
{
    Root* r = [root gRoot];
    if (r)
    {
        MotionEvent e(0, 0, MotionEvent::TYPE_DOWN);
        int allCount = [[event allTouches] count];
        if (allCount == 1)
        {
            CGPoint pt = [[touches anyObject] locationInView:view];
            e.setX(pt.x);
            e.setY(pt.y);
        }
        else
        {
            NSEnumerator *enumerator = [[event allTouches] objectEnumerator];
            id object;
            int i = 0;
            
            while ((object = [enumerator nextObject]))
            {
                UITouch* touch = (UITouch*)object;
                float x = [touch locationInView:view].x;
                float y = [touch locationInView:view].y;
                e.setX(x, i);
                e.setY(y, i);
                i++;
            }
            e.mTouchCount = allCount;
        }
        r->onTouch(e);
        [view setNeedsDisplay];
    }
}

- (void)touchesMoved:(HouyiRoot*)root :(NSSet *)touches withEvent:(UIEvent *)event :(UIView*)view
{
    Root* r = [root gRoot];
    if (r)
    {
        CGPoint pt = [[touches anyObject] locationInView:view];
        MotionEvent e(pt.x, pt.y, MotionEvent::TYPE_MOVE);
        if (!Settings::getInstance()->usePlatformGesture() && [touches count] == 2)
        {
            UITouch* touch = [[touches allObjects] objectAtIndex:1];
            float x = [touch locationInView:view].x;
            float y = [touch locationInView:view].y;
            e.setX(x, 1);
            e.setY(y, 1);
            e.mTouchCount = 2;
        }
        r->onTouch(e);
        [view setNeedsDisplay];
    }
}

- (void)touchesEnded:(HouyiRoot*)root :(NSSet *)touches withEvent:(UIEvent *)event :(UIView*)view
{
    Root* r = [root gRoot];
    if (r)
    {
        CGPoint pt = [[touches anyObject] locationInView:view];
        MotionEvent e(pt.x, pt.y, MotionEvent::TYPE_UP);
        e.mTouchCount = [[event touchesForView:view] count];
        r->onTouch(e);
        [view setNeedsDisplay];
    }
}

@end
