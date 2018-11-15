# Game Of Stones

More info: [alexsaalberg.com/gos.html](https://alexsaalberg.com/gos.html)

![Screenshot showing overview of features in Game of Stones](media/screen1.png)

## Description

Game of Stones was my senior project. It was created in a little under 10 weeks. 

It was meant to be an exploration into the type of speciality controls that might be needed to make a fully-3D "Colony Simulator" type game. (Imagine [Rimworld](https://rimworldgame.com/) rendered in Minecraft). 

Most of the time was spent simply making the rendering engine, but some basic controllable 'colonists' npcs were added as well as some interesting 3D voxel building controls

## Technologies

Language - C++

Source Control - Git

Building - CMake1
### Libraries

#### [Polyvox](http://www.volumesoffun.com/polyvox-about/)

Used for management of voxel data, generation of triangle meshes from voxel data, and for some of it's pathfinding utility functions.

#### [BulletPhysics3](https://github.com/bulletphysics/bullet3)

Used for physics simulation (duh).

#### [Glad](https://github.com/Dav1dde/glad)

Used for loading OpenGL functions.

#### [Imgui](https://github.com/ocornut/imgui)

User for debug output.

## Controls
  It’s important to understand that there are two conceptual “control modes” in this game. They are called “RTS mode” and “FPS mode.” They can be switched between by hitting the “G” key. There are some controls that are available in both modes.
  
  In FPS mode it is easier to move around and place/destroy individual blocks. Controls in this mode are fairly identical to the game ‘Minecraft’. (Left Click to destroy, Right Click to build).
  
  In RTS mode it is possible to select (LeftClick) and direct (RightClick) colonists (the grey cubes). It is also possible to select 3D volumes of the world (LeftMouse+Drag). Once a volume is selected it can be used to select multiple colonists at once, or destroy and build blocks.

Here is a summary of the controls available
#### Both Modes
* WASD to move
* Space to jump
* 1-9 select block color 
* Hit G to switch control modes

#### FPS Mode
* Move Mouse to change camera
* Left Click to destroy
* Right Click to build

#### RTS Mode
* Hold Right-Click and Move Mouse to change camera
* Left-Click to select a colonist
* Right-Click to tell all selected colonists to move somewhere
* Left-Drag to create a selection
  * Hit B to build in selection
  * Hit V to delete in selection
  * Hit C or Left-Click to cancel selection
  * Hit F to select colonists in selection

## Naming Conventions
* Variables (local and member)
  * snake_case 
* Classes
  * PascalCase
* Functions
  * camelCase

