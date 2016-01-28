//
//  HouyiUtils.m
//  Houyi
//
//  Created by Lance Mao on 23/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiUtils.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"

@implementation HouyiUtils

+ (NSData*)getDataFromFile:(NSString*) fileName
{
    // first try directly load from file
    NSData *texData = [[NSData alloc] initWithContentsOfFile:fileName];
    
    // if failed, try under main bundle
    if (texData == nil)
    {
        NSArray* divided = [fileName componentsSeparatedByString:@"."];
        if ([divided count] > 1)
        {
            NSString *path = [[NSBundle mainBundle] pathForResource:[divided objectAtIndex:0] ofType:[divided objectAtIndex:1]];
            if (path)
            {
                texData = [[NSData alloc] initWithContentsOfFile:path];
            }
            else
            {
                NSLog(@"Could not find resource from main bundle: %@\n", fileName);
                return nil;
            }
        }
        else
        {
            NSLog(@"Unsupported URL. = %@", fileName);
        }
    }
    
    return texData;
}

+ (Pass*)loadShader:(Renderer*)renderer :(NSString*) vertexShader : (NSString*) fragmentShader
{
//    Root* root = Root::getInstance();
//    if (!root) {
//        NSLog(@"Engine not ignited");
//        return 0;
//    }
    
    NSString *vertShaderPathname, *fragShaderPathname;
    
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:vertexShader ofType:@"sdr"];
    GLchar* vSource = (GLchar *)[[NSString stringWithContentsOfFile:vertShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!vSource) {
        NSLog(@"Failed to load vertex shader");
        return 0;
    }
    
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:fragmentShader ofType:@"sdr"];
    GLchar* fSource = (GLchar *)[[NSString stringWithContentsOfFile:fragShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!fSource) {
        NSLog(@"Failed to load fragment shader");
        return 0;
    }
    
    return renderer->loadShader(vSource, fSource);
}

@end
