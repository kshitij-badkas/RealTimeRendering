// mkdir -p Window.app/Contents/MacOS
// clang -Wno-deprecated-declarations -o Window.app/Contents/MacOS/Window Window.m -framework cocoa -framework QuartzCore -framework OpenGL

//Header files
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to Windows.h and Xlib.h
//new
#import <QuartzCore/CVDisplayLink.h> //audio-video core : CV-> Core Video
#import <OpenGL/gl3.h> // gl.h

//Callback function
CVReturn myDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *,
                                CVOptionFlags, CVOptionFlags *, void *);

//Global variables
FILE *gpFile = NULL;

//Forward Declaration
@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

//main
int main(int argc, char* argv[])
{
    //code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];

    //Message Loop or Run Loop or Event Loop
    [NSApp run];

    [pool release];

    return 0;
}

//When Application gets terminated ApplicationWillTerminate -> like WM_DESTROY


@interface MyOpenGLView:NSOpenGLView
@end

@implementation AppDelegate
{
    @private 
    NSWindow *window;
    MyOpenGLView *myOpenGLView;
}

//instance method
-(void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    //code
    //Create Log File
    NSBundle *appBundle = [NSBundle mainBundle]; //Class method //Application's package '.app'
    NSString *appDirPath = [appBundle bundlePath]; //instance method
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/Log.txt", parentDirPath];
    const char *pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    gpFile = fopen(pszLogFileNameWithPath, "w");
    
    if(gpFile == NULL)
    {
        [self release];
        [NSApp terminate:self];
    }
    
    fprintf(gpFile, "Program Started Successfully.\n\n");
    
    //NSPoint:x,y NSSize:w, h
    NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0); //AS it is a C library, it follows C syntax
    window = [[NSWindow alloc] initWithContentRect:win_rect
                styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | 
                NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable 
                backing:NSBackingStoreBuffered
                defer:NO];
    
    [window setTitle:@"KAB:OpenGL Blue Window"];
    [window center];
    
    myOpenGLView = [[MyOpenGLView alloc]initWithFrame:win_rect];
    [window setContentView:myOpenGLView];
    //was commented first
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void) applicationWillTerminate:(NSNotification *)aNotification
{
    //code
    if(gpFile)
    {
        fprintf(gpFile, "\nProgram Terminated Successfully.\n");
    }
    fclose(gpFile);
    gpFile = NULL; 
}

-(void) windowWillClose:(NSNotification *)aNotification
{
    //code
    [NSApp terminate:self];
}

-(void) dealloc
{
    //code
    [myOpenGLView release];
    [window release];
    [super dealloc];
}
@end
//AppDelegate end


@implementation MyOpenGLView
{
    @private
    CVDisplayLinkRef displayLink;
}

-(id) initWithFrame:(NSRect)frame
{
    //code
    self = [super initWithFrame:frame];
    if(self)
    {
        //PixelFormatDescriptor - Win, FrameAttributes - *nix
        NSOpenGLPixelFormatAttribute attributes[] =
        {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery, //don't allow to choose software rendering automatically if hardware rendering is not possible
            NSOpenGLPFAAccelerated, //hardware rendering
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer, 
            0
        };
        
        //ChoosePixelFormat
        NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
        if(pixelFormat == nil)
        {
            fprintf(gpFile, "Cannot get Pixel Format. Exitting.\n");
            [self release];
            [NSApp terminate:self];
        }
        
        //get openGL context
        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil] autorelease];
        
        //set
        [self setPixelFormat:pixelFormat];
        [self setOpenGLContext:glContext];
            
    }
    return self;
}

-(CVReturn) getFrameForTime:(const CVTimeStamp *)outputTime
{
    //code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; //create a new thread ->rendering thread -Multithreading
    [self drawView];
    [pool release];
    return kCVReturnSuccess;
}


//initialize
-(void) prepareOpenGL
{
    //code
    [super prepareOpenGL];

    //Objective C madhe get karaycha asel te direct lihaycha 'get' lihinyachi garaz nahi
    [[self openGLContext] makeCurrentContext];

    //OPENGL RELATED LOG
    fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
    fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
    fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    //swap interval - to avoid image tearing - monitor vertical sync and refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; //OpenGLCP - OpenGL Context Parameter

    //
    //from shaderSourceCode - recording 3:31
    //
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); //blue

    //Core Video and Core Graphics Library related code - CV and CGL
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &myDisplayLinkCallback, self);
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

//resize
-(void) reshape
{
    //code
    [super reshape];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    NSRect rect = [self bounds];
    if(rect.size.height < 0)
    {
        rect.size.height = 1;
    }
    glViewport(0.0f, 0.0f, (GLsizei)rect.size.width, (GLsizei)rect.size.height);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

//InvalidateRect - WM_PAINT - we use this for supporting multithreading 
-(void) drawRect:(NSRect)dirtyRect
{
    //code
    [self drawView];
}

//Display
-(void) drawView
{
    //code
    [[self openGLContext] makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Swap Buffers / Double Buffers
    CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

//Respond to Keyboard and Mouse
-(BOOL) acceptsFirstResponder
{
    //code
    [[self window] makeFirstResponder:self];
    return YES;
}

-(void) keyDown:(NSEvent *)theEvent
{
    //code 
    int key = [[theEvent characters] characterAtIndex:0];
    switch(key)
    {
        case 27: 
            [self release];
            [NSApp terminate:self];
            break;
        
        case 'f':
        case 'F':
            [[self window] toggleFullScreen:self];
    }   
}

-(void) mouseDown:(NSEvent *)theEvent
{
    //code
}

-(void) rightMouseDown:(NSEvent *)theEvent
{
    //code
}

-(void) otherMouseDown:(NSEvent *)theEvent
{
    //code
}

-(void) dealloc
{
    //code
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    [super dealloc];
}
@end


//As Callback fucntion is C style function it should be writtenin global space.
//Therefore we are wrting it after @end

//last parameter is MyOpenView class's pointer -recording : -1:05:00 to -1:15:00
CVReturn myDisplayLinkCallback(CVDisplayLinkRef displayLink, 
                                const CVTimeStamp *now, const CVTimeStamp *outputTime, 
                                CVOptionFlags flagsIn, CVOptionFlags *flagsOut, 
                                void *displayLinkContext)
{
    //code
    CVReturn result = [(MyOpenGLView *)displayLinkContext getFrameForTime:outputTime];
    return result;
}

