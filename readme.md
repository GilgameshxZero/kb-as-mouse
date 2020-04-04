# numpad-as-mouse

Converts the numpad into mouse movement on Windows. Smoother than the Windows default option. Options to use other keys than the numpad.

---

Keys specified in the configuration file (config.txt) are to be VK codes. You can find a list of VK codes here: https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx. The scroll wheel mechanics are exactly the same as the mouse movement mechanics on each axis. Negative numbers, such as -1, will disable a key (however, it is ill-advised to input a negative number for non-key settings).

All however many settings (see provided config.txt for an example) must be included in the configuration file or the program will fail. Please see the keyasmouse_errors.txt file for additional information for other failures.

Acceleration denotes, in pixels/s^2, the velocity that will be gained by the object per second the related key is held down.
Resistance denotes the coefficient which determines the final acceleration of friction against the current velocity direction of the object. This coefficient is scaled by the current velocity.
ResistancePower denotes the exponent to which the velocity is raised in the calculation of resistance friction. The high the resistance power, the more the velocity factors into friction. If the resist power is very high, then the force of friction will quickly overpower acceleration as velocity increases. If the resist power is very low, then the force of friction varies little over different velocities, and is mostly determined by resistance. Set ResistPower to 2 for a simulation of air resistance.

Acceleration Against Velocity (friction) = Resistance * Velocity ^ ResistancePower

Low FramesPerSecond may result in instabilities; for best results, run at FPS 60.

When the TerminateKey is pressed, the program will exit.
