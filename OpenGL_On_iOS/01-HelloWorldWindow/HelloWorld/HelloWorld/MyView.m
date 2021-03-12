//
//  MyView.m
//  HelloWorld
//
//  Created by DATTATRAY BADHE on 21/05/1940 Saka.
//
#import "MyView.h"

@implementation MyView
{
    NSString *centralText;
}

- (id)initWithFrame:(CGRect)frameRect
{
    self=[super initWithFrame:frameRect];
    if(self)
    {
        [self setBackgroundColor:[UIColor whiteColor]];
        
        centralText=@"HelloWorld";
        
        UITapGestureRecognizer *singleTapGuestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGuestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGuestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGuestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGuestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:2];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        [singleTapGuestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGuestureRecognizer=[[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [self addGestureRecognizer:swipeGuestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    return(self);
}
- (void)drawRect:(CGRect)rect
{
    UIColor *fillColor=[UIColor blackColor];
    [fillColor set];
    UIRectFill(rect);
    
    NSDictionary *dictionaryForTextAttributes=[NSDictionary dictionaryWithObjectsAndKeys:[UIFont fontWithName:@"Helvetica" size:24],
                                               NSFontAttributeName,
                                               [UIColor greenColor], NSForegroundColorAttributeName,
                                               nil];
    CGSize textSize=[centralText sizeWithAttributes:dictionaryForTextAttributes];
    
    CGPoint point;
    point.x=(rect.size.width/2)-(textSize.width/2);
    point.y=(rect.size.height/2)-(textSize.height/2)+12;
    
    [centralText drawAtPoint:point withAttributes:dictionaryForTextAttributes];
}

-(BOOL)acceptsFirstResponder
{
    return(YES);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
    centralText =@"onSingleTap Event";
    [self setNeedsDisplay];
}
-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    centralText =@"onDoubleTap Event";
    [self setNeedsDisplay];
}
-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    [self release];
    exit(0);
}
-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
    centralText =@"onLongPress Event";
    [self setNeedsDisplay];
}

-(void)dealloc
{
    [super dealloc];
}
/*
 // Only override drawRect: if you perform custom drawing.
 // An empty implementation adversely affects performance during animation.
 - (void)drawRect:(CGRect)rect {
 // Drawing code
 }
 */

@end
