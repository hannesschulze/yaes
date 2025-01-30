#import "view-controller.hh"
#import "scene.hh"

@implementation ViewController
{
	Scene* _scene;
	NSSize _sceneSize;
}

- (instancetype)init
{
	self = [super initWithNibName:nil bundle:nil];
	if (self)
	{
		_scene = [[Scene alloc] init];
		_sceneSize = [_scene size];
	}
	return self;
}

- (void)loadView
{
	auto view = [[SKView alloc] init];
	[view setIgnoresSiblingOrder:YES];
	[view setShowsFPS:YES];
	[view presentScene:_scene];

	[self setView:view];
}

- (NSSize)minimumDisplaySize
{
	return _sceneSize;
}

- (NSSize)preferredDisplaySize
{
	return NSMakeSize(_sceneSize.width * 3., _sceneSize.height * 3.);
}

@end