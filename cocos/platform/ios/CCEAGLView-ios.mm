/*

===== IMPORTANT =====

This is sample code demonstrating API, technology or techniques in development.
Although this sample code has been reviewed for technical accuracy, it is not
final. Apple is supplying this information to help you plan for the adoption of
the technologies and programming interfaces described herein. This information
is subject to change, and software implemented based on this sample code should
be tested with final operating system software and final documentation. Newer
versions of this sample code may be provided with future seeds of the API or
technology. For information about updates to this and other developer
documentation, view the New & Updated sidebars in subsequent documentation
seeds.

=====================

File: EAGLView.m
Abstract: Convenience class that wraps the CAEAGLLayer from CoreAnimation into a
UIView subclass.

Version: 1.3

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
("Apple") in consideration of your agreement to the following terms, and your
use, installation, modification or redistribution of this Apple software
constitutes acceptance of these terms.  If you do not agree with these terms,
please do not use, install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and subject
to these terms, Apple grants you a personal, non-exclusive license, under
Apple's copyrights in this original Apple software (the "Apple Software"), to
use, reproduce, modify and redistribute the Apple Software, with or without
modifications, in source and/or binary forms; provided that if you redistribute
the Apple Software in its entirety and without modifications, you must retain
this notice and the following text and disclaimers in all such redistributions
of the Apple Software.
Neither the name, trademarks, service marks or logos of Apple Inc. may be used
to endorse or promote products derived from the Apple Software without specific
prior written permission from Apple.  Except as expressly stated in this notice,
no other rights or licenses, express or implied, are granted by Apple herein,
including but not limited to any patent rights that may be infringed by your
derivative works or by other works in which the Apple Software may be
incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2008 Apple Inc. All Rights Reserved.

*/

#include <cocos/platform/CCPlatformConfig.h>
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#    include <cocos/platform/ios/CCEAGLView-ios.h>

#    include <cocos/base/CCDirector.h>
#    include <cocos/base/CCIMEDelegate.h>
#    include <cocos/base/CCIMEDispatcher.h>
#    include <cocos/math/CCGeometry.h>
#    include <cocos/math/Vec2.h>
#    include <cocos/platform/CCGL.h>
#    include <cocos/platform/CCGLView.h>
#    include <cocos/platform/CCPlatformMacros.h>
#    include <cocos/platform/ios/CCES2Renderer-ios.h>
#    include <cocos/platform/ios/OpenGL_Internal-ios.h>

#    import <CoreGraphics/CGAffineTransform.h>
#    import <CoreGraphics/CGBase.h>
#    import <CoreGraphics/CGGeometry.h>
#    import <Foundation/NSException.h>
#    import <Foundation/NSObjCRuntime.h>
#    import <Foundation/NSRange.h>
#    import <Foundation/NSString.h>
#    import <UIKit/UIApplication.h>
#    import <UIKit/UIScreen.h>
#    import <UIKit/UITextInput.h>
#    import <UIKit/UITextInputTraits.h>
#    import <UIKit/UIWindow.h>

#    include <cstdint>
#    include <cstring>
#    include <type_traits>

// CLASS IMPLEMENTATIONS:

#    define IOS_MAX_TOUCHES_COUNT 10

@interface CCEAGLView ()

@property (nonatomic, strong) NSString* markedText;
@property (nonatomic, assign) CGRect caretRect;
@property (nonatomic, readonly, assign) CGRect originalRect;

- (BOOL)setupSurfaceWithSharegroup:(EAGLSharegroup*)sharegroup;
- (unsigned int)convertPixelFormat:(NSString*)pixelFormat;
@end

@implementation CCEAGLView

@synthesize renderer = _renderer;
@synthesize context = _context;
@synthesize pixelformat = _pixelformat;
@synthesize depthFormat = _depthFormat;
@synthesize preserveBackbuffer = _preserveBackbuffer;
@synthesize size = _size;
@synthesize surfaceSize = _surfaceSize;
@synthesize discardFramebufferSupported = _discardFramebufferSupported;

@synthesize multisampling = _multisampling;
@synthesize requestedSamples = _requestedSamples;
@synthesize isUseUITextField = _isUseUITextField;

@synthesize isKeyboardShown = _isKeyboardShown;
@synthesize keyboardShowNotification = _keyboardShowNotification;

@synthesize beginningOfDocument = _beginningOfDocument;
@synthesize endOfDocument = _endOfDocument;
@synthesize inputDelegate = _inputDelegate;
@synthesize tokenizer = _tokenizer;

@synthesize markedText = _markedText;
@synthesize caretRect = _caretRect;
@synthesize originalRect = _originalRect;

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

+ (id)viewWithFrame:(CGRect)frame {
    return [[[self alloc] initWithFrame:frame] autorelease];
}

+ (id)viewWithFrame:(CGRect)frame pixelFormat:(NSString*)format {
    return [[[self alloc] initWithFrame:frame pixelFormat:format] autorelease];
}

+ (id)viewWithFrame:(CGRect)frame pixelFormat:(NSString*)format depthFormat:(GLuint)depth {
    return [[[self alloc] initWithFrame:frame pixelFormat:format depthFormat:depth preserveBackbuffer:NO sharegroup:nil multiSampling:NO
                        numberOfSamples:0] autorelease];
}

+ (id)viewWithFrame:(CGRect)frame
           pixelFormat:(NSString*)format
           depthFormat:(GLuint)depth
    preserveBackbuffer:(BOOL)retained
            sharegroup:(EAGLSharegroup*)sharegroup
         multiSampling:(BOOL)multisampling
       numberOfSamples:(unsigned int)samples {
    return [[[self alloc] initWithFrame:frame
                            pixelFormat:format
                            depthFormat:depth
                     preserveBackbuffer:retained
                             sharegroup:sharegroup
                          multiSampling:multisampling
                        numberOfSamples:samples] autorelease];
}

- (id)initWithFrame:(CGRect)frame {
    return [self initWithFrame:frame pixelFormat:kEAGLColorFormatRGB565 depthFormat:0 preserveBackbuffer:NO sharegroup:nil multiSampling:NO numberOfSamples:0];
}

- (id)initWithFrame:(CGRect)frame pixelFormat:(NSString*)format {
    return [self initWithFrame:frame pixelFormat:format depthFormat:0 preserveBackbuffer:NO sharegroup:nil multiSampling:NO numberOfSamples:0];
}

- (id)initWithFrame:(CGRect)frame
           pixelFormat:(NSString*)format
           depthFormat:(GLuint)depth
    preserveBackbuffer:(BOOL)retained
            sharegroup:(EAGLSharegroup*)sharegroup
         multiSampling:(BOOL)sampling
       numberOfSamples:(unsigned int)nSamples {
    if ((self = [super initWithFrame:frame]))
    {
        _isUseUITextField = YES;
        _pixelformat = format;
        _depthFormat = depth;
        _multisampling = sampling;
        _requestedSamples = nSamples;
        _preserveBackbuffer = retained;
        _markedText = nil;
        if (![self setupSurfaceWithSharegroup:sharegroup])
        {
            [self release];
            return nil;
        }

        _originalRect = self.frame;
        _keyboardShowNotification = nil;

        if ([self respondsToSelector:@selector(setContentScaleFactor:)])
        {
            self.contentScaleFactor = [[UIScreen mainScreen] scale];
        }
    }

    return self;
}

- (id)initWithCoder:(NSCoder*)aDecoder {
    if ((self = [super initWithCoder:aDecoder]))
    {
        CAEAGLLayer* eaglLayer = static_cast<CAEAGLLayer*>([self layer]);

        _pixelformat = kEAGLColorFormatRGB565;
        _depthFormat = 0; // GL_DEPTH_COMPONENT24_OES;
        _multisampling = NO;
        _requestedSamples = 0;
        _size = [eaglLayer bounds].size;
        _markedText = nil;

        if (![self setupSurfaceWithSharegroup:nil])
        {
            [self release];
            return nil;
        }
    }

    return self;
}

- (void)didMoveToWindow {
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onUIKeyboardNotification:) name:UIKeyboardWillShowNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onUIKeyboardNotification:) name:UIKeyboardDidShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onUIKeyboardNotification:) name:UIKeyboardWillHideNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onUIKeyboardNotification:) name:UIKeyboardDidHideNotification object:nil];
}

- (int)getWidth {
    CGSize bound = [self bounds].size;
    return static_cast<int>(bound.width * self.contentScaleFactor);
}

- (int)getHeight {
    CGSize bound = [self bounds].size;
    return static_cast<int>(bound.height * self.contentScaleFactor);
}

- (BOOL)setupSurfaceWithSharegroup:(EAGLSharegroup*)sharegroup {
    CAEAGLLayer* eaglLayer = static_cast<CAEAGLLayer*>(self.layer);

    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:self.preserveBackbuffer],
                                                                              kEAGLDrawablePropertyRetainedBacking, self.pixelformat,
                                                                              kEAGLDrawablePropertyColorFormat, nil];

    self.renderer = [[CCES2Renderer alloc] initWithDepthFormat:self.depthFormat
                                               withPixelFormat:[self convertPixelFormat:self.pixelformat]
                                                withSharegroup:sharegroup
                                             withMultiSampling:self.multisampling
                                           withNumberOfSamples:self.requestedSamples];

    NSAssert(self.renderer, @"OpenGL ES 2.O is required.");
    if (!self.renderer)
        return NO;

    self.context = [self.renderer context];

#    if GL_EXT_discard_framebuffer == 1
    self.discardFramebufferSupported = YES;
#    else
    self.discardFramebufferSupported = NO;
#    endif

    CHECK_GL_ERROR();

    return YES;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self]; // remove keyboard notification
    [self.renderer release];
    self.keyboardShowNotification = nullptr; // implicit release
    [super dealloc];
}

- (void)layoutSubviews {
    if (!cocos2d::Director::getInstance()->isValid())
    {
        return;
    }

    [self.renderer resizeFromLayer:static_cast<CAEAGLLayer*>(self.layer)];
    self.size = [self.renderer backingSize];

    // Issue #914 #924
    //     Director *director = [Director sharedDirector];
    //     [director reshapeProjection:size_];
    cocos2d::Size const size = {self.size.width, self.size.height};
    // cocos2d::Director::getInstance()->reshapeProjection(size);

    // Avoid flicker. Issue #350
    //[director performSelectorOnMainThread:@selector(drawScene) withObject:nil waitUntilDone:YES];
    if ([NSThread isMainThread])
    {
        cocos2d::Director::getInstance()->drawScene();
    }
}

- (void)swapBuffers {
    // IMPORTANT:
    // - preconditions
    //    -> context_ MUST be the OpenGL context
    //    -> renderbuffer_ must be the RENDER BUFFER

#    ifdef __IPHONE_4_0

    if (self.multisampling)
    {
        /* Resolve from msaaFramebuffer to resolveFramebuffer */
        // glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, [self.renderer msaaFrameBuffer]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, [self.renderer defaultFrameBuffer]);
        glResolveMultisampleFramebufferAPPLE();
    }

    if (self.discardFramebufferSupported)
    {
        if (self.multisampling)
        {
            if (self.depthFormat)
            {
                GLenum attachments[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
                glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 2, attachments);
            }
            else
            {
                GLenum attachments[] = {GL_COLOR_ATTACHMENT0};
                glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, attachments);
            }

            glBindRenderbuffer(GL_RENDERBUFFER, [self.renderer colorRenderBuffer]);
        }

        // not MSAA
        else if (self.depthFormat)
        {
            GLenum attachments[] = {GL_DEPTH_ATTACHMENT};
            glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, attachments);
        }
    }

#    endif // __IPHONE_4_0

    if (![self.context presentRenderbuffer:GL_RENDERBUFFER])
    {
        //         CCLOG(@"cocos2d: Failed to swap renderbuffer in %s\n", __FUNCTION__);
    }

#    if COCOS2D_DEBUG
    CHECK_GL_ERROR();
#    endif

    // We can safely re-bind the framebuffer here, since this will be the
    // 1st instruction of the new main loop
    if (self.multisampling)
        glBindFramebuffer(GL_FRAMEBUFFER, [self.renderer msaaFrameBuffer]);
}

- (unsigned int)convertPixelFormat:(NSString*)pixelFormat {
    // define the pixel format
    GLenum pFormat;

    if ([pixelFormat isEqualToString:@"EAGLColorFormat565"])
        pFormat = GL_RGB565;
    else
        pFormat = GL_RGBA8_OES;

    return pFormat;
}

#    pragma mark CCEAGLView - Point conversion

- (CGPoint)convertPointFromViewToSurface:(CGPoint)point {
    CGRect bounds = [self bounds];

    CGPoint ret;
    ret.x = (point.x - bounds.origin.x) / bounds.size.width * self.size.width;
    ret.y = (point.y - bounds.origin.y) / bounds.size.height * self.size.height;

    return ret;
}

- (CGRect)convertRectFromViewToSurface:(CGRect)rect {
    CGRect bounds = [self bounds];

    CGRect ret;
    ret.origin.x = (rect.origin.x - bounds.origin.x) / bounds.size.width * self.size.width;
    ret.origin.y = (rect.origin.y - bounds.origin.y) / bounds.size.height * self.size.height;
    ret.size.width = rect.size.width / bounds.size.width * self.size.width;
    ret.size.height = rect.size.height / bounds.size.height * self.size.height;

    return ret;
}

- (void)handleTouchesAfterKeyboardShow {
    NSArray* subviews = self.subviews;

    for (UIView* view in subviews)
    {
        if ([view isKindOfClass:NSClassFromString(@"UITextView")] || [view isKindOfClass:NSClassFromString(@"UITextField")])
        {
            if ([view isFirstResponder])
            {
                [view resignFirstResponder];
                return;
            }
        }
    }
}

// Pass the touches to the superview
#    pragma mark CCEAGLView - Touch Delegate
- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    if (self.isKeyboardShown)
    {
        [self handleTouchesAfterKeyboardShow];
    }

    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {nullptr};
    float xs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT] = {0.0f};

    int i = 0;
    for (UITouch* touch in touches)
    {
        ids[i] = touch;
        xs[i] = [touch locationInView:[touch view]].x * self.contentScaleFactor;
        ys[i] = [touch locationInView:[touch view]].y * self.contentScaleFactor;
        ++i;
    }

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->handleTouchesBegin(i, reinterpret_cast<intptr_t*>(ids), xs, ys);
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {nullptr};
    float xs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float fs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float ms[IOS_MAX_TOUCHES_COUNT] = {0.0f};

    int i = 0;
    for (UITouch* touch in touches)
    {
        ids[i] = touch;
        xs[i] = [touch locationInView:[touch view]].x * self.contentScaleFactor;
        ys[i] = [touch locationInView:[touch view]].y * self.contentScaleFactor;
#    if defined(__IPHONE_9_0) && (__IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_9_0)
        // running on iOS 9.0 or higher version
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 9.0f)
        {
            fs[i] = touch.force;
            ms[i] = touch.maximumPossibleForce;
        }
#    endif
        ++i;
    }

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->handleTouchesMove(i, reinterpret_cast<intptr_t*>(ids), xs, ys, fs, ms);
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {nullptr};
    float xs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT] = {0.0f};

    int i = 0;
    for (UITouch* touch in touches)
    {
        ids[i] = touch;
        xs[i] = [touch locationInView:[touch view]].x * self.contentScaleFactor;
        ys[i] = [touch locationInView:[touch view]].y * self.contentScaleFactor;
        ++i;
    }

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->handleTouchesEnd(i, reinterpret_cast<intptr_t*>(ids), xs, ys);
}

- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {nullptr};
    float xs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT] = {0.0f};

    int i = 0;
    for (UITouch* touch in touches)
    {
        ids[i] = touch;
        xs[i] = [touch locationInView:[touch view]].x * self.contentScaleFactor;
        ys[i] = [touch locationInView:[touch view]].y * self.contentScaleFactor;
        ++i;
    }

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->handleTouchesCancel(i, reinterpret_cast<intptr_t*>(ids), xs, ys);
}

#    pragma mark - UIView - Responder

- (BOOL)canBecomeFirstResponder {
    self.markedText = nil;
    if (self.isUseUITextField)
    {
        return NO;
    }
    return YES;
}

- (BOOL)becomeFirstResponder {
    self.isUseUITextField = NO;
    return [super becomeFirstResponder];
}

- (BOOL)resignFirstResponder {
    self.isUseUITextField = YES;
    return [super resignFirstResponder];
}

#    pragma mark - UIKeyInput protocol

- (BOOL)hasText {
    return NO;
}

- (void)insertText:(NSString*)text {
    self.markedText = nil;
    const char* pszText = [text cStringUsingEncoding:NSUTF8StringEncoding];
    cocos2d::IMEDispatcher::sharedDispatcher()->dispatchInsertText(pszText, strlen(pszText));
}

- (void)deleteBackward {
    self.markedText = nil;
    cocos2d::IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
}

#    pragma mark - UITextInputTrait protocol

- (UITextAutocapitalizationType)autocapitalizationType {
    return UITextAutocapitalizationTypeNone;
}

#    pragma mark - UITextInput protocol

#    pragma mark UITextInput - properties

/* Text may have a selection, either zero-length (a caret) or ranged.  Editing operations are
 * always performed on the text from this selection.  nil corresponds to no selection. */
- (void)setSelectedTextRange:(UITextRange*)aSelectedTextRange {
    CCLOG("UITextRange:setSelectedTextRange");
}
- (UITextRange*)selectedTextRange {
    return [[[UITextRange alloc] init] autorelease];
}

#    pragma mark UITextInput - Replacing and Returning Text

- (NSString*)textInRange:(UITextRange*)range {
    CCLOG("textInRange");
    return @"";
}
- (void)replaceRange:(UITextRange*)range withText:(NSString*)theText {
    CCLOG("replaceRange");
}

#    pragma mark UITextInput - Working with Marked and Selected Text

/* If text can be selected, it can be marked. Marked text represents provisionally
 * inserted text that has yet to be confirmed by the user.  It requires unique visual
 * treatment in its display.  If there is any marked text, the selection, whether a
 * caret or an extended range, always resides within.
 *
 * Setting marked text either replaces the existing marked text or, if none is present,
 * inserts it from the current selection. */

- (void)setMarkedTextRange:(UITextRange*)markedTextRange {
    CCLOG("setMarkedTextRange");
}

- (UITextRange*)markedTextRange {
    CCLOG("markedTextRange");
    return nil; // Nil if no marked text.
}
- (void)setMarkedTextStyle:(NSDictionary*)markedTextStyle {
    CCLOG("setMarkedTextStyle");
}
- (NSDictionary*)markedTextStyle {
    CCLOG("markedTextStyle");
    return nil;
}
- (void)setMarkedText:(NSString*)markedText selectedRange:(NSRange)selectedRange {
    CCLOG("setMarkedText");
    if (markedText == self.markedText)
    {
        return;
    }
    self.markedText = markedText;
}
- (void)unmarkText {
    CCLOG("unmarkText");
    if (nil == self.markedText)
    {
        return;
    }
    const char* pszText = [self.markedText cStringUsingEncoding:NSUTF8StringEncoding];
    cocos2d::IMEDispatcher::sharedDispatcher()->dispatchInsertText(pszText, strlen(pszText));
    self.markedText = nil;
}

#    pragma mark Methods for creating ranges and positions.

- (UITextRange*)textRangeFromPosition:(UITextPosition*)fromPosition toPosition:(UITextPosition*)toPosition {
    CCLOG("textRangeFromPosition");
    return nil;
}
- (UITextPosition*)positionFromPosition:(UITextPosition*)position offset:(NSInteger)offset {
    CCLOG("positionFromPosition");
    return nil;
}
- (UITextPosition*)positionFromPosition:(UITextPosition*)position inDirection:(UITextLayoutDirection)direction offset:(NSInteger)offset {
    CCLOG("positionFromPosition");
    return nil;
}

/* Simple evaluation of positions */
- (NSComparisonResult)comparePosition:(UITextPosition*)position toPosition:(UITextPosition*)other {
    CCLOG("comparePosition");
    return static_cast<NSComparisonResult>(0);
}
- (NSInteger)offsetFromPosition:(UITextPosition*)from toPosition:(UITextPosition*)toPosition {
    CCLOG("offsetFromPosition");
    return 0;
}

- (UITextPosition*)positionWithinRange:(UITextRange*)range farthestInDirection:(UITextLayoutDirection)direction {
    CCLOG("positionWithinRange");
    return nil;
}
- (UITextRange*)characterRangeByExtendingPosition:(UITextPosition*)position inDirection:(UITextLayoutDirection)direction {
    CCLOG("characterRangeByExtendingPosition");
    return nil;
}

#    pragma mark Writing direction

- (UITextWritingDirection)baseWritingDirectionForPosition:(UITextPosition*)position inDirection:(UITextStorageDirection)direction {
    CCLOG("baseWritingDirectionForPosition");
    return UITextWritingDirectionNatural;
}
- (void)setBaseWritingDirection:(UITextWritingDirection)writingDirection forRange:(UITextRange*)range {
    CCLOG("setBaseWritingDirection");
}

#    pragma mark Geometry

/* Geometry used to provide, for example, a correction rect. */
- (CGRect)firstRectForRange:(UITextRange*)range {
    CCLOG("firstRectForRange");
    return CGRectNull;
}
- (CGRect)caretRectForPosition:(UITextPosition*)position {
    CCLOG("caretRectForPosition");
    return self.caretRect;
}

#    pragma mark Hit testing

/* JS - Find the closest position to a given point */
- (UITextPosition*)closestPositionToPoint:(CGPoint)point {
    CCLOG("closestPositionToPoint");
    return nil;
}
- (UITextPosition*)closestPositionToPoint:(CGPoint)point withinRange:(UITextRange*)range {
    CCLOG("closestPositionToPoint");
    return nil;
}
- (UITextRange*)characterRangeAtPoint:(CGPoint)point {
    CCLOG("characterRangeAtPoint");
    return nil;
}

- (NSArray*)selectionRectsForRange:(UITextRange*)range {
    CCLOG("selectionRectsForRange");
    return nil;
}

#    pragma mark - UIKeyboard notification

- (void)onUIKeyboardNotification:(NSNotification*)notif {
    NSString* type = notif.name;

    NSDictionary* info = [notif userInfo];
    CGRect begin = [self convertRect:[[info objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue] fromView:self];
    CGRect end = [self convertRect:[[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue] fromView:self];
    double aniDuration = [[info objectForKey:UIKeyboardAnimationDurationUserInfoKey] doubleValue];

    CGSize viewSize = self.frame.size;

#    if defined(CC_TARGET_OS_TVOS)
    // statusBarOrientation not defined on tvOS, and also, orientation makes
    // no sense on tvOS
    begin.origin.y = viewSize.height - begin.origin.y - begin.size.height;
    end.origin.y = viewSize.height - end.origin.y - end.size.height;
#    else
    CGFloat tmp;
    switch (getFixedOrientation([[UIApplication sharedApplication] statusBarOrientation]))
    {
        case UIInterfaceOrientationUnknown:
            break;
        case UIInterfaceOrientationPortrait:
            begin.origin.y = viewSize.height - begin.origin.y - begin.size.height;
            end.origin.y = viewSize.height - end.origin.y - end.size.height;
            break;

        case UIInterfaceOrientationPortraitUpsideDown:
            begin.origin.x = viewSize.width - (begin.origin.x + begin.size.width);
            end.origin.x = viewSize.width - (end.origin.x + end.size.width);
            break;

        case UIInterfaceOrientationLandscapeLeft:
            std::swap(begin.size.width, begin.size.height);
            std::swap(end.size.width, end.size.height);
            std::swap(viewSize.width, viewSize.height);

            tmp = begin.origin.x;
            begin.origin.x = begin.origin.y;
            begin.origin.y = viewSize.height - tmp - begin.size.height;
            tmp = end.origin.x;
            end.origin.x = end.origin.y;
            end.origin.y = viewSize.height - tmp - end.size.height;
            break;

        case UIInterfaceOrientationLandscapeRight:
            std::swap(begin.size.width, begin.size.height);
            std::swap(end.size.width, end.size.height);
            std::swap(viewSize.width, viewSize.height);

            tmp = begin.origin.x;
            begin.origin.x = begin.origin.y;
            begin.origin.y = tmp;
            tmp = end.origin.x;
            end.origin.x = end.origin.y;
            end.origin.y = tmp;
            break;
    }
#    endif

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    float scaleX = glview->getScaleX();
    float scaleY = glview->getScaleY();

    // Convert to pixel coordinate
    begin = CGRectApplyAffineTransform(begin, CGAffineTransformScale(CGAffineTransformIdentity, self.contentScaleFactor, self.contentScaleFactor));
    end = CGRectApplyAffineTransform(end, CGAffineTransformScale(CGAffineTransformIdentity, self.contentScaleFactor, self.contentScaleFactor));

    float offestY = glview->getViewPortRect().origin.y;
    CCLOG("offestY = %f", offestY);
    if (offestY < 0.0f)
    {
        begin.origin.y += static_cast<CGFloat>(offestY);
        begin.size.height -= static_cast<CGFloat>(offestY);
        end.size.height -= static_cast<CGFloat>(offestY);
    }

    // Convert to design coordinate
    begin = CGRectApplyAffineTransform(begin,
                                       CGAffineTransformScale(CGAffineTransformIdentity, 1.0 / static_cast<CGFloat>(scaleX), 1.0 / static_cast<CGFloat>(scaleY)));
    end = CGRectApplyAffineTransform(end,
                                     CGAffineTransformScale(CGAffineTransformIdentity, 1.0 / static_cast<CGFloat>(scaleX), 1.0 / static_cast<CGFloat>(scaleY)));

    cocos2d::IMEKeyboardNotificationInfo notiInfo;
    notiInfo.begin = cocos2d::Rect(begin.origin.x, begin.origin.y, begin.size.width, begin.size.height);
    notiInfo.end = cocos2d::Rect(end.origin.x, end.origin.y, end.size.width, end.size.height);
    notiInfo.duration = static_cast<float>(aniDuration);

    cocos2d::IMEDispatcher* dispatcher = cocos2d::IMEDispatcher::sharedDispatcher();
    if (UIKeyboardWillShowNotification == type)
    {
        self.keyboardShowNotification = notif; // implicit copy
        dispatcher->dispatchKeyboardWillShow(notiInfo);
    }
    else if (UIKeyboardDidShowNotification == type)
    {
        // CGSize screenSize = self.window.screen.bounds.size;
        dispatcher->dispatchKeyboardDidShow(notiInfo);
        self.caretRect = end;

#    if defined(CC_TARGET_OS_TVOS)
        // smallSystemFontSize not available on TVOS
        int fontSize = 12;
#    else
        int fontSize = [UIFont smallSystemFontSize];
#    endif

        self.caretRect.origin = CGPointMake(self.caretRect.origin.x, viewSize.height - (self.caretRect.origin.y + self.caretRect.size.height + fontSize));
        self.caretRect.size = CGSizeMake(self.caretRect.size.height, 0.0);

        self.isKeyboardShown = YES;
    }
    else if (UIKeyboardWillHideNotification == type)
    {
        dispatcher->dispatchKeyboardWillHide(notiInfo);
    }
    else if (UIKeyboardDidHideNotification == type)
    {
        self.caretRect = CGRectZero;
        dispatcher->dispatchKeyboardDidHide(notiInfo);
        self.isKeyboardShown = NO;
    }
}

#    if !defined(CC_TARGET_OS_TVOS)
UIInterfaceOrientation getFixedOrientation(UIInterfaceOrientation statusBarOrientation)
{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.f)
    {
        statusBarOrientation = UIInterfaceOrientationPortrait;
    }
    return statusBarOrientation;
}
#    endif

- (void)doAnimationWhenKeyboardMoveWithDuration:(float)duration distance:(float)dis {
    [UIView beginAnimations:nil context:nullptr];
    [UIView setAnimationDelegate:self];
    [UIView setAnimationDuration:static_cast<NSTimeInterval>(duration)];
    [UIView setAnimationBeginsFromCurrentState:YES];

    // NSLog(@"[animation] dis = %f, scale = %f \n", dis, cocos2d::GLView::getInstance()->getScaleY());

    if (dis < 0.0f)
        dis = 0.0f;

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    dis *= glview->getScaleY();

    dis /= self.contentScaleFactor;

#    if defined(CC_TARGET_OS_TVOS)
    self.frame = CGRectMake(self.originalRect.origin.x, self.originalRect.origin.y - dis, self.originalRect.size.width, self.originalRect.size.height);
#    else
    switch (getFixedOrientation([[UIApplication sharedApplication] statusBarOrientation]))
    {
        case UIInterfaceOrientationUnknown:
            break;
        case UIInterfaceOrientationPortrait:
            self.frame = CGRectMake(self.originalRect.origin.x, self.originalRect.origin.y - static_cast<CGFloat>(dis), self.originalRect.size.width,
                                    self.originalRect.size.height);
            break;

        case UIInterfaceOrientationPortraitUpsideDown:
            self.frame = CGRectMake(self.originalRect.origin.x, self.originalRect.origin.y + static_cast<CGFloat>(dis), self.originalRect.size.width,
                                    self.originalRect.size.height);
            break;

        case UIInterfaceOrientationLandscapeLeft:
            self.frame = CGRectMake(self.originalRect.origin.x - static_cast<CGFloat>(dis), self.originalRect.origin.y, self.originalRect.size.width,
                                    self.originalRect.size.height);
            break;

        case UIInterfaceOrientationLandscapeRight:
            self.frame = CGRectMake(self.originalRect.origin.x + static_cast<CGFloat>(dis), self.originalRect.origin.y, self.originalRect.size.width,
                                    self.originalRect.size.height);
            break;
    }
#    endif

    [UIView commitAnimations];
}

- (void)doAnimationWhenAnotherEditBeClicked {
    if (self.keyboardShowNotification != nil)
    {
        [[NSNotificationCenter defaultCenter] postNotification:self.keyboardShowNotification];
    }
}

@end

#endif // CC_PLATFORM_IOS
