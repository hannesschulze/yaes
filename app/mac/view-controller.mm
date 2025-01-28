#import "view-controller.hh"
#import "controller-adapter-keyboard.hh"
#import "scene.hh"

@implementation ViewController
{
	Scene* _scene;
	NSSize _sceneSize;
	ControllerAdapterKeyboard* _keyboardController;
}

- (instancetype)initWithFilePath:(NSString*)filePath
{
	self = [super initWithNibName:nil bundle:nil];
	if (self)
	{
		_scene = [[Scene alloc] initWithFilePath:filePath];
		_sceneSize = [_scene size];

		_keyboardController = [[ControllerAdapterKeyboard alloc] init];
		[_scene setPrimaryController:_keyboardController];
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