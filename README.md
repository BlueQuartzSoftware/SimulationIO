# Linking DREAM.3D with Materials Response Simulation Codes and Meshing Packages

SimulationIO plugin links DREAM.3D with different materials response simulation and meshing packages to allow seamless flow of data between these external codes and DREAM.3D. The objective is to develop an integrated computational materials engineering tool for quantitatively analyzing the influence of microstructure on material properties. Such a tool will accelerate the design and development cycle of new materials with desired properties. 

Five different filters are available in this plugin:

1. Create FEA Input Files: This Filter writes the input files for different simulation packages. Currently supported packages are ABAQUS, PZFLEX and BSAM.

2. Export 3d Solid Mesh: This Filter can be used to create a volume mesh of the sample using three different packages: TetGen (ver 1.5.1; http://wias-berlin.de/software/index.jsp?id=TetGen&lang=1), Netgen (https://ngsolve.org/), and Gmsh (ver 4.1.5; http://gmsh.info/).

3. Export DAMASK Files: This Filter writes the geometry file (*.geom) and material.config file required as an input for the DAMASK package. 

4. Export LAMMPS Data File: This Filter is used to create an atomistic representation of microstructure from image data. The data file produced by this filter can be used for initializing the atomic coordinates in LAMMPS package.

5. Import FEA Data: This Filter reads data files created by different simulation packages and saves the data in a newly created Data Container. Currently, it can read the output files obtained from ABAQUS, BSAM, and DEFORM.