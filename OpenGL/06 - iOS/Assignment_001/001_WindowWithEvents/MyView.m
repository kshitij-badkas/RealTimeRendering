//
//  MyView.m
//  HelloWorld
//
//  Created by STC on 03/07/21.
//

#import "MyView.h"

@implementation MyView
{
    @private
    NSString *centralText;
}

-(id)initWithFrame:(CGRect)frame
{
    // code
    self = [super initWithFrame:frame];
    if(self)
    {
        centralText = @"Hello World !!!";
        
        UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [swipeGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [longPressGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    
    return (self);
}

-(void)drawRect:(CGRect)rect
{
    // Drawing code
    UIColor *backgroundColor = [UIColor blackColor];
    [backgroundColor set];
    UIRectFill(rect);
    
    NSDictionary *dictionaryForTextAttribute = [NSDictionary dictionaryWithObjectsAndKeys:
                                                [UIFont fontWithName: @"Helvetica" size:32], NSFontAttributeName,
                                                [UIColor greenColor], NSForegroundColorAttributeName,
                                                nil];
    
    CGSize textSize = [centralText sizeWithAttributes: dictionaryForTextAttribute];
    CGPoint point;
    point.x = (rect.size.width / 2) - (textSize.width / 2) + 12;
    point.y = (rect.size.height / 2) - (textSize.height / 2) + 12;
    
    [centralText drawAtPoint:point withAttributes:dictionaryForTextAttribute];
    
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
    // code
    centralText = @"Single Tap";
    [self setNeedsDisplay];
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    // code
    centralText = @"Double Tap";
    [self setNeedsDisplay];
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    // code
    //centralText = @"Swipe";
    //[self setNeedsDisplay];
    
    //exit
    [self release];
    exit(0);
}

-(void)onLongPress:(UISwipeGestureRecognizer *)gr
{
    // code
    centralText = @"Long Press";
    [self setNeedsDisplay];
}

-(void)dealloc
{
        // code
    [super dealloc];
}

@end
