#include "LoadLayer.h"
#include "PathUtilities.h"

#include <iostream>

HRESULT testLoadLayerFromGDB(void) {
  HRESULT hr = S_OK;

  const char * fullpath = "/home/admin/Documents/test/report.gdb";
  CComBSTR path;
  CComBSTR name;
  CComBSTR bstrFullPath;
  hr = GetParentDirFromFullPath(fullpath, &path);
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
    std::cerr << "Failed to open the dest folder." << std::endl;
    return E_FAIL;
  } else {
    std::cerr << "open the dest folder success" << std::endl;
  }

  IEnumDatasetNamePtr ipEnumDatasetName;
  hr = ipWorkspace->get_DatasetNames(esriDTFeatureDataset, &ipEnumDatasetName);
  if (FAILED(hr) || ipEnumDatasetName == 0) {
    std::cerr << "Get dataset names failed" << std::endl;
  } else {
    std::cerr << "Get dataset names success" << std::endl;
  }

  IDatasetNamePtr ipDatasetName;
  ipEnumDatasetName->Next(&ipDatasetName);
  while (ipDatasetName != 0) {
    CComBSTR name;
    ipDatasetName->get_Name(&name);
    std::wcerr << L"dataset name: " << (BSTR)name << std::endl;

    // get feature class that included in a feature set
    IFeatureDatasetName2Ptr ipFeatureDatasetName2(ipDatasetName);
    IEnumDatasetNamePtr ipFeatureClassNames;
    hr = ipFeatureDatasetName2->get_FeatureClassNames(&ipFeatureClassNames);
    if (FAILED(hr)) {
      std::wcerr << L"get feature class names failed" << std::endl;
    } else {
      std::wcerr << L"get feature class names success" << std::endl;
    }
    IDatasetNamePtr ipFeatureName;
    ipFeatureClassNames->Next(&ipFeatureName);
    while (ipFeatureName != 0) {
      ipFeatureName->get_Name(&name);
      std::wcerr << L"feature Name: " << (BSTR)name << std::endl;
      ipFeatureClassNames->Next(&ipFeatureName);
    }
    
    IEnumDatasetNamePtr ipTopologyNames;
    ipFeatureDatasetName2->get_TopologyNames(&ipTopologyNames);
    IDatasetNamePtr ipTopologyName;
    ipTopologyNames->Next(&ipTopologyName);
    while (ipTopologyName != 0) {
      ipTopologyName->get_Name(&name);
      std::wcerr << L"topology name: " << (BSTR)name << std::endl;
      ipTopologyNames->Next(&ipTopologyName);
    }

    ipEnumDatasetName->Next(&ipDatasetName);
  }

  IFeatureWorkspacePtr ipFeatureWorkspace(ipWorkspace);
  IFeatureDatasetPtr ipFeatureDataset;
  hr = ipFeatureWorkspace->OpenFeatureDataset(CComBSTR("test"), &ipFeatureDataset);
  if (SUCCEEDED(hr) && ipFeatureDataset != 0) {
    std::cerr << "Feature set exists" << std::endl;
  } else {
    std::cerr << "Feature set does not exists" << std::endl;
    return E_FAIL;
  }

  ITopologyWorkspacePtr ipTopologyWorkspace(ipWorkspace);
  ITopologyPtr ipTopology;
  hr = ipTopologyWorkspace->OpenTopology(CComBSTR("topology"), &ipTopology);
  if (FAILED(hr) || ipTopology == 0) {
    std::cerr << "open topology failed" << std::endl;
  } else {
    std::cerr << "open topology success" << std::endl;
  }
  
  ITopologyRuleContainerPtr ipTopoRuleContainer(ipTopology);
  IEnumRulePtr ipEnumRule;
  hr = ipTopoRuleContainer->get_Rules(&ipEnumRule);
  if (FAILED(hr) || ipEnumRule == 0) {
    std::cerr << "get topology rules failed" << std::endl;
  } else {
    std::cerr << "get topology rules success" << std::endl;
  }

  // enum topology rules type
  ipEnumRule->Reset();
  IRulePtr ipRule;
  ipEnumRule->Next(&ipRule);
  while (ipRule != 0) {
    esriRuleType type;
    ipRule->get_Type(&type);
    std::cerr << "topology type: " << type << std::endl;
    ipEnumRule->Next(&ipRule);
  }

  IGeoDatasetPtr ipGeoDataset(ipTopology);
  ISpatialReferencePtr ipSpatialReference;
  ipGeoDataset->get_SpatialReference(&ipSpatialReference);

  IEnvelopePtr ipEnvelope;
  ipGeoDataset->get_Extent(&ipEnvelope);

  ipEnumRule->Reset();
  ipEnumRule->Next(&ipRule);
  while (ipRule != 0) {
    // show topology rule name
    ITopologyRulePtr ipTopologyRule(ipRule);
    CComBSTR name;
    ipTopologyRule->get_Name(&name);
    std::wcerr << L"rule name: " << (BSTR)name << std::endl;

    IErrorFeatureContainerPtr ipErrorFeatureContainer(ipTopology);
    IEnumTopologyErrorFeaturePtr ipEnumTopologyErrorFeature;
    hr = ipErrorFeatureContainer->get_ErrorFeatures(ipSpatialReference,
                                                    (ITopologyRulePtr)ipRule,
                                                    ipEnvelope,
                                                    TRUE,
                                                    TRUE,
                                                    &ipEnumTopologyErrorFeature);
    if (FAILED(hr)) {
      std::cerr << "Get topology error feature failed" << std::endl; 
    } else {
      std::cerr << "Get topology error feature success" << std::endl; 
    }
    ITopologyErrorFeaturePtr ipTopologyErrorFeature;
    ipEnumTopologyErrorFeature->Next(&ipTopologyErrorFeature);
    while (ipTopologyErrorFeature != 0) {
      long oid;
      ipTopologyErrorFeature->get_OriginOID(&oid);
      std::cerr << "Origin feature OID: " << oid << std::endl;
      ipEnumTopologyErrorFeature->Next(&ipTopologyErrorFeature);
    }
    ipEnumRule->Next(&ipRule);
  }

  return hr;
}

HRESULT testOpenFeatureSet(void) {
  HRESULT hr = S_OK;

  const char * fullpath = "/home/admin/Documents/test/report.gdb";
  CComBSTR path;
  CComBSTR name;
  CComBSTR bstrFullPath;
  hr = GetParentDirFromFullPath(fullpath, &path);
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
    std::cerr << "Failed to open the dest folder." << std::endl;
    return E_FAIL;
  } else {
    std::cerr << "open the dest folder success" << std::endl;
  }

  IEnumDatasetNamePtr ipEnumDatasetName;
  hr = ipWorkspace->get_DatasetNames(esriDTFeatureDataset, &ipEnumDatasetName);
  if (FAILED(hr) || ipEnumDatasetName == 0) {
    std::cerr << "Get dataset names failed" << std::endl;
  } else {
    std::cerr << "Get dataset names success" << std::endl;
  }

  ILayerFactoryHelperPtr ipLayerFactoryHelper(CLSID_LayerFactoryHelper);

  IDatasetNamePtr ipDatasetName;
  ipEnumDatasetName->Next(&ipDatasetName);
  while (ipDatasetName != 0) {
    CComBSTR name;
    ipDatasetName->get_Name(&name);
    std::wcerr << L"dataset name: " << (BSTR)name << std::endl;

    // get feature class that included in a feature set
    IFeatureDatasetName2Ptr ipFeatureDatasetName2(ipDatasetName);
    IEnumDatasetNamePtr ipFeatureClassNames;
    hr = ipFeatureDatasetName2->get_FeatureClassNames(&ipFeatureClassNames);
    if (FAILED(hr)) {
      std::wcerr << L"get feature class names failed" << std::endl;
    } else {
      std::wcerr << L"get feature class names success" << std::endl;
    }
    IDatasetNamePtr ipFeatureName;
    ipFeatureClassNames->Next(&ipFeatureName);
    while (ipFeatureName != 0) {
      ipFeatureName->get_Name(&name);
      std::wcerr << L"feature Name: " << (BSTR)name << std::endl;
      ipFeatureClassNames->Next(&ipFeatureName);
    }

    ipFeatureClassNames->Reset();
    ipFeatureClassNames->Next(&ipFeatureName);
    while (ipFeatureName != 0) {
      IEnumLayerPtr ipEnumLayer;
      hr = ipLayerFactoryHelper->CreateLayersFromName((INamePtr)ipFeatureName, &ipEnumLayer);
      if (FAILED(hr)) {
        std::cerr << "create layer failed" << std::endl;
      } else {
        std::cerr << "create layer success" << std::endl;
      }
      ipFeatureClassNames->Next(&ipFeatureName);
    }
    
    IEnumDatasetNamePtr ipTopologyNames;
    ipFeatureDatasetName2->get_TopologyNames(&ipTopologyNames);
    IDatasetNamePtr ipTopologyName;
    ipTopologyNames->Next(&ipTopologyName);
    while (ipTopologyName != 0) {
      ipTopologyName->get_Name(&name);
      std::wcerr << L"topology name: " << (BSTR)name << std::endl;
      ipTopologyNames->Next(&ipTopologyName);
    }

    ipEnumDatasetName->Next(&ipDatasetName);
  }

  IFeatureWorkspacePtr ipFeatureWorkspace(ipWorkspace);
  IFeatureDatasetPtr ipFeatureDataset;
  hr = ipFeatureWorkspace->OpenFeatureDataset(CComBSTR("test"), &ipFeatureDataset);
  if (SUCCEEDED(hr) && ipFeatureDataset != 0) {
    std::cerr << "Feature set exists" << std::endl;
  } else {
    std::cerr << "Feature set does not exists" << std::endl;
    return E_FAIL;
  }

  ITopologyWorkspacePtr ipTopologyWorkspace(ipWorkspace);
  ITopologyPtr ipTopology;
  hr = ipTopologyWorkspace->OpenTopology(CComBSTR("topology"), &ipTopology);
  if (FAILED(hr) || ipTopology == 0) {
    std::cerr << "open topology failed" << std::endl;
  } else {
    std::cerr << "open topology success" << std::endl;
  }
  
  ITopologyRuleContainerPtr ipTopoRuleContainer(ipTopology);
  IEnumRulePtr ipEnumRule;
  hr = ipTopoRuleContainer->get_Rules(&ipEnumRule);
  if (FAILED(hr) || ipEnumRule == 0) {
    std::cerr << "get topology rules failed" << std::endl;
  } else {
    std::cerr << "get topology rules success" << std::endl;
  }

  // enum topology rules type
  ipEnumRule->Reset();
  IRulePtr ipRule;
  ipEnumRule->Next(&ipRule);
  while (ipRule != 0) {
    esriRuleType type;
    ipRule->get_Type(&type);
    std::cerr << "topology type: " << type << std::endl;
    ipEnumRule->Next(&ipRule);
  }

  IGeoDatasetPtr ipGeoDataset(ipTopology);
  ISpatialReferencePtr ipSpatialReference;
  ipGeoDataset->get_SpatialReference(&ipSpatialReference);

  IEnvelopePtr ipEnvelope;
  ipGeoDataset->get_Extent(&ipEnvelope);

  ipEnumRule->Reset();
  ipEnumRule->Next(&ipRule);
  while (ipRule != 0) {
    // show topology rule name
    ITopologyRulePtr ipTopologyRule(ipRule);
    CComBSTR name;
    ipTopologyRule->get_Name(&name);
    std::wcerr << L"rule name: " << (BSTR)name << std::endl;

    IErrorFeatureContainerPtr ipErrorFeatureContainer(ipTopology);
    IEnumTopologyErrorFeaturePtr ipEnumTopologyErrorFeature;
    hr = ipErrorFeatureContainer->get_ErrorFeatures(ipSpatialReference,
                                                    (ITopologyRulePtr)ipRule,
                                                    ipEnvelope,
                                                    TRUE,
                                                    TRUE,
                                                    &ipEnumTopologyErrorFeature);
    if (FAILED(hr)) {
      std::cerr << "Get topology error feature failed" << std::endl; 
    } else {
      std::cerr << "Get topology error feature success" << std::endl; 
    }
    ITopologyErrorFeaturePtr ipTopologyErrorFeature;
    ipEnumTopologyErrorFeature->Next(&ipTopologyErrorFeature);
    while (ipTopologyErrorFeature != 0) {
      long oid;
      ipTopologyErrorFeature->get_OriginOID(&oid);
      std::cerr << "Origin feature OID: " << oid << std::endl;
      ipEnumTopologyErrorFeature->Next(&ipTopologyErrorFeature);
    }
    ipEnumRule->Next(&ipRule);
  }

  return hr;
}
