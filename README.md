# Journey-like sand in OpenGL

This is my final project for TDT4230, accounting for 20% of my grade. In it, I
endeavour to create sand similar to the game Journey in OpenGL, inspired by
[this GDC talk](https://youtu.be/wt2yYnBRD3U).

# How to build

On Linux:

```
git clone --recursive git@github.com:solbjorg/opengl-sand.git
mkdir build
cd build
cmake ..
make
./glowbox
```

# Result

This progress gif shows most of the effects added to the scene:
![](progress.gif)

The glitter has been cranked up to make it more visible.

# Credit
I was entirely focused on implementing the graphical effect in OpenGL. Models and textures were lifted from Atwood Deng's similar Unity project, found [here](https://github.com/AtwoodDeng/JourneySand), used according to the [CC-BY-SA-4.0 license](https://creativecommons.org/licenses/by-sa/4.0/), share-alike being satisfied through [my use of the GPLv3 license](https://creativecommons.org/2015/10/08/cc-by-sa-4-0-now-one-way-compatible-with-gplv3/).
