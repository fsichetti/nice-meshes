# Nice Meshes
*Nicemesh* is a progam that can generate triangle meshes with very regular geometry, to be used as ground truth when testing geometry processing algorithms.

# Building
Clone the repo, move to the root folder and run `make`.
Dependencies:
- [Epoxy](https://github.com/anholt/libepoxy)
- [FreeGlut](https://freeglut.sourceforge.net/)
- [FreeImage](https://freeimage.sourceforge.io/)
*TODO: move to CMake*

# Running
Program behaviour is specified in a standard INI file. The INI file can contain any number of sections, each corresponding to a different configuration. The program takes two optional arguments: the name of the configuration (INI section) to use, and the path to the INI file itself, which defaults to `./configuration.ini`. Key-value pairs specified before any section are applied to all configurations.
The first argument is the name of the INI file. If more arguments are specified, the rest are configurations from that file that are run in order; otherwise, all configurations from the INI file are executed in order. The `-p` options tells the program to run in parallel, which disables interactivity.

## Configuration parameters
All parameters are case-insensitive.

### Generation
- **shape**: Must be one of *sphere*, *torus*, *catenoid*, *bezier*. Sets the type of shape to be generated and the parameters that are used. Defaults to *torus*.
- **name**: Sets the name of the mesh, which is used when saving the mesh in any format. Defaults to "mesh".
- **anisotropy**: If unspecified, samples the mesh regularly. Otherwise perform an irregular sampling with the specified anisotropy. Use anisotropy 1 to get an isotropic sampling. WIP
- **samples**: Determines the number of samples in one of the natural coordinates. If the shape is a surface of revolution (torus or catenoid), then it is the number of samples in the direction of rotation; the number of samples in the other direction is determined automatically in order to obtain the nice mesh. If the shape is a Bézier patch, it is the number of samples in any of the two directions (unless the **sampling** parameter is specified). Defaults to 64.
- **subdivision**: For the sphere, it is the number of times an icosahedron is subdivided to generate the sphere. Defaults to 3.
- **radius**: For the sphere, it is the radius of the sphere. For a randomly generated Bézier patch, it is the radius of the sphere on which the 4 corner points lie. Defaults to 1.
- **innerRadius**: For the torus, the first is the radius of the circle that revolves around the axis. For the catenoid, it is the radius of the circular section at the middle point, where the catenoid is most narrow. For Bézier patches, it is ignored. Defaults to 1.
- **outerRadius**: For the torus, it is the distance of the revolved circle from the axis of revolution. For the catenoid, it is the radius of the ends of the shape. Defaults to 2.
- **borderVariance**: Only relevant to random Bézier patches. How far the border vertices can deviate from their starting position on the line connecting two corner vertices.
- **innerVariance**: Only relevant to random Bézier patches. How far the inner vertices can deviate from their starting position.

### Processing
- **centered**: If "true", the mesh is centered at the origin after it is generated. This is only useful for random Bézier patches. Defaults to "false".
- **noise**: Sets the variance of gaussian noise relative to the average edge length of the mesh. Defaults to 0.
- **noiseType**: Sets the type of noise. Defaults to "3d". Possible values:
    - *3d*: Each vertex is displaced in 3D space.
    - *normal*: Each vertex is displaced along its normal direction.
    - *tangential*: Each vertex is displaced along a direction on its tangent plane.

### Additional data
- **scalarField**: If "true", a scalar field defined on vertices is written to a file named `<name>Scalar.txt`.
- **scalarFrequency**: If set to a value, multiplies the frequency of the generated scalar field by that number. Defaults to 1.
- **scalarAmplitude**: Scales the scalar field. Defaults to 1.
- **scalarHeader**: If "true", adds a header line to the scalar field file containing the string "`SCALAR_FIELD`" and the number of vertices.
- **scalarLaplacian**/**scalarGradient**/**scalarHessian**: If "true", also compute the laplacian/gradient/hessian of the scalar field and write it to `<name>Laplacian.txt`/`<name>Gradient.txt`/`<name>Hessian.txt`.
- **scalarFaceGradient**: If "true", compute the gradient of the scalar field on face centroids and write it to `<name>FaceGradient.txt`. Useful for comparing methods that estimate the gradient in triangle faces.

### Behaviour
- **interactive** If "true", displays the generated mesh in the interactive viewer, where it can be exported to any format via keyboard shortcuts. Defaults to "true".
- **repeat**: Used for batch generation of random surfaces. Controls how many times the generation is executed. The names of the resulting meshes are obtained by appending a number to the base name specified in the **name** field. If **interactive** is on, **repeat** is ignored. Defaults to 1.
- **savePLY**/**saveOBJ**/**saveOFF**: If "true", exports the mesh with the requested format. Note that the number of vertex attributes included in the file may vary. The PLY file format is guaranteed to include all attributes. All default to "false".
- **exportUV**: If "true", exports the uv coordinates of each vertex in a .txt file with two columns.
- **exportControlGrid**: If "separate", exports the Bézier patch's control grid coordinates in 3 separate txt files, each containing a 4x4 matrix.
- **inputShape**: Instead of generating the mesh from scratch, read it from the specified OBJ file and reproject it to compute differential quantities exactly. If **shape** is specified, the read data will be interpreted as that shape for the purpose of computing normals, parametric coordinates, curvature, etcetera. For spheres, tori and catenoids, whatever coordinates are received are projected on the corresponding shape with the specified radii.
- **inputPlane**: For tori, catenoids, and Bézier patches, the program expects a path to a meshed \[0,1\] XY plane, which is used to create the topology of the new mesh. The Z coordinate, if present, is ignored. Vertices on the border should match where the mesh wraps around.
- **outFolder**: Path to the folder where the exported meshes should be saved. The folder must exist. Defaults to the current folder.
- **seed**: Sets the seed for the random number generator. Defaults to empty, which tells the program to generate a seed from system time. Only relevant to random Bézier patches, since the other options do not use RNG.

## Keyboard shortcuts
todo