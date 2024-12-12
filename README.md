# Spaghetti Engine
It's the Second Stage of the Assignment for the GameEngine, it's a further completion compared to the previous iteration. 

The engine is using SDL2, glew, assimp, imgui, glm and devil, made by the Group 6 for the Motors subject at CITM

# Trello link
https://trello.com/invite/b/671f5539e9e45eef32369bab/ATTIb1663741f6c55a31d3413ee318b1b78f85079D03/rollin-spaguetti

# Link to the repository

[![Github](https://static.vecteezy.com/system/resources/thumbnails/016/833/880/small_2x/github-logo-git-hub-icon-with-text-on-white-background-free-vector.jpg)](https://github.com/CITM-UPC/Forked-AV-Mace_Engine-Spaghetti)


# Team members

- Yiwei Ye - [Link](https://github.com/Yeeway69)
- Pablo Longaron - [Link](https://github.com/PabloL2003)
- Andrea Doña - [Link](https://github.com/poderoculto5)


# Controls

- WASD - Move Camera
- Shift - Accelerate movement
- Mouse Wheel - Zoom in and out
- Right Click - Rotate Camera

# Additional comments
## ResourceManagent
1. The ResourceManager system is properly implemented as indicated during the class. Any asset files dropped inside the Assets folder will be automatically imported and converted to our Engine Custom files (MYMesh, MyTEX), and stored inside the Library folder. <br/><br/>
2. The engine loads from the custom file and not the original files. <br/><br/>
3. Files can be deleted from the Assets panel and it will reflect on the original file, the metafile, and the custom file inside the Library folder.  <br/><br/>

## Scene Hierarchy 
1. The Scene Hierarchy is working properly, the only minor issue remaining is that the GameObject can not be completely unparented (parent to _root). <br/><br/>
2. The Hierarchy has the correct effect on the GameObject transform (position, rotation, scale) when parented, however, the other related issue is that the bounding box is not behaving as expected when being a child, but the Mesh and GameObject behave correctly. <br/><br/>

## Camera Component
1. The Camera is now a component Class added to GameObejcts. <br/><br/>
2. Has a Basic setting available inside the component setting. <br/><br/>
3. The Editor Camera now showing in the hierarchy to demonstrate the camera component better.  <br/><br/>

## Frustrum Culling
The current frustrum culling works as follows:
For some reason the camera that performs the frustrum culling is not the main camera we use for our Engine, but an additional one that is created in the centre of the scene. 
So if you select an object and move it beyond the coordinates -5 in X and -5 in Z, you can see that the mesh is not rendered implying that it is the area outside the frustrum of the additional camera (the one that performs the frustrum culling). <br/><br/>

## Loading Street
When loading the FBX of the complete street, we encountered several setbacks, mainly that the whole street was rotated by 90º, then we had difficulties when implementing the TGA textures, in the end we were able to solve these problems except for placing the houses in their correct position. <br/><br/>

## Bounding Boxes
After several days working on the code, we were able to implement what was a simple bounding box that covered the baker house. Then we got each child to have its own bounding box implemented in the street. And finally we added rotation and translation to each individual object.
We worked on being able to use the parent as a reference to move all the objects with their bounding boxes but in the end it was not possible to implement it. 
After all this, we had to update the use of the bounding boxes to transform them into AABB, to be able to use them in the frustrum culling in a correct way. <br/><br/>

## Mouse Picking 
In this section, we have tried to correctly implement the mouse picking function. It worked in such a way that we first obtained the dimensions of the screen. Then we detected where the mouse was between those dimensions, and finally we projected a Raycast from the mouse to the object in order to select the bounding box of that object. This has been worked on and implemented over a couple of days. In the end we think the implementation is correct, but we can't see the selection in the scene. <br/><br/>

# Additional functionalities
The Assets panel tab inside the Bottom Panel can switch between the Console tab and the Assets tab

In the Assets tab, there is detailed debug information on the process of Resource importing and loading  <br/><br/>

