# CSCI3260 Assignment
Here saves all my assignment of the course CSCI3260 Principles of Computer Graphics.

## Remark
* Use Visual Studio 2019 or above to create an empty C++ project
* Copy all the files into the folder where the `.vcxproj` file locates
* Add all the `.h` files into Header Files and all the `.cpp` files into Source Files
* Copy the whole folder `Dependencies` into `csci3260asg1`, `csci3260asg2` or `csci3260proj` (the one you want to test)
* Configuration Properties -> Linker -> General -> Additional Library Directories:
    * Path to `Dependencies/glm`
    * Path to `Dependencies/GLFW`
    * Path to `Dependencies/glew`
* Configuration Properties -> Linker -> Input -> Additional Dependencies:
    * `opengl32.lib`
    * `glfw3.lib`
    * `glew32.lib`
* Set the solution platform to x64
* Copy `Dependencies/glew/glew32.dll` to the folder where the executable file locates after building the project
