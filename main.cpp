#include <iostream>

#include "Convert2Shape.h"
#include "Convert2GDB.h"
#include "CreateTopologyGDB.h"
#include "CreateGDB.h"
#include "LoadLayer.h"

#include "LicenseUtilities.h"
#include "PathUtilities.h"

HRESULT TestFunc(void);
HRESULT TestReadShape(void);

int main(int argc, char** argv) {
  
  if (!InitializeApp()) {
    AoExit(0);
  }

  HRESULT hr = S_OK;

  //HRESULT hr = TestFunc();
  //HRESULT hr = TestReadShape();
  //HRESULT hr = Convert2Shape();
  //hr = Convert2GDBFeatureClass();
  //hr = CreateTopologyGDB();
  //hr = CreateGDB();
  //hr = ValidateTopology();
  //hr = CheckTopology();
  //hr = testLoadLayerFromGDB();
  hr = testOpenFeatureSet();

  // Check returned HRESULT
  switch (hr)
  {
  case S_OK:
    std::cerr << "hr is S_OK" << std::endl;
    break;
  case E_FAIL:
    std::cerr << "hr is E_FAIL" << std::endl;
    break;
  case E_POINTER:
    std::cerr << "hr is E_POINTER" << std::endl;
    break;
  default:
    std::cerr << "hr is 0x" << std::hex << hr << std::dec << " " << hr << std::endl;
    break;
  }

  ShutdownApp();
  AoExit(0);
}

HRESULT TestFunc(void) {
  const char * fullpath = "/home/admin/Documents/test/example.gdb";
  CComBSTR path;
  CComBSTR name;
  CComBSTR bstrFullPath;
  HRESULT hr = GetParentDirFromFullPath(fullpath, &path);
  if (FAILED(hr) || path.Length() <= 0) {
    std::cerr << "Couldn't get path" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"path: " << (BSTR)path << std::endl;
  }
  hr = GetFileFromFullPath(fullpath, &name);
  if (FAILED(hr) || name.Length() <= 0) {
    std::cerr << "Couldn't get name" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"name: " << (BSTR)name << std::endl;
  }
  
  bstrFullPath = path;
  bstrFullPath.Append("/");
  bstrFullPath.Append(name);
  std::wcout << L"fullpath: " << (BSTR)bstrFullPath << std::endl;

  IWorkspaceFactoryPtr ipWorkspaceFactory(CLSID_FileGDBWorkspaceFactory);
  VARIANT_BOOL chk;
  hr = ipWorkspaceFactory->IsWorkspace(bstrFullPath, &chk);
  if (FAILED(hr)) {
    std::cerr << "Couldn't check workspace" << std::endl;
    return E_FAIL;
  } else {
    if (chk == VARIANT_FALSE) {
      std::cerr << "Invalid workspace" << std::endl;
      return E_FAIL;
    }
  }

  IWorkspacePtr ipWorkspace;
  hr = ipWorkspaceFactory->OpenFromFile(bstrFullPath, 0, &ipWorkspace);
  if (FAILED(hr) || ipWorkspace == 0) {
    std::cout << "Failed to open the dest folder." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "success" << std::endl;
  }

  IFeatureWorkspacePtr ipFeatureWorkspace(ipWorkspace);
  IFeatureDatasetPtr ipFeatureDataset;
  hr = ipFeatureWorkspace->OpenFeatureDataset(CComBSTR("test"), &ipFeatureDataset);
  if (SUCCEEDED(hr) && ipFeatureDataset != 0) {
    std::cout << "Feature set exists, will be deleted" << std::endl;
    hr = ipFeatureDataset->Delete();
  } else {
    std::cout << "Feature set does not exists" << std::endl;
  }

  // create spatial reference
  ISpatialReferenceFactory2Ptr ipSpaRefFact2(CLSID_SpatialReferenceEnvironment);
  ISpatialReferencePtr ipSpaRef;
  hr = ipSpaRefFact2->CreateSpatialReference(esriSRProjCS_WGS1984WebMercatorMajorAuxSphere, &ipSpaRef);
  if (FAILED(hr) || ipSpaRef == 0) {
    std::cerr << "create SP failed." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "create sp success." << std::endl;
  }

  // create feature data set
  IFeatureDatasetPtr ipOtherFeatureDataset;
  hr = ipFeatureWorkspace->CreateFeatureDataset(CComBSTR("test"), ipSpaRef, &ipOtherFeatureDataset);
  if (FAILED(hr) || ipOtherFeatureDataset == 0) {
    std::cerr << "create fature set failed." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "create feature set success." << std::endl;
  }

  return hr;
}

HRESULT TestReadShape(void) {
  const char * fullpath = "/home/admin/Documents/Road/chinaroad.shp";
  CComBSTR path;
  CComBSTR name;
  CComBSTR bstrFullPath;
  HRESULT hr = GetParentDirFromFullPath(fullpath, &path);
  if (FAILED(hr) || path.Length() <= 0) {
    std::cerr << "Couldn't get path" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"path: " << (BSTR)path << std::endl;
  }
  hr = GetFileFromFullPath(fullpath, &name);
  if (FAILED(hr) || name.Length() <= 0) {
    std::cerr << "Couldn't get name" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"name: " << (BSTR)name << std::endl;
  }
  
  bstrFullPath = path;
  bstrFullPath.Append("/");
  bstrFullPath.Append(name);
  std::wcout << L"fullpath: " << (BSTR)bstrFullPath << std::endl;

  IWorkspaceFactoryPtr ipWorkspaceFactory(CLSID_ShapefileWorkspaceFactory);
  VARIANT_BOOL chk;
  hr = ipWorkspaceFactory->IsWorkspace(path, &chk);
  if (FAILED(hr)) {
    std::cerr << "Couldn't check workspace" << std::endl;
    return E_FAIL;
  } else {
    if (chk == VARIANT_FALSE) {
      std::cerr << "Invalid workspace" << std::endl;
      return E_FAIL;
    }
  }

  IWorkspacePtr ipWorkspace;
  hr = ipWorkspaceFactory->OpenFromFile(path, 0, &ipWorkspace);
  if (FAILED(hr) || ipWorkspace == 0) {
    std::cerr << "Failed to open the dest folder." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "open folder success" << std::endl;
  }

  IFeatureWorkspacePtr ipFeatureWorkspace(ipWorkspace);
  IFeatureClassPtr ipFeatureClass;
  hr = ipFeatureWorkspace->OpenFeatureClass(name, &ipFeatureClass);
  if (FAILED(hr) || ipWorkspace == 0) {
    std::cerr << "Read feature class faild." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "Read fature class success." << std::endl;
  }

  // get class fields
  IClassPtr ipClass(ipFeatureClass);
  IFieldsPtr ipFieldsPtr;
  ipClass->get_Fields(&ipFieldsPtr);
  long lFieldNumber;
  ipFieldsPtr->get_FieldCount(&lFieldNumber);
  std::wcerr << lFieldNumber << std::endl;
  
  // list all data
  IFeatureCursorPtr ipFeatureCursor;
  ipFeatureClass->Search(0, VARIANT_TRUE, &ipFeatureCursor);

  IFieldsPtr ipFields;
  ipFeatureCursor->get_Fields(&ipFields);

  long lFieldCount;
  ipFields->get_FieldCount(&lFieldCount);
  std::wcerr << lFieldCount << std::endl;

  IFieldPtr ipField;
  CComBSTR bsFieldNames;
  CComBSTR bsFieldName;
  for (int i = 0; i < lFieldCount; ++i)
  {
    ipFields->get_Field(i, &ipField);
    ipField->get_Name(&bsFieldName);
    bsFieldNames += CComBSTR(L"  "); 
    bsFieldNames += bsFieldName;
  }

  std::wcerr << (BSTR) bsFieldNames << std::endl;

  /*
  IFeaturePtr ipFeature;
  ipFeatureCursor->NextFeature(&ipFeature);

  CComBSTR bsValues;
  CComVariant varValue;
  esriFieldType esriFldType;
  while (ipFeature)
  {
    bsValues = CComBSTR(L"  ");
    for (int i = 0; i < lFieldCount; ++i)
    {
      ipFields->get_Field(i, &ipField);
      ipField->get_Type(&esriFldType);
      switch (esriFldType)
	    {
	    case (esriFieldTypeGeometry):
	      bsValues += CComBSTR(L"Shape  ");
	      break;
	    default:
	      ipFeature->get_Value(i, &varValue);
	      if (varValue.vt != VT_BSTR)
          varValue.ChangeType(VT_BSTR);
	      bsValues += varValue.bstrVal;
	      bsValues += CComBSTR(L"  ");
	      break;
	    }
    }
    std::wcerr << (BSTR) bsValues << std::endl;
    ipFeatureCursor->NextFeature(&ipFeature);
  }
  */

  return hr;
}
