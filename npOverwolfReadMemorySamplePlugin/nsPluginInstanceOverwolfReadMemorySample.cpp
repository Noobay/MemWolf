/*
  Overwolf Sample Plugin
  Copyright (c) 2014 Ovefwolf Ltd.
*/
#include "nsPluginInstanceOverwolfReadMemorySample.h"
#include "nsScriptableObjectOverwolfReadMemorySample.h" // our specific API

// we use this to force our plugin container to shut down
// when no one is using it.  Browsers try to keep the plugin
// open for optimization reasons - we don't want it
int nsPluginInstanceOverwolfReadMemorySample::ref_count_ = 0;

////////////////////////////////////////
//
// nsPluginInstanceOverwolfReadMemorySample class implementation
//
nsPluginInstanceOverwolfReadMemorySample::nsPluginInstanceOverwolfReadMemorySample(NPP instance) :
  nsPluginInstanceBase(),
  instance_(instance),
  initialized_(FALSE),
  scriptable_object_(nullptr) {

  nsPluginInstanceOverwolfReadMemorySample::ref_count_++;
}

nsPluginInstanceOverwolfReadMemorySample::~nsPluginInstanceOverwolfReadMemorySample() {
  nsPluginInstanceOverwolfReadMemorySample::ref_count_--;

  if (0 == nsPluginInstanceOverwolfReadMemorySample::ref_count_) {
    PostQuitMessage(0);
  }
}

// NOTE:
// ------
// Overwolf plugins should not implement windows - NPAPI will
// probably be removed in the near feature and will be changed
// by a different method that will only support non-visual
// plugins
NPBool nsPluginInstanceOverwolfReadMemorySample::init(NPWindow* window) {
  // no GUI to init in windowless case
  initialized_ = TRUE;
  return TRUE;
}

void nsPluginInstanceOverwolfReadMemorySample::shut() {
  if (nullptr != scriptable_object_) {
    NPN_ReleaseObject(scriptable_object_);
  }

  initialized_ = FALSE;
}

NPBool nsPluginInstanceOverwolfReadMemorySample::isInitialized() {
  return initialized_;
}

// here we supply our scriptable object
NPError nsPluginInstanceOverwolfReadMemorySample::GetValue(
  NPPVariable variable, void* ret_value) {
  
  NPError rv = NPERR_INVALID_PARAM;

  switch (variable) {
    case NPPVpluginScriptableNPObject:
    {
      if (nullptr == scriptable_object_) {
        scriptable_object_ = 
          NPN_CreateObject(
            instance_, 
            GET_NPOBJECT_CLASS(nsScriptableObjectOverwolfReadMemorySample));

        NPN_RetainObject(scriptable_object_);

        ((nsScriptableObjectOverwolfReadMemorySample*)scriptable_object_)->Init();
        *(NPObject **)ret_value = scriptable_object_;
      }

      rv = NPERR_NO_ERROR;
      return rv;
    }
    default:
      break;
  }

  return rv;
}