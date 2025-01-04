# EscapeDungeon
A simple game interaction interface based on the programmable pipeline OpenGL API, realising the following functions:

- Window display and interaction callback function settings (window adjustment, etc.): GLFW library
- Camera setup and movement, currently set to FOLLOW mode.
- Import and display of common models, import of models with bones and animations: ASSIMP library.
  - Import static models with obj, dynamic models with dae, fbx, etc.
  - Vertex shader support for animation playback
  - Fragment shader for normal or Blinn-Phong lit models (must work with the selected imported model, not currently lit).
- Background music and action sound effects playback: IrrKlang library
- Physics effects simulation and collision detection: PhysX library
  - Use of static and dynamic rigid bodies, with the player as a kinematic rigid body




#### Project Architecture

The basic design refers to LearnOpenGL, based on which it is further structured to facilitate custom import and placement of the specified model, as well as to follow the OOP programming principles; the structural relationship of the project's main classes is shown in the following figure
 

The main logic of the programme is implemented in main.cpp, from here you can start to sort out the project code structure, there are basic comments in the code, if you can't understand it, then come back to ask. The basic flow of the project runtime is shown in the following figure



#### Usage

How to use the project: Use Visual Studio 2022, with CMake management; cross-platform has not been verified, at least the dll and lib need to be replaced.

- Open Visual Studio 2022, select ‘Open’ → ‘CMake’ and open the project through CMakeLists.txt.
- Adjust the project to run in Release mode, the project MSVC compiler options have been set to MT (GLFW libraries and PhysX libraries are compiled with MT options).
- Before running, remember to switch to CMakeLists.txt and save it to refresh the CMake cache. This will copy the *.dll and assets to the executable generation directory for the application to use.

Importing custom models: In the `World::initObjects()` function in the World.cpp file, this function first compiles and builds the specified shader, then imports the model with the specified path, then builds the object with the specified shader, model, and position according to the configuration, and finally optionally generates the object's physical scene in PhysX. The shape (rigid body) of the Object in the PhysX physics scene can be optionally generated to participate in the physics simulation.

- The reason for separating Model and Object construction is to allow reuse of models, i.e. different Objects can point to the same Model and different Objects can point to the same shader.
- There are two Object constructors, which can be passed directly into the model matrix, as well as position and orientation (default world_up is y-axis positive, **note the limitations on the use of Euler angles, and orientation can't be along the y-axis initially**); you can try to add a third Object constructor, which uses the rotated quaternion directly (might help to understand the code, and to set the initial shape better! )
- Physical rigid body creation: call `createRigidDynamic` or `createRigidStatic` according to your needs; Player and Ground's physical initialisation is relatively special, so it is not recommended to change it.

The basic operation mode when the programme is running

- Keyboard input: WASD to control the player, FOLLOW mode to move the camera; pressing space will output the coordinates of the current character in stdout, which can be used for debugging or measuring.
- Mouse move: ordinary situation control camera Euler angle, the current FOLLOW mode camera does not respond to
- Mouse wheel: camera perspective zoom



### Note

1. models mainly from Sketchfab, the site model quality and usability seems to be higher; can be re-selected by yourself, from Blender to export obj need to note that
   - Make sure to unpack the **texture**, and then re-add the new unpacked texture in the shader view, otherwise the texture may be lost when importing (tips: you can check the obj file directly after exporting, if the texture is not lost, it means it's ok)
   - Meanwhile, pay attention to the path of the texture when exporting, choose ‘relative path’, and keep the same relative path when copying to project assets, otherwise the texture will be lost when importing (tips: you can check the path in the corresponding mtl file, if it is not right, you can directly modify the path to the correct relative path, provided that you Make sure the previous point is correct)
2. the model of the drive animation can be in dae format, but when the project imported the newly exported dae file from Blender, the rendered vertex position will be very strange (it looks like the order of quaternion doesn't match), and the current version of ASSIMP doesn't support the newly exported glb format of Blender, after testing the project, the animation in fbx format is imported correctly, so the main animation is in fbx format.
   - The skeleton and walking animation of Player's corresponding model is added by myself, which is a bit rough; I can't seem to find any character model with reasonable skeleton and animation for the time being.
   - The model of the ground is also built by myself with Blender, i.e. flat + texture mapping, which is rather crude; it seems to be difficult to find a scene model with reasonable skeleton and animation.
3. the ground in PhysX can't be used as a Plane, because without thickness RigidDynamic would pass right through it, so it was given thickness in the physics simulation

4. The Player's corresponding dynamic rigid body is set with a ‘kinematic rigid body’ `setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true)`, controlled only by user input, and the expected position is passed in prior to each frame of the simulation using `rigid_dynamic->setKinematicTarget(transform_to_test)`. rigid_dynamic->setKinematicTarget(transform_to_test)` to pass in the expected position, which is subsequently determined when `mScene->simulate(...) ` when determining; the physical model of the character is simulated using a simple wraparound box
   - It is suspected that the `px_triangle_mesh` built from the model doesn't work well, so the Player's physical shape in PhysX is statically configured to be a simple AABB box, with `_aabb_hy` as its half-height.
5. Collision detection between kinematic and static rigid bodies RigidStatic in PhysX is not yet successful: it is not yet known how to trigger the callback function correctly; **Collisions between kinematic and dynamic rigid bodies can be validated**, see collision between the character and the barrels in the behaviour of the runtime application
6. the world field has a border `border` of ±30.0f, which the character does not exceed
7. the imported walking sound is distorted at some points in time, but the first step is normal, maybe try cutting it to just the first step and looping it.



### Reference

Third-party libraries: they are all integrated in the thirdparty directory, and can be used directly on Windows.

- OpenGL 4.6
- GLFW 3.4: /MT build
- Glad
- Assimp 3.3.1
- PhysX 5.5
- IrrKlang 1.5

References

- LearnOpenGL website https://learnopengl-cn.github.io/ projects https://github.com/JoeyDeVries/LearnOpenGL 
  - More generalised tutorials https://www.bilibili.com/video/BV1aK4y1z7ii
- For reference on how to use PhysX
  - Official documentation https://nvidia-omniverse.github.io/PhysX/physx/5.5.0/index.html
  - Initialisation examples https://www.youtube.com/watch?v=zOYpVAoQFyU
  - Project https://github.com/kmiloarguello/openGL-physX, locally compilable, but uses PhysX 4.1, which is different from the 5.5 implementation.

Auxiliary tools

- Blender 4.3.2

Sources

- Sketchfab, CGTrader, Free3D (seems to be of poor quality and low quantity).
- BGM for Tower of Magic 1, walking sound from https://sc.chinaz.com/yinxiao/201201513682.htm
