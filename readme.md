# kb-as-mouse (KBAM)

Perform mouse actions with the keyboard (Windows-only).

Option to customize keymapping (movement, scrolling, modifier) and acceleration/resistance behavior.

By default, movement is controlled by the `pl;'` keys, and left click is right ALT. You can pause the program by pressing the applications/hamburger icon key (most keyboards have this), or by left-clicking/right-clicking the tray icon. Other defaults/customization options are described in a later section.

Note: numpad keys are usable—however, they’re still a bit buggy when used with SHIFT. I’m looking for a workaraound for this.

## Why use KBAM instead of [AutoHotkey, Windows Ease of Access, etc.]?

KBAM is built for simplicity—while you could accomplish the same thing with an AHK script, customization of the script is more difficult and requires you to learn the range of the AHK scripting language. Other options, such as Window’s built-in Ease of Access allowing for use of the numpad as a mouse, is limited in scope and has fewer options (acceleration, resistance, customization of keys, etc.).

## FAQ

### KBAM isn’t working in some windows (Task Manager, games, etc.).

Task Manager and many games are run in Adminstrator Mode (elevated). If KBAM is not run elevated, it cannot inject events into elevated applications. So, run KBAM with elevated permissions (right click KBAM, and select “Run as administrator”).

## Customization

The keys used in KBAM can be configured by adding a `kb-as-mouse.cfg` file in the working directory of `numpad-as-mouse`. `kb-as-mouse.cfg` can contain the following values:

Option|Default|Usage
-|-|-
`upKey`|`80` (`p` key)|Key code to move mouse up.
`rightKey`|`222` (`'` or `"`)|Key code to move mouse right.
`downKey`|`186` (`;` or `:`)|Key code to move mouse down.
`leftKey`|`76` (`l`)|Key code to move mouse left.
`leftClickKey`|`165` (right ALT)|Key code to left click.
`rightClickKey`|`221` (`]` or `}`)|Key code to right click.
`middleClickKey`|`161` (right SHIFT)|Key code to middle click.
`scrollUpKey`|`219` (`o`)|Key code to scroll up.
`scrollDownKey`|`79` (`[` or `{`)|Key code to scroll down.
`scrollLeftSingleKey`|`190` (`.` or `>`)|Key code to scroll left one notch.
`scrollRightSingleKey`|`191` (`/` or `?`)|Key code to scroll right one notch.
`slowKey`|`13` (ENTER)|Key code (held) to apply a modifier to acceleration values for mouse movement and scrolling.
`pauseKey`|`93` (applications i.e. hamburger menu key)|Key code to pause the application.
`terminateKey`|`-1` (none)|Key code to terminate the application.
`framesPerSecond`|`90`|Frames per second for the physics engine. Higher results in smoother motion and movement, but decreased performance.
`mouseResistance`|`10`|"Air resistance" applied to mouse movement.
`mouseAcceleration`|`120`|Acceleration applied to mouse when moved.
`scrollResistance`|`10`|"Air resistance" applied to scrolling.
`scrollAcceleration`|`400`|Acceleration applied to scroll wheel when moved.
`slowRatio`|`0.2`|Modifier applied to mouse movement and scrolling accelerations when `slowKey` held.

A `-1` invalidates the key code option. Specify key codes with VK codes: <https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes>. Note that the configuration file takes numbers in decimal—whereas they’re specified in hexidecimal on MSDN.

An example `.cfg` might look like the following:

```cfg
terminateKey 93
slowRatio 0.3
```

## Known Issues

* Fix occasional stalling bug when left mouse button is simulated.
* Check physics/hook engines and make sure they’re up to standard.
* SHIFT+numpad still doesn’t quite work.

## Build

You'll want to grab the `rain` submodule with:

```bash
git submodule init
git submodule update
```

The project is setup as a Visual Studio project in `build/`.
