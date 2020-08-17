# ImportDelamData #

## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** imports data from a CSDGM and a Bvid stdout text file and creates a rectilinear geometry out of it.

### CSDGM File ###
+ Must have 4 columns (X, Y, Z, and data value) that are all convertable to floats

The 4th term in each line is the data that will be inserted into the rectilinear grid at the voxel that contains the coordinates (1st, 2nd, 3rd terms).

### Bvid StdOut File ###
+ Must have a line where the 2nd term in the line is `0/90ply`, and the last term is convertable to a `float`.
+ Must have a line that starts with `Previous`, and the last term is convertable to a `float`.
+ Must have a line where the 4th term is `CZM` and the last term is convertable to an `integer`.
+ Must have a line where the 4th term is `plies` and the last term is convertable to an `integer`.
+ Must have a line where the 2nd term is `CPT:` and the last term is convertable to a `float`.

These lines are parsed to get the values necessary to calculate the size and bounds of the rectilinear grid geometry.

These requirements were taken directly from a Python script written by John Wertz from the Air Force Research Lab.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| CSDGM File | Input File | The path to the CSDGM file |
| Bvid StdOut File | Input File | The path to the Bvid stdout file |
| Interface Thickness | Float | The interface thickness for the data |

## Required Geometry ##

Not Applicable

## Required Objects ##

Not Applicable

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | Data Container | RectGrid | N/A | The rectilinear grid data container |
| **Attribute Matrix** | Attribute Matrix | Cell | (1) | The Cell attribute matrix to store the data |
| **Element/Feature/Ensemble/etc. Attribute Array** | Data Array | float | (1) | The array that stores the rect grid data |

## Example Pipelines ##

None.

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users