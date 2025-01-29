#import "view-controller.hh"
#import "controller-adapter-keyboard.hh"
#import "controller-adapter-gamepad.hh"
#import "scene.hh"

@implementation ViewController
{
	Scene* _scene;
	NSSize _sceneSize;
	ControllerAdapterKeyboard* _keyboardController;
	NSMutableArray<ControllerAdapterGamepad*>* _gamepadControllers;
}

- (instancetype)initWithFilePath:(NSString*)filePath
{
	self = [super initWithNibName:nil bundle:nil];
	if (self)
	{
		_scene = [[Scene alloc] initWithFilePath:filePath];
		_sceneSize = [_scene size];

		_keyboardController = [[ControllerAdapterKeyboard alloc] init];
		_gamepadControllers = [NSMutableArray array];
		for (GCController* controller in [GCController controllers])
		{
			auto gamepad = [controller extendedGamepad];
			if (gamepad)
			{
				auto adapter = [[ControllerAdapterGamepad alloc] initWithProfile:gamepad];
				[_gamepadControllers addObject:adapter];
			}
		}
		[self updateControllers];

		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleControllerConnected:)
													 name:GCControllerDidConnectNotification
												   object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleControllerDisconnected:)
													 name:GCControllerDidDisconnectNotification
												   object:nil];
	}
	return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)loadView
{
	auto view = [[SKView alloc] init];
	[view setIgnoresSiblingOrder:YES];
	[view setShowsFPS:YES];
	[view presentScene:_scene];

	[self setView:view];
}

- (void)updateControllers
{
	// Prefer controllers, then fallback to keyboard input.
	if ([_gamepadControllers count] >= 1)
	{
		[_scene setPrimaryController:[_gamepadControllers objectAtIndex:0]];
		if ([_gamepadControllers count] >= 2)
		{
			[_scene setSecondaryController:[_gamepadControllers objectAtIndex:1]];
		}
		else
		{
			[_scene setSecondaryController:_keyboardController];
		}
	}
	else
	{
		[_scene setPrimaryController:_keyboardController];
		[_scene setSecondaryController:nil];
	}
}

- (void)handleControllerConnected:(NSNotification*)notification
{
	auto controller = (GCController*)[notification object];
	auto gamepad = [controller extendedGamepad];
	if (gamepad)
	{
		auto adapter = [[ControllerAdapterGamepad alloc] initWithProfile:gamepad];
		[_gamepadControllers addObject:adapter];
	}
	[self updateControllers];
}

- (void)handleControllerDisconnected:(NSNotification*)notification
{
	auto controller = (GCController*)[notification object];
	for (auto i = NSUInteger{ [_gamepadControllers count] }; i > 0; --i)
	{
		auto adapter = [_gamepadControllers objectAtIndex:(i - 1)];
		if ([[adapter profile] controller] == controller)
		{
			[_gamepadControllers removeObjectAtIndex:(i - 1)];
		}
	}
	[self updateControllers];
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