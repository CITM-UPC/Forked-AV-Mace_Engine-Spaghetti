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

# Additional functionalities
The Assets panel tab inside the Bottom Panel can switch between the Console tab and the Assets tab

In the Assets tab, there is detailed debug information on the process of Resource importing and loading  <br/><br/>

