# ImportFEAData #


## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** reads data files created by different simulation packages and saves the data in a newly created **Data Container**.

#### Packages ####

##### ABAQUS #####

##### BSAM #####

##### DEFORM #####

## Parameters ##

| Name | Type | Description |
|------|------|------|
| FEA package | Enumeration | Package to be used for reading a file |
| odb Name | String | Name of odb file, if _ABAQUS_ is chosen |
| odb File Path | Path | Path of the directory where the odb file exists, if _ABAQUS_ is chosen |
| Instance Name | String | Name of the instance in upper case, if _ABAQUS_ is chosen |
| Step | String | Step number, if _ABAQUS_ is chosen |
| Frame Number | int | Frame Number, if _ABAQUS_ is chosen |
| Output Variable | String | output variable, if _ABAQUS_ is chosen |
| Element Set | String | element set, if _ABAQUS_ is chosen |
| Input File | Path | Name and address of the input file, if _BSAM_, _DEFORM_, or _DEFORM_POINT_TRACK_is chosen |
| Read Single Time Step| bool | Option to read just a single time step instead of all time steps, if _DEFORM_POINT_TRACK_is chosen |
| Time Step | int | Specify the time step, if _DEFORM_POINT_TRACK_is chosen and data corresponding to only one time step needs to be read in DREAM.3D | 

## Required Geometry ##

Not Applicable

## Required Objects ##

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | DataContainer | N/A | N/A | Created **Data Container** |
| **Attribute Matrix** | VertexData | Vertex | N/A | Created **Vertex Attribute Matrix** name |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** name |
| **Time Series Bundle Name** | TimeSeriesBundle | N/A | N/A | Created **Time Series Bundle** name |

## Example Pipelines ##

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users