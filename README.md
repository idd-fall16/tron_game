# CS294 Assignment 3: Video Game Controller
Ethan Wells, Tyler Chesebro, T.S. Yew

We created two game controllers to play the Tron game on Microsoft Windows. One controller uses an Adafruit accelerometer while the other uses two Adafruit pressure sensors. The accelerometer can be tilted left or right to change the player's direction, or tilted up to brake. The two pressure sensors control direction and if both are pressed together, the brake is activated.

Both controllers are connected to a Redbear Duo running Arduino C++ code. A program like 232key is needed to translate the Duo's serial output into PC keystrokes.
