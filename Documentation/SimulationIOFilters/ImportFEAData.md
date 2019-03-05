# ImportFEAData #


## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** reads data files created by different simulation packages and saves the data in a newly created **Data Container**. Currently, it can read the output files obtained from ABAQUS, BSAM, and DEFORM.

#### Packages ####

##### ABAQUS #####
ABAQUS saves all the output data in *.odb file. This **filter** reads the *.odb file, writes and runs a python script (odbName.py) to extract the data from *.odb file and saves it in odbtotxt.dat file, and then scans odbtotxt.dat file to save the data in a nely created **Data Container**. odbName.py and odbtotxt.dat files are created in the directory mentioned in **odb File Path** field and are not deleted. This **filter** reads data corresponding to a particular instance, step and frame.

Based on the operating system, the command used for running a python script in ABAQUS should be written in the **ABAQUS Python Command** field. Since the python script is executed in the background, following command should be entered in the case of WINDOWS:

cmd /C abaqus python <odbname.py>

It is assumed that ABAQUS is installed on the machine and "abaqus python *.py" is working on a command window. Currently, element types of C3D8R, C3D4, CPE4R, CPS4R, CPE3, and CPS3 can be read using this **filter**.

##### BSAM #####
The output from BSAM consists of an array of *.dat files, with each file corresponding to a different load step. This **Filter** reads one file at a time and saves the geometry (nodal coordinates and connectivity), nodal stresses and strains, nodal displacements, values of the variable "cluster" at different nodes, and nodal values of the variable "va" (va1, va2, va3, va4) in a newly created **Data Container**. The current implementation is for brick elements with 8 nodes.

##### DEFORM #####
Two different types of output files are produced by DEFORM: (i) single time step (*.DAT) file, and (ii) point tracking output (*.RST) file. The option DEFORM in the drop-down menu of this **filter** is used to read the single time step file whereas the option "DEFORM_POINT_TRACK" is used to read the point tracking output file.

In the case of single time step file, this **filter** reads the quadrilateral mesh data (nodal coordinates and connectivity), and the value of variables such as stress, strain, ndtmp, etc at cells and nodes. 

The point tracking output file consists of data at a set of points for different time steps. The **filter** creates an array of **Data Containers**, with each **Data Container** corresponding to a particular time step. In each **Data Container**, a **Vertex** geometry is used to store the information at different points. User also has the option to read data for just one time step by entering the value of the time step index that needs to be read. For example, if the point tracking file has data for time steps 0, 10 and 20, and the user wants to read in the data for time step number 20, it is required to enter 2 in the "Time step" field in the **filter**. 

## Parameters ##

| Name | Type | Description |
|------|------|------|
| FEA package | Enumeration | Package to be used |
| odb Name | String | Name of odb file, if _ABAQUS_ is chosen |
| odb File Path | Path | Path of the directory where the odb file exists, if _ABAQUS_ is chosen |
| ABAQUS Python Command | String | Command to run python scripts, if _ABAQUS_ is chosen |
| Instance Name | String | Name of the instance in UPPER case, if _ABAQUS_ is chosen |
| Step | String | Step number, if _ABAQUS_ is chosen |
| Frame Number | int | Frame Number, if _ABAQUS_ is chosen |
| Input File | Path | Name and address of the input file, if _BSAM_, _DEFORM_, or _DEFORM_POINT_TRACK_is chosen |
| Read Single Time Step| bool | Option to read just a single time step instead of all the time steps, if _DEFORM_POINT_TRACK_is chosen |
| Time Step | int | Specify the time step index, if _DEFORM_POINT_TRACK_is chosen and data corresponding to only one time step needs to be read in DREAM.3D | 

## Required Geometry ##

Not Applicable

## Required Objects ##

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | DataContainer | N/A | N/A | Created **Data Container** |
| **Attribute Matrix** | VertexData | Vertex | N/A | Created **Vertex Attribute Matrix** name |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** name |
| **Time Series Bundle Name** | TimeSeriesBundle | N/A | N/A | Created **Time Series Bundle** name, if _DEFORM_POINT_TRACK_is chosen |

## Example Pipelines ##

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users