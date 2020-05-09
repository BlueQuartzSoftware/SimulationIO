/*
 * Your License or Copyright can go here
 */

#include "ExportDAMASKFiles.h"

#include <QtCore/QTextStream>
#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportDAMASKFiles::ExportDAMASKFiles()
: m_DataFormat(0)
, m_OutputPath("")
, m_GeometryFileName("")
, m_HomogenizationIndex(1)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CellEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerAngles)
, m_CompressGeomFile(true)

{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportDAMASKFiles::~ExportDAMASKFiles() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Data Format");
    parameter->setPropertyName("DataFormat");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ExportDAMASKFiles, this, DataFormat));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ExportDAMASKFiles, this, DataFormat));
    QVector<QString> choices;
    choices.push_back("pointwise");
    choices.push_back("grainwise");
    parameter->setChoices(choices);
    QStringList linkedProps = {"CompressGeomFile"};
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, ExportDAMASKFiles, "*", "*"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Geometry File Name", GeometryFileName, FilterParameter::Parameter, ExportDAMASKFiles));

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Homogenization Index", HomogenizationIndex, FilterParameter::Parameter, ExportDAMASKFiles));

  parameters.push_back(SIMPL_NEW_BOOL_FP("Compress Geom File", CompressGeomFile, FilterParameter::Parameter, ExportDAMASKFiles, 0));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ExportDAMASKFiles, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, ExportDAMASKFiles, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, ExportDAMASKFiles, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  setDataFormat(reader->readValue("DataFormat", getDataFormat()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setGeometryFileName(reader->readString("GeometryFileName", getGeometryFileName()));
  setHomogenizationIndex(reader->readValue("HomogenizationIndex", getHomogenizationIndex()));
  setCompressGeomFile(reader->readValue("CompressGeomFile", getCompressGeomFile()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  std::vector<size_t> cDims(1, 1);

  m_FeatureIdsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                           /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getFeatureIdsArrayPath());
  }

  m_CellPhasesPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())                                                                           /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  cDims[0] = 3;
  m_CellEulerAnglesPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getCellEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getCellEulerAnglesArrayPath());
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::execute()
{
  initialize();
  dataCheck();

  // Check Output Path
  QDir dir;
  if(!dir.mkpath(m_OutputPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(m_OutputPath);
    setErrorCondition(-1, ss);
    return;
  }
  //
  //
  QString geomFile = m_OutputPath + QDir::separator() + m_GeometryFileName + ".geom";
  QString matFile = m_OutputPath + QDir::separator() + "material.config";
  FILE* geomf = fopen(geomFile.toLatin1().data(), "wb");
  FILE* matf = fopen(matFile.toLatin1().data(), "wb");
  //
  //
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  SizeVec3Type dims = m->getGeometryAs<ImageGeom>()->getDimensions();
  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();
  FloatVec3Type origin = m->getGeometryAs<ImageGeom>()->getOrigin();
  float size[3] = {0.0f, 0.0f, 0.0f};

  for(int32_t i = 0; i < 3; i++)
  {
    size[i] = dims[i] * spacing[i];
  }

  int32_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();

  // find total number of features
  int32_t maxGrainId = 0;
  for(int32_t i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > maxGrainId)
    {
      maxGrainId = m_FeatureIds[i];
    }
  }
  //
  //
  fprintf(geomf, "6       header\n");
  fprintf(geomf, "# Generated by DREAM.3D\n");
  fprintf(geomf, "grid    a %zu    b %zu    c %zu\n", dims[0], dims[1], dims[2]);
  fprintf(geomf, "size    x %.3f    y %.3f    z %.3f\n", size[0], size[1], size[2]);
  fprintf(geomf, "origin    x %.3f    y %.3f    z %.3f\n", origin[0], origin[1], origin[2]);
  fprintf(geomf, "homogenization  %d\n", m_HomogenizationIndex);

  switch(m_DataFormat)
  {
  case 0: // pointwise
  {
    fprintf(geomf, "microstructures %d\n", totalPoints);

    if(m_CompressGeomFile)
    {
      fprintf(geomf, "1 to %10d", totalPoints);
    }
    else
    {
      int32_t entriesPerLine = 0;
      for(int32_t i = 1; i <= totalPoints; i++)
      {
        if(entriesPerLine != 0)
        {
          if((entriesPerLine % 10) != 0) // 10 per line
          {
            fprintf(geomf, " ");
          }
          else
          {
            fprintf(geomf, "\n");
            entriesPerLine = 0;
          }
        }
        fprintf(geomf, "%10d", i);
        entriesPerLine++;
      }
    }
    break;
  }
  case 1: // grainwise
  {
    fprintf(geomf, "microstructures %d\n", maxGrainId);

    int32_t entriesPerLine = 0;
    for(int32_t i = 0; i < totalPoints; i++)
    {
      if(entriesPerLine != 0)
      {
        if((entriesPerLine % 10) != 0) // 10 per line
        {
          fprintf(geomf, " ");
        }
        else
        {
          fprintf(geomf, "\n");
          entriesPerLine = 0;
        }
      }
      fprintf(geomf, "%10d", m_FeatureIds[i]);
      entriesPerLine++;
      //
    }
    break;
  }
  }

  //
  //
  fprintf(matf, "#############################################################################\n");
  fprintf(matf, "# Generated by DREAM.3D\n");
  fprintf(matf, "#############################################################################\n");
  fprintf(matf, "# Add <homogenization>, <crystallite>, and <phase> for a complete definition\n");
  fprintf(matf, "#############################################################################\n");
  //
  //
  switch(m_DataFormat)
  {
  case 0: // pointwise
  {

    fprintf(matf, "<texture>\n");
    for(int32_t i = 0; i < totalPoints; i++)
    {
      fprintf(matf, "[point%d]\n", i + 1);
      fprintf(matf, "(gauss) phi1 %.3f   Phi %.3f    phi2 %.3f   scatter 0.0   fraction 1.0 \n", m_CellEulerAngles[i * 3] * 180.0 * SIMPLib::Constants::k_1OverPi,
              m_CellEulerAngles[i * 3 + 1] * 180.0 * SIMPLib::Constants::k_1OverPi, m_CellEulerAngles[i * 3 + 2] * 180.0 * SIMPLib::Constants::k_1OverPi);
    }

    fprintf(matf, "<microstructure>\n");
    for(int32_t i = 0; i < totalPoints; i++)
    {
      fprintf(matf, "[point%d]\n", i + 1);
      fprintf(matf, "crystallite 1\n");
      fprintf(matf, "(constituent)   phase %d texture %d fraction 1.0\n", m_CellPhases[i], i + 1);
    }
    //
    //
    break;
  }
  case 1: // grainwise
  {
    //
    Int32ArrayType::Pointer m_phaseIdLengthPtr = Int32ArrayType::CreateArray(maxGrainId, "PHASEID_INTERNAL_USE_ONLY", true);
    int32_t* m_phaseId = m_phaseIdLengthPtr->getPointer(0);

    FloatArrayType::Pointer m_orientLengthPtr = FloatArrayType::CreateArray(maxGrainId * 3, "ORIENTATION_INTERNAL_USE_ONLY", true);
    float* m_orient = m_orientLengthPtr->getPointer(0);

    int32_t grainId = 1;
    while(grainId <= maxGrainId)
    {
      for(int32_t i = 0; i < totalPoints; i++)
      {
        if(m_FeatureIds[i] == grainId)
        {
          m_phaseId[grainId - 1] = m_CellPhases[i];
          m_orient[(grainId - 1) * 3] = m_CellEulerAngles[i * 3] * 180.0 * SIMPLib::Constants::k_1OverPi;
          m_orient[(grainId - 1) * 3 + 1] = m_CellEulerAngles[i * 3 + 1] * 180.0 * SIMPLib::Constants::k_1OverPi;
          m_orient[(grainId - 1) * 3 + 2] = m_CellEulerAngles[i * 3 + 2] * 180.0 * SIMPLib::Constants::k_1OverPi;
        }
      }
      grainId++;
    }
    //
    //
    //
    fprintf(matf, "<texture>\n");
    for(int32_t i = 1; i <= maxGrainId; i++)
    {
      fprintf(matf, "[grain%d]\n", i);
      fprintf(matf, "(gauss) phi1 %.3f   Phi %.3f    phi2 %.3f   scatter 0.0   fraction 1.0 \n", m_orient[(i - 1) * 3], m_orient[(i - 1) * 3 + 1], m_orient[(i - 1) * 3 + 2]);
    }

    fprintf(matf, "<microstructure>\n");
    for(int32_t i = 1; i <= maxGrainId; i++)
    {
      fprintf(matf, "[grain%d]\n", i);
      fprintf(matf, "crystallite 1\n");
      fprintf(matf, "(constituent)   phase %d texture %d fraction 1.0\n", m_phaseId[i - 1], i);
    }
    //
    //
    break;
  }
    //
  }
  //
  //
  fclose(geomf);
  fclose(matf);
  //
  //
  if(getErrorCode() < 0)
  {
    return;
  }
  //
  if(getCancel())
  {
    return;
  }
  //
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExportDAMASKFiles::newFilterInstance(bool copyFilterParameters) const
{
  ExportDAMASKFiles::Pointer filter = ExportDAMASKFiles::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getBrandingString() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getHumanLabel() const
{
  return "Export DAMASK Files";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ExportDAMASKFiles::getUuid() const
{
  return QUuid("{7c58e612-d7d6-5ec7-806b-cce0c1c211a3}");
}

// -----------------------------------------------------------------------------
ExportDAMASKFiles::Pointer ExportDAMASKFiles::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ExportDAMASKFiles> ExportDAMASKFiles::New()
{
  struct make_shared_enabler : public ExportDAMASKFiles
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getNameOfClass() const
{
  return QString("ExportDAMASKFiles");
}

// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::ClassName()
{
  return QString("ExportDAMASKFiles");
}

// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setDataFormat(int value)
{
  m_DataFormat = value;
}

// -----------------------------------------------------------------------------
int ExportDAMASKFiles::getDataFormat() const
{
  return m_DataFormat;
}

// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setGeometryFileName(const QString& value)
{
  m_GeometryFileName = value;
}

// -----------------------------------------------------------------------------
QString ExportDAMASKFiles::getGeometryFileName() const
{
  return m_GeometryFileName;
}

// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setHomogenizationIndex(int value)
{
  m_HomogenizationIndex = value;
}

// -----------------------------------------------------------------------------
int ExportDAMASKFiles::getHomogenizationIndex() const
{
  return m_HomogenizationIndex;
}

// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExportDAMASKFiles::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExportDAMASKFiles::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setCellEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_CellEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExportDAMASKFiles::getCellEulerAnglesArrayPath() const
{
  return m_CellEulerAnglesArrayPath;
}

// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setCompressGeomFile(bool value)
{
  m_CompressGeomFile = value;
}

// -----------------------------------------------------------------------------
bool ExportDAMASKFiles::getCompressGeomFile() const
{
  return m_CompressGeomFile;
}
