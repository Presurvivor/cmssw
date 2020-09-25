#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/DDCMS/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsDetConstruction.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

template <class FilteredView>
void CmsDetConstruction<FilteredView>::buildSmallDetsforGlued(FilteredView& fv,
                                                              GeometricDet* mother,
                                                              const std::string& attribute) {
  GeometricDet* det = new GeometricDet(&fv,
                                       CmsTrackerLevelBuilder<FilteredView>::theCmsTrackerStringToEnum.type(
                                           ExtractStringFromDDD<FilteredView>::getString(attribute, &fv)));
  if (det->stereo()) {
    uint32_t temp = 1;
    det->setGeographicalID(DetId(temp));
  } else {
    uint32_t temp = 2;
    det->setGeographicalID(DetId(temp));
  }

  mother->addComponent(det);
}

template <class FilteredView>
void CmsDetConstruction<FilteredView>::buildSmallDetsforStack(FilteredView& fv,
                                                              GeometricDet* mother,
                                                              const std::string& attribute) {
  GeometricDet* det = new GeometricDet(&fv,
                                       CmsTrackerLevelBuilder<FilteredView>::theCmsTrackerStringToEnum.type(
                                           ExtractStringFromDDD<FilteredView>::getString(attribute, &fv)));

  if (det->isLowerSensor()) {
    uint32_t temp = 1;
    det->setGeographicalID(DetId(temp));
  } else if (det->isUpperSensor()) {
    uint32_t temp = 2;
    det->setGeographicalID(DetId(temp));
  } else {
    edm::LogError("DetConstruction") << " module defined in a Stack but not upper either lower!? ";
  }
  mother->addComponent(det);
}

template <>
void CmsDetConstruction<DDFilteredView>::buildComponent(DDFilteredView& fv,
                                                        GeometricDet* mother,
                                                        const std::string& attribute) {

  GeometricDet* det = new GeometricDet(&fv,
                                       CmsTrackerLevelBuilder<DDFilteredView>::theCmsTrackerStringToEnum.type(
                                           ExtractStringFromDDD<DDFilteredView>::getString(attribute, &fv)));

  //Phase1 mergedDet: searching for sensors
  if (CmsTrackerLevelBuilder<DDFilteredView>::theCmsTrackerStringToEnum.type(
          ExtractStringFromDDD<DDFilteredView>::getString(attribute, &fv)) == GeometricDet::mergedDet) {
    // I have to go one step lower ...
    bool dodets = fv.firstChild();  // descend to the first Layer
    while (dodets) {
      buildSmallDetsforGlued(fv, det, attribute);
      dodets = setNext(fv);
    }
    fv.parent();

  }

  //Phase2 stackDet: same procedure, different nomenclature
  else if (CmsTrackerLevelBuilder<DDFilteredView>::theCmsTrackerStringToEnum.type(
               ExtractStringFromDDD<DDFilteredView>::getString(attribute, &fv)) == GeometricDet::OTPhase2Stack) {
    bool dodets = fv.firstChild();
    while (dodets) {
      buildSmallDetsforStack(fv, det, attribute);
      dodets = setNext(fv);
    }
    fv.parent();
  }

  mother->addComponent(det);
}

template <>
void CmsDetConstruction<cms::DDFilteredView>::buildComponent(cms::DDFilteredView& fv,
                                                             GeometricDet* mother,
                                                             const std::string& attribute) {
  GeometricDet* det = new GeometricDet(&fv,
                                       CmsTrackerLevelBuilder<cms::DDFilteredView>::theCmsTrackerStringToEnum.type(
                                           ExtractStringFromDDD<cms::DDFilteredView>::getString(attribute, &fv)));

  // PHASE 1 (MERGEDDET)
  if (CmsTrackerLevelBuilder<cms::DDFilteredView>::theCmsTrackerStringToEnum.type(
          ExtractStringFromDDD<cms::DDFilteredView>::getString(attribute, &fv)) == GeometricDet::mergedDet) {
    // Go down in hierarchy: from module to sensor
    if (!fv.firstChild()) {
      edm::LogError("CmsDetConstruction::buildComponent. Cannot go down to sensor volume.");
      return;
    }

    // This is the sensor hierarchy level
    const int sensorHierarchyLevel = fv.level();

    // Loop on all siblings (ie, on all sensors)
    while (fv.level() == sensorHierarchyLevel) {
      buildSmallDetsforGlued(fv, det, attribute);

      // Go to the next volume in FilteredView.
      // NB: If this volume is another sensor of the same module, will stay in the loop.
      fv.firstChild();
    }
  }

  // PHASE 2 (STACKDET)
  else if (CmsTrackerLevelBuilder<cms::DDFilteredView>::theCmsTrackerStringToEnum.type(
               ExtractStringFromDDD<cms::DDFilteredView>::getString(attribute, &fv)) == GeometricDet::OTPhase2Stack) {
    // Go down in hierarchy: from module to sensor
    if (!fv.firstChild()) {
      edm::LogError("CmsDetConstruction::buildComponent. Cannot go down to sensor volume.");
      return;
    }

    // This is the sensor hierarchy level
    const int sensorHierarchyLevel = fv.level();

    // Loop on all siblings (ie, on all sensors)
    while (fv.level() == sensorHierarchyLevel) {
      buildSmallDetsforStack(fv, det, attribute);

      // Go to the next volume in FilteredView.
      // NB: If this volume is another sensor of the same module, will stay in the loop.
      fv.firstChild();
    }
  }

  else {
    // Go to the next volume in FilteredView.
    fv.firstChild();
  }

  mother->addComponent(det);
}
