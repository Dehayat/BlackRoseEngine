# GameEngine

Game engine with the goal of making a super simple and portable 2d platformer game engine inc c++.

## Setup

On Windows Just open is visual studio and build
but make sure you have vcpkg in your visual studio installation
On other platforms it should compile but you have to run vcpkg and configurations are only on the visual studio solution. 
but you should still be able to run release and debug no problem. and to run in editor config just define _EDITOR

## Usage

Level.yaml file has en example of loading everything supported.
loading textures happens in the setup function in Game.cpp along with loading the level.

#