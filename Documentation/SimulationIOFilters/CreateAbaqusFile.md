# CreateAbaqusFile #

## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** writes the input files for ABAQUS.

This **filter** assumes that the finite element discretization is equal to the discretization of the image. Thus, the number of finite elements in each direction are equal to the number of cells. Please try **Export 3D Solid Mesh** filter to make mesh that is different from the distribution of cells in an image.

This **filter** writes out five files for input into the Abaqus analysis tool. The files created are: xxx.inp (the master file), xxx_nodes.inp, xxx_elems.inp, xxx_elset.inp and xxx_sects.inp. 

The material information is written in the master file (xxx.inp). There is a table to enter the values of material constants. However, the total number of material constants that will be written in the *.inp file will be (5 + "Number of Material Constants"). The first five values under *User Material are grainID, phaseID, Euler1, Euler2, and Euler3. The user provided material constants are written after these five values. A format of material data that is written to *.inp file is shown below:

*Material, name = GrainId#_PhaseID#_set
*Depvar
< Number of Solution Dependent Variables>
*User Material, constants = 5 + "Number of Material Constants"
grainID, phaseID, Euler1, Euler2, Euler3, materialConstant1, materialConstant2, materialConstant3
...
*User Output Variables
"Number of User Output Variables"

Currently, this **filter** is valid only for cuboidal geometries and creates brick elements (C3D8/C3D8R) only.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output Path | Path | path of the directory where files will be created |
| Output File Prefix | String | output file prefix |
| Job Name | String | job name |
| Number of Solution Dependent Variables | int | number of solution dependent variables |
| Number of User Output Variables | int | number of usev output variables |
| Material Constants | DynamicTableData | values of material constants |

## Required Geometry ##

 Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | FeatureIds | int32_t | (1) |  Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | Euler Angles | float | (3) | Three angles defining the orientation of the **Feature** |
| **Feature Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Cell** belongs |

## Created Objects ##

## Example Pipelines ##
+pzflexPipeline

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
