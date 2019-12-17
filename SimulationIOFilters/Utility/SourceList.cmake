set(${PLUGIN_NAME}_UTILITY_HDRS
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/AbaqusFileWriter.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/BsamFileWriter.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/OnScaleTableFileWriter.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/EntriesHelper.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/DeformDataParser.hpp
)

set(${PLUGIN_NAME}_UTILITY_SRCS
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/AbaqusFileWriter.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/BsamFileWriter.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/OnScaleTableFileWriter.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/Utility/EntriesHelper.cpp
)

cmp_IDE_SOURCE_PROPERTIES("${PLUGIN_NAME}Filters/Utility" "${${PLUGIN_NAME}_UTILITY_HDRS}" "${${PLUGIN_NAME}_UTILITY_SRCS}" "0")
