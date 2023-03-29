
Real-Time GLSL-Based PathTracing
==========
This is a real-time ray tracing project with GPU-denoise. The general framework of the project comes from knightcrawler25/GLSL-PathTracer (https://github.com/knightcrawler25/GLSL-PathTracer) 

It's about 45 FPS when running on 3060Ti and I5-12600KF.

Features
--------
- Only 1 spp for each pixel
- Spatial filtering combining with depth, color and normal
- Temporal filtering
- Outlier remove

More Features
--------
- Two-level BVH for instancing
- SAH BVH
- Disney BSDF

Steps
--------
- 1 spp path tracing
  ![Jinx](./screenshots/original.png)
- Get GBuffer
  
  normal:
  ![Jinx](./screenshots/GBufferNormal.png)
  depth:
  ![Jinx](./screenshots/GBufferDepth.png)
  position:
  ![Jinx](./screenshots/GBufferPosition.png)
- Spatial filtering
  ![Jinx](./screenshots/SpatialFiltering.png)
- Motion Vector
  ![Jinx](./screenshots/MotionVector1.png)
  ![Jinx](./screenshots/MotionVector2.png)
- Temperal filtering
  ![Jinx](./screenshots/TemporalFiltering.png)
  Afterimages occur during fast motion
  ![Jinx](./screenshots/TemporalClamp.png)
- Outlier remove
  ![Jinx](./screenshots/OutlierRemove.png)
  Works well on simple models, but blurry on complex models.

TODO
--------
- Add svgf
- Complete the GBuffer when no hit to support IBL
- Use a more efficient filter kernel