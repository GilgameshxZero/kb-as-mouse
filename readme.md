# numpad-as-mouse

Perform mouse actions with the keyboard on Windows.

Smoother than the Windows default option. Options to use keys other than the numpad.

You'll want to grab the `rain` submodule with:

```bash
git submodule init
git submodule update
```

## Configuration

The keys used in `numpad-as-mouse` can be configured by adding a `.cfg` file in the working directory of `numpad-as-mouse`. `.cfg` can contain the following values:

Option|Default|Usage
-|-|-
`upKey`|`104`|Key code to move mouse up.
`rightKey`|`102`|Key code to move mouse right.
`downKey`|`101`|Key code to move mouse down.
`leftKey`|`100`|Key code to move mouse left.
`leftClickKey`|`96`|Key code to left click.
`rightClickKey`|`110`|Key code to right click.
`middleClickKey`|`98`|Key code to middle click.
`scrollUpKey`|`105`|Key code to scroll up.
`scrollDownKey`|`103`|Key code to scroll down.
`scrollLeftSingleKey`|`97`|Key code to scroll left one notch.
`scrollRightSingleKey`|`99`|Key code to scroll right one notch.
`slowKey`|`107`|Key code to apply a modifier to acceleration values for mouse movement and scrolling.
`pauseKey`|`-1`|Key code to pause the application.
`terminateKey`|`-1`|Key code to terminate the application.
`framesPerSecond`|`90`|Frames per second for the physics engine. Higher results in smoother motion and movement, but decreased performance.
`mouseResistance`|`10`|"Air resistance" applied to mouse movement.
`mouseAcceleration`|`80`|Acceleration applied to mouse when moved.
`scrollResistance`|`10`|"Air resistance" applied to scrolling.
`scrollAcceleration`|`500`|Acceleration applied to scroll wheel when moved.
`slowRatio`|`0.2`|Modifier applied to mouse movement and scrolling accelerations when `slowKey` held.

A `-1` invalidates the key code option. Specify key codes with VK codes (<https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx>).

An example `.cfg` might look like the following:

```cfg
terminateKey 93
slowRatio 0.3
```

## Changelog

### 3.1.0

* Tray icon now removes itself on normal exit.
* Ideally, `SendInput` should be more stable now, fixing a bug where program would stall as soon as the left button was pressed very occasionally.

### 3.0.3

* Update to `rain` `2.0.0`, which removes `css` dependencies, and relink files.

### 3.0.2

* Cleanup small amounts of code.
* Cleanup readme.

### 3.0.1

* Renamed `Settings` to lowercase.
* Change relevant `CRLF` to `LF`.
* Removed powers in physics and simplified physics calculations.
* Configuration file is now `.cfg`, and overwrites built-in default configuration options.
* Removed low-level mouse hook.
* Added `MessageBox` messages.
* Limit to one `numpad-as-mouse` instance.
* Added tray icon to manage program state (pause, exit).
* Updated app icon.

### 3.0.0

*This version does not build.*

* Rename to `numpad-as-mouse`.
* Update `rain` library.
* Started code refactor with style guide from `rain`.
* Renaming `Settings` to lowercase.

### 2.2.1

* now interprets all injected mouse messages except self-injected mouse messages (as intended), instead of skipping all injected mouse messages

### 2.2.0

* added options to move the mouse wheel horizontally, without physics implementation

### 2.1.2

* fixed bug where holding any of the click keys or pause would cause corresponding down messages to be sent multiple times (single wheel keys still work as before)
* fixed bug where the termination key would not have its effects blocked by the program upon release

### 2.1.1

* added setting for SlowMode speed reduction (or increase)

### 2.1.0

* added DiffMode, a different way mouse movement is processed, typically useful for applications where the mouse is fixed in a predetermined location, such as most framesPerSeconds or Minecraft
* added SlowMode, such that when a key is being held all scroll and mouse move accelerations are reduced by half
* added pause capability, which toggles the program functions, instead of turning it off directly like TerminateKey
* added scrollUpKeySingle/scrollDownKeySingle, two keys that send exactly one tick of the scroll wheel when pressed/held, unlike the scrollUpKey/scrollDownKey keys, which have physics applied to the scroll wheel
* cleaned up mouse messages to be adhere to standards more

### 2.0.0

* code refactoring
* reimplemented mouse and scroll wheel physics to be more flexible and natural
* reformatted config file and added flexibility to format interpretation
* reduced CPU usage by shutting off the timer while velocity of mouse or scroll wheel is low
* removed timer bug
* added program icon

### 1.0.0
