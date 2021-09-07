# Changelog

## 4.0.1

Remove DPI scaling since it seems to already be handled by `SendInput`. Adjust default physics to accomodate. Increase default FPS to VSync * 1.5 because of unknown blips in frame time when setting it to exactly VSync.

## 4.0.0

A rewrite of most of kb-as-mouse, primarily focused on eliminating the bug which occasionally turns kb-as-mouse into a zombie process during high CPU loads.

The cause of the bug was due to the low-level keyboard hook being unhooked due to processing time during a call to `SendInput`, which would cause a memory access error. By adding intercepted key events from the low-level keyboard hook into a queue to be processed separately by another thread, the hook is able to return almost immediately, both lowering the possibility it is unhooked by Windows as well as prevent it from crashing kb-as-mouse due to `SendInput` being processed in a separate thread now.

This release also incorporates `rain` “Rosario” and various bugfixes and build procedure updates from there. The physics engine is altered slightly such that constants have slightly more meaning. And finally, the configuration specification has been moved from a file to the command-line options.

The icon for kb-as-mouse has changed, and changes between paused/unpaused states on the tray.

## 3.2.1

* Swapped default keys for `slowKey` and `rightClickKey`.
* Changed default mouse acceleration to `120` and default scroll acceleration to `400`.

## 3.2.0

* Upgrade to new `rain v6.4.8` build.
* Update project name to `kb-as-mouse` from `numpad-as-mouse`.
* Update icon to be that of a pointer instead of a numpad.
* Fix a bug where releasing a directional key while shift was held would not register.
* Removed dialog boxes when pausing/resuming/terminating.

## 3.1.0

* Tray icon now removes itself on normal exit.
* Ideally, `SendInput` should be more stable now, fixing a bug where program would very occasionally stall as soon as the left button was pressed.

## 3.0.3

* Update to `rain` `2.0.0`, which removes `css` dependencies, and relink files.

## 3.0.2

* Cleanup small amounts of code.
* Cleanup readme.

## 3.0.1

* Renamed `Settings` to lowercase.
* Change relevant `CRLF` to `LF`.
* Removed powers in physics and simplified physics calculations.
* Configuration file is now `.cfg`, and overwrites built-in default configuration options.
* Removed low-level mouse hook.
* Added `MessageBox` messages.
* Limit to one `numpad-as-mouse` instance.
* Added tray icon to manage program state (pause, exit).
* Updated app icon.

## 3.0.0

*This version does not build.*

* Rename to `numpad-as-mouse`.
* Update `rain` library.
* Started code refactor with style guide from `rain`.
* Renaming `Settings` to lowercase.

## 2.2.1

* now interprets all injected mouse messages except self-injected mouse messages (as intended), instead of skipping all injected mouse messages

## 2.2.0

* added options to move the mouse wheel horizontally, without physics implementation

## 2.1.2

* fixed bug where holding any of the click keys or pause would cause corresponding down messages to be sent multiple times (single wheel keys still work as before)
* fixed bug where the termination key would not have its effects blocked by the program upon release

## 2.1.1

* added setting for SlowMode speed reduction (or increase)

## 2.1.0

* added DiffMode, a different way mouse movement is processed, typically useful for applications where the mouse is fixed in a predetermined location, such as most framesPerSeconds or Minecraft
* added SlowMode, such that when a key is being held all scroll and mouse move accelerations are reduced by half
* added pause capability, which toggles the program functions, instead of turning it off directly like TerminateKey
* added scrollUpKeySingle/scrollDownKeySingle, two keys that send exactly one tick of the scroll wheel when pressed/held, unlike the scrollUpKey/scrollDownKey keys, which have physics applied to the scroll wheel
* cleaned up mouse messages to be adhere to standards more

## 2.0.0

* code refactoring
* reimplemented mouse and scroll wheel physics to be more flexible and natural
* reformatted config file and added flexibility to format interpretation
* reduced CPU usage by shutting off the timer while velocity of mouse or scroll wheel is low
* removed timer bug
* added program icon

## 1.0.0
