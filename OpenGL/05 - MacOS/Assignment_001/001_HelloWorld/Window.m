/////
//    All objects in Obj-C are pointers by default except Struct variables.
/////

// clang -o Window Window.m -framework cocoa
// 
// mkdir -p Window.app/Contents/MacOS
// clang -o Window.app/Contents/MacOS/Window Window.m -framework cocoa

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> // --> analogous to Windows.h and Xlib.h

//Forward Declaration - from C++
//Delegation Conformance
@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>
@end
//NS --> NeXTSTEP platform
//similar to 'extends' (:) and 'implements' (< >)
//Here AppDelegate extends NSObject and implements <NSApplicationDelegate, NSWindowDelegate>


//main
int main(int argc, char *argv[])
{
    //code
    //NSApp - la tyacha delegate set karna mhanje NSApp chi 'applicationDidFinishLaunching()' call karne just like WM_CREATE
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; 
    NSApp = [NSApplication sharedApplication]; //NSApp - in-built global object like 'document' in JavaScript
    [NSApp setDelegate:[[AppDelegate alloc] init]];

    //Message Loop or Run Loop or Event Loop
    [NSApp run];

    //program end
    //release the allocated pool
    [pool release];

    return 0;
}

//Forward declaration
@interface MyView:NSView
@end

//start AppDelegate
@implementation AppDelegate
{
    @private
    NSWindow *window;
    MyView *view; //because we want to configure it ourselves, hence we are not using NSView's object directly.
}

//instance method
-(void) applicationDidFinishLaunching:(NSNotification *)aNotification
{    
    //code
    //WM_CREATE
    
    //NSPoint:x,y NSSize:w, h
    NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0); //AS it is a C library, it follows C syntax
    window = [[NSWindow alloc] initWithContentRect:win_rect
                styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | 
                NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable 
                backing: NSBackingStoreBuffered
                defer: NO];
    
    [window setTitle: @"KAB:MacOSWindow"];
    [window center];
    
    
    view = [[MyView alloc]initWithFrame: win_rect];
    [window setContentView: view];
    [window setDelegate: self];
    [window makeKeyAndOrderFront: self]; //SetFocus, SetForegroundWindow
}

//When Application gets terminated NSApp's applicationWillTerminate -> like WM_DESTROY
-(void) applicationWillTerminate:(NSNotification *)aNotification
{
    //code
    //WM_DESTROY
}

-(void) windowWillClose:(NSNotification *)aNotification
{
    //code
    [NSApp terminate: self];
}

-(void) dealloc
{
    //code
    [view release];
    [window release];
    [super dealloc];
}
@end
//end AppDelegate


//start MyView
@implementation MyView 
{
    @private    
    NSString *centralText;
}

//function returning object (IS-AN-OBJECT) //like (void *) but for objects
-(id) initWithFrame:(NSRect)frame
{
    //code
    self = [super initWithFrame:frame];
    if(self)
    {
        centralText = @"Hello World !!!";
    }
    return self;
}

//InvalidateRect
-(void) drawRect:(NSRect)dirtyRect
{
    //code
    NSColor *backgroundColor = [NSColor blackColor];
    [backgroundColor set];
    NSRectFill(dirtyRect);
    NSDictionary *dictionaryForTextAttribute = 
                            [NSDictionary dictionaryWithObjectsAndKeys: 
                                [NSFont fontWithName: @"Helvetica" size:32], NSFontAttributeName,
                                [NSColor greenColor], NSForegroundColorAttributeName,
                                nil
                            ];

    NSSize textSize = [centralText sizeWithAttributes: dictionaryForTextAttribute];
    NSPoint point;
    point.x = (dirtyRect.size.width / 2) - (textSize.width / 2) + 12;
    point.y = (dirtyRect.size.height / 2) - (textSize.height / 2) + 12;

    [centralText drawAtPoint:point withAttributes:dictionaryForTextAttribute];
}

//How to shift keyboard and mouse control from NSWindow to NSView? -> implement acceptFirstResponder 
//Respond to Keyboard and Mouse
-(BOOL) acceptsFirstResponder //or 'acceptsFirstResponder
{
    //code
    [[self window] makeFirstResponder: self];
    return YES;
}

-(void) keyDown:(NSEvent *)theEvent
{
    //code 
    int key = [[theEvent characters] characterAtIndex:0];
    switch(key)
    {
        case 27: 
                // 'Esc'
            [self release];
            [NSApp terminate: self];
            break;
        
        case 'f':
        case 'F':
                //'FullScreen'
            [[self window] toggleFullScreen: self];
    }   
}

-(void) mouseDown:(NSEvent *)theEvent
{
    //code
    centralText = @"Left Mouse Button Is Clicked";
    [self setNeedsDisplay: YES];
}

-(void) rightMouseDown:(NSEvent *)theEvent
{
    //code    
    centralText = @"Right Mouse Button Is Clicked";
    [self setNeedsDisplay: YES];
}

-(void) otherMouseDown:(NSEvent *)theEvent
{
    //code    
    centralText = @"Hello World !!!";
    [self setNeedsDisplay: YES];
}

-(void) dealloc
{
    //code
    [super dealloc];
    //no need to release NSString centralText - control is with RunTime itself
}
@end
//end MyView



