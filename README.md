## CS 330 Final Project Reflection

### Software Design Approach

When approaching software design, I begin by identifying the core objective and breaking the problem into manageable components. For this 3D scene project, I first analyzed the reference image and determined which real-world objects could be recreated using basic geometric primitives. I focused on simplifying complex objects into combinations of planes, boxes, cylinders, and cones while maintaining visual accuracy. This approach helped me prioritize structure before aesthetics.

Throughout this project, I strengthened my ability to think spatially in three dimensions. I learned how to visualize object placement relative to the X, Y, and Z axes and how affine transformations (translation, rotation, and scaling) interact when building composite objects like the laptop and lamp. I also developed a better understanding of scene composition and how lighting influences depth perception and realism.

My design process followed an iterative workflow. I began by creating base shapes, then gradually refined placement, added textures, incorporated lighting, and finally implemented camera controls. Each milestone allowed me to refine earlier decisions based on testing and instructor feedback. This layered development process helped prevent overwhelming complexity.

The tactics I used—modularizing object creation, isolating transformations per object, and refining incrementally—can be applied to future projects in graphics, simulation, or even full-stack development. Breaking problems into reusable components improves maintainability and scalability.



### Program Development Approach

When developing programs, I prioritize functionality first, then refinement. For this project, I ensured that object creation, camera navigation, and lighting systems worked independently before integrating them into a cohesive scene.

A key development strategy I used was isolating model matrices for each object to prevent unintended transformation stacking. Creating helper functions for generating transformation matrices made the code more readable and modular. Implementing camera controls required careful attention to vector math, particularly with yaw, pitch, and normalized direction vectors.

Iteration played a significant role in development. Many adjustments were made to object positioning, rotation pivots, lighting intensity, and camera responsiveness. Through repeated testing, I refined object placement to better match the reference image and improved navigation smoothness.

Over the course of the milestones, my development process became more structured. Early stages focused heavily on experimentation, while later stages emphasized organization, readability, and maintainability. I became more confident in debugging transformation issues and understanding how projection matrices affect scene perception.


### Computer Science and My Future Goals

Computer science equips me with problem-solving skills that extend beyond coding. This project strengthened my analytical thinking, spatial reasoning, and understanding of graphical systems—skills that are transferable to many technical domains.

Computational graphics and visualization expand my technical foundation in areas such as simulation, interactive systems, game development, augmented reality, and user interface design. Understanding how to construct and render 3D environments deepens my comprehension of real-time systems and performance considerations.

From an educational standpoint, this experience prepares me for more advanced coursework involving graphics pipelines, GPU programming, and interactive system design. Professionally, these skills could be applied in industries such as game development, simulation modeling, data visualization, virtual prototyping, and digital product design.

This project demonstrates not only my ability to create a functional 3D environment using C++ and OpenGL, but also my growth in structured development, iterative refinement, and modular design principles.
