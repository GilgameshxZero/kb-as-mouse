# Changelog

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

* added DiffMode, a different way mouse movement is processed, typically useful for applications where the mouse is fixed in a predetermined location, such as most FPSs or Minecraft
* added SlowMode, such that when a key is being held all scroll and mouse move accelerations are reduced by half
* added pause capability, which toggles the program functions, instead of turning it off directly like TerminateKey
* added WheelUpSingle/WheelDownSingle, two keys that send exactly one tick of the scroll wheel when pressed/held, unlike the WheelUp/WheelDown keys, which have physics applied to the scroll wheel
* cleaned up mouse messages to be adhere to standards more

## 2.0.0

* code refactoring
* reimplemented mouse and scroll wheel physics to be more flexible and natural
* reformatted config file and added flexibility to format interpretation
* reduced CPU usage by shutting off the timer while velocity of mouse or scroll wheel is low
* removed timer bug
* added program icon

## 1.0.0
