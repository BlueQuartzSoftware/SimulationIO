/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include <QtCore/QFile>
#include <QtCore/QMutex>
#include <QtCore/QProcess>
#include <QtCore/QSharedPointer>
#include <QtCore/QWaitCondition>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class AttributeMatrix;
class DataContainer;
class QProcess;

#include "SimulationIO/SimulationIODLLExport.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOFilters/Utility/DeformDataParser.hpp"

/**
 * @brief The ImportFEAData class. See [Filter documentation](@ref importfeadata) for details.
 */
class SimulationIO_EXPORT ImportFEAData : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ImportFEAData SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ImportFEAData)
  PYB11_FILTER_NEW_MACRO(ImportFEAData)
  PYB11_PROPERTY(int FEAPackage READ getFEAPackage WRITE setFEAPackage)
  PYB11_PROPERTY(QString odbName READ getodbName WRITE setodbName)
  // PYB11_PROPERTY(QString ABQInputFile READ getABQInputFile WRITE setABQInputFile)
  PYB11_PROPERTY(QString odbFilePath READ getodbFilePath WRITE setodbFilePath)
  PYB11_PROPERTY(QString ABQPythonCommand READ getABQPythonCommand WRITE setABQPythonCommand)
  PYB11_PROPERTY(QString InstanceName READ getInstanceName WRITE setInstanceName)
  PYB11_PROPERTY(QString Step READ getStep WRITE setStep)
  PYB11_PROPERTY(int FrameNumber READ getFrameNumber WRITE setFrameNumber)
  //  PYB11_PROPERTY(QString OutputVariable READ getOutputVariable WRITE setOutputVariable)
  //  PYB11_PROPERTY(QString ElementSet READ getElementSet WRITE setElementSet)
  PYB11_PROPERTY(QString BSAMInputFile READ getBSAMInputFile WRITE setBSAMInputFile)
  PYB11_PROPERTY(QString DEFORMInputFile READ getDEFORMInputFile WRITE setDEFORMInputFile)
  PYB11_PROPERTY(QString DEFORMPointTrackInputFile READ getDEFORMPointTrackInputFile WRITE setDEFORMPointTrackInputFile)
  PYB11_PROPERTY(QString TimeSeriesBundleName READ getTimeSeriesBundleName WRITE setTimeSeriesBundleName)
  PYB11_PROPERTY(bool ImportSingleTimeStep READ getImportSingleTimeStep WRITE setImportSingleTimeStep)
  PYB11_PROPERTY(int SingleTimeStepValue READ getSingleTimeStepValue WRITE setSingleTimeStepValue)
  PYB11_PROPERTY(QString SelectedTimeArrayName READ getSelectedTimeArrayName WRITE setSelectedTimeArrayName)
  PYB11_PROPERTY(QString SelectedTimeStepArrayName READ getSelectedTimeStepArrayName WRITE setSelectedTimeStepArrayName)
  PYB11_PROPERTY(QString SelectedPointNumArrayName READ getSelectedPointNumArrayName WRITE setSelectedPointNumArrayName)
  PYB11_PROPERTY(QString SelectedXCoordArrayName READ getSelectedXCoordArrayName WRITE setSelectedXCoordArrayName)
  PYB11_PROPERTY(QString SelectedYCoordArrayName READ getSelectedYCoordArrayName WRITE setSelectedYCoordArrayName)
  PYB11_PROPERTY(QStringList DataArrayList READ getDataArrayList WRITE setDataArrayList)
  PYB11_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ImportFEAData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<ImportFEAData> New();

  /**
   * @brief Returns the name of the class for ImportFEAData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportFEAData
   */
  static QString ClassName();

  ~ImportFEAData() override;

  /**
   * @brief Setter property for FEAPackage
   */
  void setFEAPackage(int value);
  /**
   * @brief Getter property for FEAPackage
   * @return Value of FEAPackage
   */
  int getFEAPackage() const;
  Q_PROPERTY(int FEAPackage READ getFEAPackage WRITE setFEAPackage)

  /**
   * @brief Setter property for odbName
   */
  void setodbName(const QString& value);
  /**
   * @brief Getter property for odbName
   * @return Value of odbName
   */
  QString getodbName() const;
  Q_PROPERTY(QString odbName READ getodbName WRITE setodbName)

  /**
   * @brief Setter property for odbFilePath
   */
  void setodbFilePath(const QString& value);
  /**
   * @brief Getter property for odbFilePath
   * @return Value of odbFilePath
   */
  QString getodbFilePath() const;
  Q_PROPERTY(QString odbFilePath READ getodbFilePath WRITE setodbFilePath)

  /**
   * @brief Setter property for ABQPythonCommand
   */
  void setABQPythonCommand(const QString& value);
  /**
   * @brief Getter property for ABQPythonCommand
   * @return Value of ABQPythonCommand
   */
  QString getABQPythonCommand() const;
  Q_PROPERTY(QString ABQPythonCommand READ getABQPythonCommand WRITE setABQPythonCommand)

  /**
   * @brief Setter property for InstanceName
   */
  void setInstanceName(const QString& value);
  /**
   * @brief Getter property for InstanceName
   * @return Value of InstanceName
   */
  QString getInstanceName() const;
  Q_PROPERTY(QString InstanceName READ getInstanceName WRITE setInstanceName)

  /**
   * @brief Setter property for Step
   */
  void setStep(const QString& value);
  /**
   * @brief Getter property for Step
   * @return Value of Step
   */
  QString getStep() const;
  Q_PROPERTY(QString Step READ getStep WRITE setStep)

  /**
   * @brief Setter property for FrameNumber
   */
  void setFrameNumber(int value);
  /**
   * @brief Getter property for FrameNumber
   * @return Value of FrameNumber
   */
  int getFrameNumber() const;
  Q_PROPERTY(int FrameNumber READ getFrameNumber WRITE setFrameNumber)

  /**
   * @brief Setter property for DEFORMInputFile
   */
  void setDEFORMInputFile(const QString& value);
  /**
   * @brief Getter property for DEFORMInputFile
   * @return Value of DEFORMInputFile
   */
  QString getDEFORMInputFile() const;
  Q_PROPERTY(QString DEFORMInputFile READ getDEFORMInputFile WRITE setDEFORMInputFile)

  /**
   * @brief Setter property for BSAMInputFile
   */
  void setBSAMInputFile(const QString& value);
  /**
   * @brief Getter property for BSAMInputFile
   * @return Value of BSAMInputFile
   */
  QString getBSAMInputFile() const;
  Q_PROPERTY(QString BSAMInputFile READ getBSAMInputFile WRITE setBSAMInputFile)

  /**
   * @brief Setter property for DEFORMPointTrackInputFile
   */
  void setDEFORMPointTrackInputFile(const QString& value);
  /**
   * @brief Getter property for DEFORMPointTrackInputFile
   * @return Value of DEFORMPointTrackInputFile
   */
  QString getDEFORMPointTrackInputFile() const;
  Q_PROPERTY(QString DEFORMPointTrackInputFile READ getDEFORMPointTrackInputFile WRITE setDEFORMPointTrackInputFile)

  /**
   * @brief Setter property for TimeSeriesBundleName
   */
  void setTimeSeriesBundleName(const QString& value);
  /**
   * @brief Getter property for TimeSeriesBundleName
   * @return Value of TimeSeriesBundleName
   */
  QString getTimeSeriesBundleName() const;
  Q_PROPERTY(QString TimeSeriesBundleName READ getTimeSeriesBundleName WRITE setTimeSeriesBundleName)

  /**
   * @brief Setter property for ImportSingleTimeStep
   */
  void setImportSingleTimeStep(bool value);
  /**
   * @brief Getter property for ImportSingleTimeStep
   * @return Value of ImportSingleTimeStep
   */
  bool getImportSingleTimeStep() const;
  Q_PROPERTY(bool ImportSingleTimeStep READ getImportSingleTimeStep WRITE setImportSingleTimeStep)

  /**
   * @brief Setter property for SingleTimeStepValue
   */
  void setSingleTimeStepValue(int value);
  /**
   * @brief Getter property for SingleTimeStepValue
   * @return Value of SingleTimeStepValue
   */
  int getSingleTimeStepValue() const;
  Q_PROPERTY(int SingleTimeStepValue READ getSingleTimeStepValue WRITE setSingleTimeStepValue)

  /**
   * @brief Setter property for SelectedTimeArrayName
   */
  void setSelectedTimeArrayName(const QString& value);
  /**
   * @brief Getter property for SelectedTimeArrayName
   * @return Value of SelectedTimeArrayName
   */
  QString getSelectedTimeArrayName() const;

  // Q_PROPERTY(QString SelectedTimeArrayName READ getSelectedTimeArrayName WRITE setSelectedTimeArrayName)

  /**
   * @brief Setter property for SelectedTimeStepArrayName
   */
  void setSelectedTimeStepArrayName(const QString& value);
  /**
   * @brief Getter property for SelectedTimeStepArrayName
   * @return Value of SelectedTimeStepArrayName
   */
  QString getSelectedTimeStepArrayName() const;

  // Q_PROPERTY(QString SelectedTimeStepArrayName READ getSelectedTimeStepArrayName WRITE setSelectedTimeStepArrayName)

  /**
   * @brief Setter property for SelectedPointNumArrayName
   */
  void setSelectedPointNumArrayName(const QString& value);
  /**
   * @brief Getter property for SelectedPointNumArrayName
   * @return Value of SelectedPointNumArrayName
   */
  QString getSelectedPointNumArrayName() const;

  // Q_PROPERTY(QString SelectedPointNumArrayName READ getSelectedPointNumArrayName WRITE setSelectedPointNumArrayName)

  /**
   * @brief Setter property for SelectedXCoordArrayName
   */
  void setSelectedXCoordArrayName(const QString& value);
  /**
   * @brief Getter property for SelectedXCoordArrayName
   * @return Value of SelectedXCoordArrayName
   */
  QString getSelectedXCoordArrayName() const;

  // Q_PROPERTY(QString SelectedXCoordArrayName READ getSelectedXCoordArrayName WRITE setSelectedXCoordArrayName)

  /**
   * @brief Setter property for SelectedYCoordArrayName
   */
  void setSelectedYCoordArrayName(const QString& value);
  /**
   * @brief Getter property for SelectedYCoordArrayName
   * @return Value of SelectedYCoordArrayName
   */
  QString getSelectedYCoordArrayName() const;

  // Q_PROPERTY(QString SelectedYCoordArrayName READ getSelectedYCoordArrayName WRITE setSelectedYCoordArrayName)

  /**
   * @brief Setter property for DataArrayList
   */
  void setDataArrayList(const QStringList& value);
  /**
   * @brief Getter property for DataArrayList
   * @return Value of DataArrayList
   */
  QStringList getDataArrayList() const;
  Q_PROPERTY(QStringList DataArrayList READ getDataArrayList WRITE setDataArrayList)

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const QString& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  QString getDataContainerName() const;
  Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

  /**
   * @brief Setter property for VertexAttributeMatrixName
   */
  void setVertexAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName
   * @return Value of VertexAttributeMatrixName
   */
  QString getVertexAttributeMatrixName() const;
  Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  ImportFEAData();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  void readHeader(QFile& reader);
  void parseDataBlock(QVector<QByteArray>& block);
  QVector<QByteArray> splitDataBlock(QVector<QByteArray>& dataBlock);
  void readTimeStep(QFile& reader, qint32 t);
  QVector<QByteArray> tokenizeNodeBlock(QFile& reader);
  void parseDataTokens(QVector<QByteArray>& tokens, qint32 nodeIdx);

protected slots:
  void processHasFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void processHasErroredOut(QProcess::ProcessError error);
  void sendErrorOutput();
  void sendStandardOutput();

private:
  int m_FEAPackage = {};
  QString m_odbName = {};
  QString m_odbFilePath = {};
  QString m_ABQPythonCommand = {};
  QString m_InstanceName = {};
  QString m_Step = {};
  int m_FrameNumber = {};
  QString m_DEFORMInputFile = {};
  QString m_BSAMInputFile = {};
  QString m_DEFORMPointTrackInputFile = {};
  QString m_TimeSeriesBundleName = {};
  bool m_ImportSingleTimeStep = {};
  int m_SingleTimeStepValue = {};
  QString m_SelectedTimeArrayName = {};
  QString m_SelectedTimeStepArrayName = {};
  QString m_SelectedPointNumArrayName = {};
  QString m_SelectedXCoordArrayName = {};
  QString m_SelectedYCoordArrayName = {};
  QStringList m_DataArrayList = {};
  QString m_DataContainerName = {};
  QString m_VertexAttributeMatrixName = {};
  QString m_CellAttributeMatrixName = {};

  /**
   * @brief writeABQpyscr
   * @param file
   * @param odbName
   * @param odbFilePath
   * @param instanceName
   * @param step
   * @param frameNum
   * @param outputVar
   * @param elSet
   * @return
   */
  int32_t writeABQpyscr(const QString& file, const QString& odbName, const QString& odbFilePath, const QString& instanceName, const QString& step, int frameNum);

  void runABQpyscr(const QString& file);

  void scanABQFile(const QString& file, DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix);

  void scanDEFORMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix);

  void scanBSAMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix);

  QWaitCondition m_WaitCondition;
  QMutex m_Mutex;
  bool m_Pause = false;
  QSharedPointer<QProcess> m_ProcessPtr;
  //  QStringList arguments;

  QStringList splitArgumentsString(QString arguments);

  QString m_CachedFileName;
  QFile m_InStream;
  QMap<QString, QString> m_DataTypes;

  QMap<QString, SimulationIO::DeformDataParser::Pointer> m_NamePointerMap;
  qint32 m_NumBlocks = 0;
  qint32 m_NumPoints = 0;
  qint32 m_NumTimeSteps = 0;
  qint32 m_LinesPerBlock = 0;
  bool m_HeaderIsComplete = false;
  int m_selectedTimeStepValue = 0;
  bool m_selectedTimeStep = false;

  QString m_BundleMetaDataAMName;

public:
  ImportFEAData(const ImportFEAData&) = delete;            // Copy Constructor Not Implemented
  ImportFEAData& operator=(const ImportFEAData&) = delete; // Copy Assignment Not Implemented
  ImportFEAData(ImportFEAData&&) = delete;                 // Move Constructor Not Implemented
  ImportFEAData& operator=(ImportFEAData&&) = delete;      // Move Assignment Not Implemented
};
