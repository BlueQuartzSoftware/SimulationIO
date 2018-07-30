#pragma once

#include "SimulationIO/SimulationIOPlugin.h"

class SimulationIOGuiPlugin : public SimulationIOPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "com.your.domain.SimulationIOGuiPlugin")

public:
  SimulationIOGuiPlugin();
  ~SimulationIOGuiPlugin() override;

  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;

public:
  SimulationIOGuiPlugin(const SimulationIOGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  SimulationIOGuiPlugin(SimulationIOGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  SimulationIOGuiPlugin& operator=(const SimulationIOGuiPlugin&) = delete; // Copy Assignment Not Implemented
  SimulationIOGuiPlugin& operator=(SimulationIOGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
