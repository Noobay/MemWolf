/*
  Overwolf Sample Plugin
  Copyright (c) 2014 Overwolf Ltd.
*/
#ifndef NNSSCRIPTABLEOBJECTOverwolfReadMemorySample_H_
#define NNSSCRIPTABLEOBJECTOverwolfReadMemorySample_H_

//plugin-specific dependencies

#include "nsScriptableObjectBase.h"
#include <map>




namespace utils {
class Thread; // forward declaration
}

class nsScriptableObjectOverwolfReadMemorySample : public nsScriptableObjectBase {
public:
  nsScriptableObjectOverwolfReadMemorySample(NPP npp);
  virtual ~nsScriptableObjectOverwolfReadMemorySample(void);

public:
  bool Init();

// nsScriptableObjectBase overrides
public:
  virtual bool HasMethod(NPIdentifier name);
  virtual bool Invoke(
    NPIdentifier name, 
    const NPVariant *args, 
    uint32_t argCount, 
    NPVariant *result);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value);

// public methods
private:
  // Because we want to have a responsive plugin - we make sure that all
  // our functions run on a separate thread (not on the main browser
  // thread).  To achieve this, we create a Task function (see Thread.h) that 
  // runs on a separate thread triggers a callback

  // Initiate Memory Reading 
	bool InitReadMemory(
    NPIdentifier name, 
    const NPVariant *args, 
    uint32_t argCount, 
    NPVariant *result);

	// Initiate Memory Reading for 64-bit Process
	bool InitReadMemory64(
	  NPIdentifier name,
	  const NPVariant *args,
	  uint32_t argCount,
	  NPVariant *result);

  // Check Memory for loot Availability
    bool GW2IsLootAvailable(
    NPIdentifier name, 
    const NPVariant *args, 
    uint32_t argCount, 
    NPVariant *result);

  // Check Memory for Loot Availability on 64-bit Process
  bool GW2IsLootAvailable64(
	  NPIdentifier name,
	  const NPVariant *args,
	  uint32_t argCount,
	  NPVariant *result);
  
  bool FinishReading(
    NPIdentifier name, 
    const NPVariant *args, 
    uint32_t argCount, 
    NPVariant *result);

// These are the functions that run on a separate thread - 
// there is one for each public function we expose.
// Eventually, they will run TriggerCallbackOnMainThread
private:
  void InitReadMemoryTask(
    const std::string& message,
	const std::string& caption,
	double offset,
    NPObject* callback);

  void InitReadMemory64Task(
	  const std::string& message,
	  const std::string& caption,
	  double offset,
	  NPObject* callback);

  void GW2IsLootAvailableTask(
    NPObject* callback);

  void GW2IsLootAvailable64Task(
	  NPObject* callback);

  void FinishReadingTask(
    NPObject* callback);

// member variables
private:
  // defines a generic method
  typedef bool (nsScriptableObjectOverwolfReadMemorySample::*MethodHandle)(
    NPIdentifier,
    const NPVariant*, 
    uint32_t, 
    NPVariant*);

  // holds the public methods
  typedef std::map<NPIdentifier, MethodHandle> MethodsMap;
  MethodsMap methods_;

  double sample_property_;

  // good idea for when having an autonomous thread sending callbacks
  bool shutting_down_;

  // this allows us to run our code on a separate thread than the 
  // main browser thread - to be more responsive
  std::auto_ptr<utils::Thread> thread_;
};

// declare our NPObject-derived scriptable object class
DECLARE_NPOBJECT_CLASS_WITH_BASE(
  nsScriptableObjectOverwolfReadMemorySample, 
  AllocateNpObject<nsScriptableObjectOverwolfReadMemorySample>);

#endif // NNSSCRIPTABLEOBJECTOverwolfReadMemorySample_H_