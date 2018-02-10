# cpsc436d

## Project Info

### Info

#### Pitch
- [OneDrive - DOCX](https://onedrive.live.com/view.aspx?resid=C3F0298314E4ECCD!284946&ithint=file%2cdocx&app=Word&authkey=!AJgdQbFssTUXqhU)

#### Presentation 1
- [OneDrive - PPTX](https://onedrive.live.com/view.aspx?resid=C3F0298314E4ECCD!295036&ithint=file%2cpptx&app=PowerPoint&authkey=!AHZauhwFBn72fPo)

#### Presentation 2
- [OneDrive - PPTX](https://onedrive.live.com/view.aspx?resid=C3F0298314E4ECCD!303764&ithint=file%2cpptx&app=PowerPoint&authkey=!APpASRWldZN18Xs)

### Dependencies
- SDL2
- SDL2_mixer
- OpenGL 3.3

### How to run
- Git clone
- Pull in submodule dependencies: `git submodule update --init --recursive`

#### Installation on Ubuntu
```bash
apt-get install -y cmake g++ libsdl2-dev libsdl2-mixer-dev libglfw3-dev
```

#### Compiling
```bash
mkdir build
cd build 
cmake ..
make -j4
```
#### Installation on Windows

You should be able to simply open `visual_studio/Attempt 1.sln` and run it the same way you would any other Visual Studio project.

## Milestone 1

### Requirements

**Requirements set in our own pitch document are italicized**

 - [x] Loading and rendering of non‐trivial geometry 
 - [x] *Ability to load .obj files and correctly load their material and texture data for use in the shader code*
 
I'm not sure any of our geometry is trivial. Maybe the sand tiles? Even those are loaded from an OBJ with a seperate MTL file for the material information though.

 - [x] Loading and rendering of textured  geometry with correct blending 
 - [x] Basic shaders as needed for rendering: Similar to what was
seen in the first assignment 
 - [x] *Shader that implements Phong shading and handles textures if present*
 
We have Phong shading that currently doesn't compute the specular highlights because as of right now we don't have any materials with a non 0 specular material. This is expected to change and is not much work to add so I imagine it'll be added to milestone 2.

 - [x] Working 2D Transformations 

The balls move across the screen by having their Mat4 model transformed! 

 - [x] Response to user input (mouse, keyboard): including changes in the set of rendered objects, object eometry, position, orientation, textures, colors, and other attributes.  
 - [x] *Fully functioning camera (allows the player to move and rotate the view as they please)*

We have a working camera that we use to create a view matrix. The camera supports moving (WASD, ←↑→↓), rotating on the camera's up axis (QE), rotating around the camera's horizontal axis (scroll wheel), zooming and unzooming (Z + scroll wheel)

 - [x] Basic key‐frame/state interpolation (smooth movement from point A to point B in Cartesian or angle space).  
 - [x] *Have entities that can move to a given location (with movement happening smoothly between the two points, but not with pathing logic to avoid obstacles)*
 
The balls move smoothly to a target location (they bounce because we then change their target location, not because of any collision detection).

 - [x] *Add a skybox*
 
 We did, and it looks awesome! It even uses a cube texture

- [x] *Have the ability to select a tile*

We can (you can change the selected tile using IJKL). Admittedly we're going to change this to use the mouse in the future but thatsa surprisingly difficult (yay 3D!)

### Non Requirement Stuff thats worth mentioning

 - [x] We added a pipeline to test our builds!
 - [x] We made and found a bunch more assets
 - [x] We did a bunch of refactoring so future additions are easier
 - [x] We found (and fixed) a frankly disturbing number of bugs
