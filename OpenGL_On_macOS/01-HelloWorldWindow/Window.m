#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

@interface AppDelegate:NSObject <NSApplicationDelegate,NSWindowDelegate>
@end

@interface MyView:NSView
@end

int main(int argc,const char *argv[]){
    NSAutoreleasePool *pPool=[[NSAutoreleasePool alloc]init];
    NSApp=[NSApplication sharedApplication]; //Static method
    [NSApp setDelegate:[[AppDelegate alloc]init]];//This line states that Delegate me all the methods of this class(NSApplicatio) and send all messages you receive to me(NSApp).
/*
    ==>For the above line we can write in other way it is as follow:-
            AppDelegate *appDelegate=[[AppDelegate alloc]init];
            [NSApp setDelegate:appDelegate];
    ==>init is like calling to Constructor
 */
    [NSApp run];
    [pPool release]; //for that particular class's dealloc() method gets called.
    return 0;
}


@implementation AppDelegate{
//The Fields inside this interface are called as ClassField

    @private
        NSWindow *window;
        MyView *view;
}

-(void)applicationDidFinishLaunching:(NSNotification *) aNotification{
//NSNotification * has all the events,for whom the message is sent.
    NSRect win_rect;
    win_rect=NSMakeRect(0.0,0.0,800.0,600.0);
/*
    ==>win_rect is a valuetype and NSMakeRect is the referenceType.
    ==>NSMakeRect() is a macro-like function.
 */
    window=[[NSWindow alloc]initWithContentRect:win_rect
                                        styleMask:NSWindowStyleMaskTitled |
                                        NSWindowStyleMaskClosable |
                                        NSWindowStyleMaskMiniaturizable |
                                        NSWindowStyleMaskResizable
                                        backing:NSBackingStoreBuffered
                                        defer:NO];
/*
    ==>styleMask,backing,defer all are named parameters, this concept is of Objective C.
    ==>defer states that whether we want to make our window readable or reacting towards action as per we specify YES or NO.
    ==>Instead of TRUE or FALSE of BOOL like that of in Windows, for mac we have YES or NO.
 */
    [window setTitle:@"macOS Window"];//@ is called as literal and its type is NSString *.
    [window center];
    view=[[MyView alloc]initWithFrame:win_rect];
/*
    ==>Whenever win_rect get change then we write as follows:-
        view=[[MyView alloc]initWithFrame:win_rect];
 */
    [window setContentView:view];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void)windowWillClose:(NSNotification *)notification{
    [NSApp terminate:self];
}

-(void)dealloc{
    [view release];
    [window release];
    [super dealloc];
}
@end

@implementation MyView{
    NSString *centralText;//We cannot initialize while declaration we have to initialize via constructor.
}

-(id)initWithFrame:(NSRect)frame;
{
    self=[super initWithFrame:frame];
    if(self){
        [[self window]setContentView:self];
        centralText=@"Hello World!!!";
    }
/*
    ==>If Window's object hashcode and view's hashcode is same then ContentRect happens.
 */
    return (self);
}

-(void)drawRect:(NSRect)dirtyRect{
    NSColor *fillColor=[NSColor blackColor];
    [fillColor set];
    NSRectFill(dirtyRect);
    NSDictionary *dictionaryForTextAttributes=[NSDictionary dictionaryWithObjectsAndKeys:
                                               [NSFont fontWithName:@"Helvetica" size:32],
                                               NSFontAttributeName, [NSColor greenColor],
                                               NSForegroundColorAttributeName,
                                               nil];
/*
    ==>fontWithName is same like init"SomeName", it is constructor creates object and calls alloc and init.
    ==>nil tells that Dictionary has finished
 */
    
    NSSize textSize=[centralText sizeWithAttributes:dictionaryForTextAttributes];
    NSPoint point;
    point.x=(dirtyRect.size.width/2)-(textSize.width/2);
    point.y=(dirtyRect.size.height/2)-(textSize.height/2)+12;
    [centralText drawAtPoint:point withAttributes:dictionaryForTextAttributes];
}

-(BOOL)acceptsFirstResponder{
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent *)theEvent{
    int key=(int)[[theEvent characters]characterAtIndex:0];
    switch(key){
        case 27:
            [self release];
            [NSApp terminate:self];//instead of self nill is also OK.
            break;
        case 'F':
        case 'f':
            centralText=@"'F' or 'f' Key is Pressed!!!!";
            [[self window]toggleFullScreen:self];
            break;
        default:
            break;
    }
}

-(void)mouseDown:(NSEvent *)theEvent{
    centralText=@"Left Mouse Button is Clicked!!!!";
    [self setNeedsDisplay:YES];
}

-(void)mouseDragged:(NSEvent *)theEvent{
    
}

-(void)rightMouseDown:(NSEvent *)theEvent{
    centralText=@"Right Mouse Button is Clicked!!!!!";
    [self setNeedsDisplay:YES];
}

-(void)dealloc{
    [super dealloc];
}

@end
