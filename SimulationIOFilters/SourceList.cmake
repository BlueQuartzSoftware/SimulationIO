#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName SimulationIOFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

#--------
# This macro must come first before we start adding any filters
SIMPL_START_FILTER_GROUP(
  ALL_FILTERS_HEADERFILE ${AllFiltersHeaderFile}
  REGISTER_KNOWN_FILTERS_FILE ${RegisterKnownFiltersFile}
  FILTER_GROUP "${_filterGroupName}"
  BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}
  )

#---------
# List your public filters here
set(_PublicFilters
  Export3dSolidMesh
  ImportFEAData
  ImportDeformKeyFilev12Filter
  ExportLAMMPSFile
  ExportDAMASKFiles
  ExportOnScaleTableFile
  CreateAbaqusFile
  CreateBsamFile
  ExportMultiOnScaleTableFile
  ImportOnScaleTableFile
  ImportDelamData
)

list(LENGTH _PublicFilters PluginNumFilters)
set_property(GLOBAL PROPERTY PluginNumFilters ${PluginNumFilters})

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_SIMPL_FILTER(  "${PLUGIN_NAME}" "${PLUGIN_NAME}"
                        ${_filterGroupName} ${f}
                        ${${PLUGIN_NAME}_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md 
                        TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters

)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_SIMPL_FILTER(  "${PLUGIN_NAME}" "${PLUGIN_NAME}"
                        ${_filterGroupName} ${f}
                        ${${PLUGIN_NAME}_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md FALSE)
endforeach()

#---------
# List your filter algorithms here
set(_Algorithms
ImportDeformKeyFilev12
)

#-------------
# These are files that need to be compiled but are NOT filters
foreach(f ${_Algorithms} )
  ADD_SIMPL_SUPPORT_HEADER_SUBDIR(${${PLUGIN_NAME}_SOURCE_DIR} ${_filterGroupName} ${f}.h Algorithms)
  ADD_SIMPL_SUPPORT_HEADER_SUBDIR(${${PLUGIN_NAME}_SOURCE_DIR} ${_filterGroupName} ${f}.cpp Algorithms)
endforeach()


#---------------------
# This macro must come last after we are done adding all the filters and support files.
SIMPL_END_FILTER_GROUP(${${PLUGIN_NAME}_BINARY_DIR} "${_filterGroupName}" "${PLUGIN_NAME}")

