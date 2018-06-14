# URE (Unnamed Rendering Engine)
An OpenGL / Vulkan rendering engine which is nowhere near complete (for now at least).

# Building
Install Vulkan (preferably 1.0.65.1 or higher).  
Build using cmake.  
Assimp (the engine uses 4.0.1) must be integrated manually due to its abnormal cmake set-up.  This will be added to the build hierarchy soon (hopefully) but until then get it from https://github.com/assimp/assimp/releases, build it, and include it in the project.

# Controls
Right Arrow = Look Right  
Left Arrow = Look Left  
Up Arrow = Look Down  
Down Arrow = Look Up  
Right Ctrl / Numpad 0 = Roll Camera  
W = Move Camera Forward  
S = Move Camera Backward  
A = Move Camera Left  
D = Move Camera Right  
Q = Move Camera Down  
E = Move Camera Up  

1 = Toggle Lights  
Enter = Toggle Pause  
SpaceBar = Switch Renderer  