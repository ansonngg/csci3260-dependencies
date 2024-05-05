# CSCI3260 Dependencies
These are the original dependencies used in CSCI3260 assignments. I leave them as a backup in case the packages installed from vcpkg do not work or someone likes to test the behaviour of the code when using the original dependencies.

## Instructions
* Use Visual Studio 2019 or above to create an empty C++ project
* Copy all the files in one of the following repositories into the folder where the `.vcxproj` file locates
    * [ansonngg/csci3260-asg1](https://github.com/ansonngg/csci3260-asg1)
    * [ansonngg/csci3260-asg2](https://github.com/ansonngg/csci3260-asg2)
    * [ansonngg/csci3260-proj](https://github.com/ansonngg/csci3260-proj)
* Add all the `.h` files into Header Files and all the `.cpp` files into Source Files
* Copy all the files in this repository into the folder where the `.vcxproj` file locates
* Configuration Properties -> Linker -> General -> Additional Library Directories:
    * Path to `glm`
    * Path to `GLFW`
    * Path to `GL`
* Configuration Properties -> Linker -> Input -> Additional Dependencies:
    * `opengl32.lib`
    * `glfw3.lib`
    * `glew32.lib`
* Set the solution platform to x64
* Copy `GL/glew32.dll` to the folder where the executable file locates after building the project
