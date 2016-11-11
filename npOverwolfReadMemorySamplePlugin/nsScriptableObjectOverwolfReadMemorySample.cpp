/*
  Overwolf Sample Plugin
  Copyright (c) 2014 Ovefwolf Ltd.
*/
#include "MemWolf.h"
#include "nsScriptableObjectOverwolfReadMemorySample.h"
#include "utils/Thread.h"

MemWolfObject memWolfReader;

#define REGISTER_METHOD(name, method) { \
  methods_[NPN_GetStringIdentifier(name)] = &method; \
}

const char kAPropretyName[] = "sampleProperty";

nsScriptableObjectOverwolfReadMemorySample::nsScriptableObjectOverwolfReadMemorySample(NPP npp) :
  nsScriptableObjectBase(npp),
  sample_property_(0.0),
  shutting_down_(false) {
}

nsScriptableObjectOverwolfReadMemorySample::~nsScriptableObjectOverwolfReadMemorySample(void) {
  shutting_down_ = true;
  
  if (thread_.get()) {
    thread_->Stop();
  }
}

bool nsScriptableObjectOverwolfReadMemorySample::Init() {
  REGISTER_METHOD("InitReadMemory", nsScriptableObjectOverwolfReadMemorySample::InitReadMemory);
  REGISTER_METHOD("InitReadMemory64", nsScriptableObjectOverwolfReadMemorySample::InitReadMemory64);
  REGISTER_METHOD("GW2IsLootAvailable", nsScriptableObjectOverwolfReadMemorySample::GW2IsLootAvailable);
  REGISTER_METHOD("GW2IsLootAvailable64", nsScriptableObjectOverwolfReadMemorySample::GW2IsLootAvailable64);

  sample_property_ = 42;

  thread_.reset(new utils::Thread());
  return thread_->Start();
}

bool nsScriptableObjectOverwolfReadMemorySample::HasMethod(NPIdentifier name) {
#ifdef _DEBUG
  NPUTF8* name_utf8 = NPN_UTF8FromIdentifier(name);
  NPN_MemFree((void*)name_utf8);
#endif

  // does the method exist?
  return (methods_.find(name) != methods_.end());
}

bool nsScriptableObjectOverwolfReadMemorySample::Invoke(
  NPIdentifier name, 
  const NPVariant *args, 
  uint32_t argCount, 
  NPVariant *result) {
#ifdef _DEBUG
      NPUTF8* szName = NPN_UTF8FromIdentifier(name);
      NPN_MemFree((void*)szName);
#endif

  // dispatch method to appropriate handler
  MethodsMap::iterator iter = methods_.find(name);
  
  if (iter == methods_.end()) {
    // should never reach here
    NPN_SetException(this, "bad function called??");
    return false;
  }

  return (this->*iter->second)(name, args, argCount, result);
}

/************************************************************************/
/* Public properties
/************************************************************************/
bool nsScriptableObjectOverwolfReadMemorySample::HasProperty(NPIdentifier name) {
  return (NPN_GetStringIdentifier(kAPropretyName) == name);
}

// NOTE: you could also implement SetProperty which receives a callback
// function (as an NPObject) and then you can trigger this callback
// with nsScriptableObjectOverwolfReadMemorySample::TriggerCallbackOnMainThread.
// This allows you to implement a "global" event for the plugin
bool nsScriptableObjectOverwolfReadMemorySample::GetProperty(
  NPIdentifier name, NPVariant *result) {

  DOUBLE_TO_NPVARIANT(sample_property_, *result);
  return true;
}

bool nsScriptableObjectOverwolfReadMemorySample::SetProperty(
  NPIdentifier name, const NPVariant *value) {
  NPN_SetException(this, "this property is read-only!");
  return true;
}


/************************************************************************/
/* Public methods
/************************************************************************/
bool nsScriptableObjectOverwolfReadMemorySample::InitReadMemory(
  NPIdentifier name, 
  const NPVariant *args, 
  uint32_t argCount, 
  NPVariant *result) {

  if (argCount < 2 ||
    !NPVARIANT_IS_STRING(args[0]) ||
	!NPVARIANT_IS_STRING(args[1]) ||
	!NPVARIANT_IS_DOUBLE (args[2]) ||
    !NPVARIANT_IS_OBJECT(args[3])) {
      NPN_SetException(this, "invalid params passed to function");
      return true;
  }

  // add ref count to callback object so it won't delete
  NPN_RetainObject(NPVARIANT_TO_OBJECT(args[3]));

  // convert into std::string
  std::string message; 
  std::string caption;

  message.append(
    NPVARIANT_TO_STRING(args[0]).UTF8Characters,
    NPVARIANT_TO_STRING(args[0]).UTF8Length);
  caption.append(
    NPVARIANT_TO_STRING(args[1]).UTF8Characters,
    NPVARIANT_TO_STRING(args[1]).UTF8Length);

  double offset = NPVARIANT_TO_DOUBLE(args[2]);
  // post to separate thread so that we are responsive
  return thread_->PostTask(
    std::bind(
      &nsScriptableObjectOverwolfReadMemorySample::InitReadMemoryTask, 
      this, 
      message, 
	  caption,
	  offset,
      NPVARIANT_TO_OBJECT(args[3])));
}

bool nsScriptableObjectOverwolfReadMemorySample::InitReadMemory64(
	NPIdentifier name,
	const NPVariant *args,
	uint32_t argCount,
	NPVariant *result) {

	if (argCount < 2 ||
		!NPVARIANT_IS_STRING(args[0]) ||
		!NPVARIANT_IS_STRING(args[1]) ||
		!NPVARIANT_IS_DOUBLE(args[2]) ||
		!NPVARIANT_IS_OBJECT(args[3])) {
		NPN_SetException(this, "invalid params passed to function");
		return true;
	}

	// add ref count to callback object so it won't delete
	NPN_RetainObject(NPVARIANT_TO_OBJECT(args[3]));

	// convert into std::string
	std::string message;
	std::string caption;

	message.append(
		NPVARIANT_TO_STRING(args[0]).UTF8Characters,
		NPVARIANT_TO_STRING(args[0]).UTF8Length);
	caption.append(
		NPVARIANT_TO_STRING(args[1]).UTF8Characters,
		NPVARIANT_TO_STRING(args[1]).UTF8Length);

	double offset = NPVARIANT_TO_DOUBLE(args[2]);
	// post to separate thread so that we are responsive
	return thread_->PostTask(
		std::bind(
			&nsScriptableObjectOverwolfReadMemorySample::InitReadMemory64Task,
			this,
			message,
			caption,
			offset,
			NPVARIANT_TO_OBJECT(args[3])));
}

bool nsScriptableObjectOverwolfReadMemorySample::GW2IsLootAvailable(
  NPIdentifier name, 
  const NPVariant *args, 
  uint32_t argCount, 
  NPVariant *result) {

  if (argCount < 1 ||
      !NPVARIANT_IS_OBJECT(args[0])) {
    NPN_SetException(this, "invalid params passed to function");
    return true;
  }

  // add ref count to callback object so it won't delete
  NPN_RetainObject(NPVARIANT_TO_OBJECT(args[0]));

  // post to separate thread so that we are responsive
  return thread_->PostTask(
    std::bind(
      &nsScriptableObjectOverwolfReadMemorySample::GW2IsLootAvailableTask, 
      this,  
      NPVARIANT_TO_OBJECT(args[0])));
}

bool nsScriptableObjectOverwolfReadMemorySample::GW2IsLootAvailable64(
	NPIdentifier name,
	const NPVariant *args,
	uint32_t argCount,
	NPVariant *result) {

	if (argCount < 1 ||
		!NPVARIANT_IS_OBJECT(args[0])) {
		NPN_SetException(this, "invalid params passed to function");
		return true;
	}

	// add ref count to callback object so it won't delete
	NPN_RetainObject(NPVARIANT_TO_OBJECT(args[0]));

	// post to separate thread so that we are responsive
	return thread_->PostTask(
		std::bind(
			&nsScriptableObjectOverwolfReadMemorySample::GW2IsLootAvailable64Task,
			this,
			NPVARIANT_TO_OBJECT(args[0])));
}

bool nsScriptableObjectOverwolfReadMemorySample::FinishReading(
  NPIdentifier name, 
  const NPVariant *args, 
  uint32_t argCount, 
  NPVariant *result) {

  if (argCount < 1 ||
    !NPVARIANT_IS_OBJECT(args[0])) {
      NPN_SetException(this, "invalid params passed to function");
      return true;
  }

  // add ref count to callback object so it won't delete
  NPN_RetainObject(NPVARIANT_TO_OBJECT(args[0]));


  // post to separate thread so that we are responsive
  return thread_->PostTask(
    std::bind(
      &nsScriptableObjectOverwolfReadMemorySample::FinishReadingTask, 
      this, 
      NPVARIANT_TO_OBJECT(args[0])));
}
/************************************************************************/
/* Separate thrad implementations for public functions
/************************************************************************/
// this is not called on the main thread - so we need to 
// use NPN_PluginThreadAsyncCall
void nsScriptableObjectOverwolfReadMemorySample::InitReadMemoryTask(
  const std::string& message,const std::string& caption, double offset, NPObject* callback) {

  if (shutting_down_) {
    return;
  }

  NPVariant arg;
  NPVariant ret_val;

    STRINGN_TO_NPVARIANT(
    message.c_str(),
    message.size(),
    arg);

	
  DOUBLE_TO_NPVARIANT(memWolfReader.InitMemoryRead(message.c_str(), caption.c_str(), offset),arg);
  //STRINGN_TO_NPVARIANT(message.c_str(),message.size(),arg);
  // fire callback
  NPN_InvokeDefault(
    __super::npp_, 
    callback, 
    &arg, 
    1, 
    &ret_val);

  NPN_ReleaseVariantValue(&ret_val);
}

void nsScriptableObjectOverwolfReadMemorySample::InitReadMemory64Task(
	const std::string& message, const std::string& caption, double offset, NPObject* callback) {

	if (shutting_down_) {
		return;
	}

	NPVariant arg;
	NPVariant ret_val;

	STRINGN_TO_NPVARIANT(
		message.c_str(),
		message.size(),
		arg);


	DOUBLE_TO_NPVARIANT(memWolfReader.InitMemoryRead64(message.c_str(), caption.c_str(), offset), arg);
	//STRINGN_TO_NPVARIANT(message.c_str(),message.size(),arg);
	// fire callback
	NPN_InvokeDefault(
		__super::npp_,
		callback,
		&arg,
		1,
		&ret_val);

	NPN_ReleaseVariantValue(&ret_val);
}

void nsScriptableObjectOverwolfReadMemorySample::GW2IsLootAvailableTask(
  NPObject* callback) {
  if (shutting_down_) {
    return;
  }

  NPVariant arg;
  NPVariant ret_val;

  BOOLEAN_TO_NPVARIANT(memWolfReader.IsLootAvailable(), arg);

  // fire callback
  NPN_InvokeDefault(
    __super::npp_, 
    callback, 
    &arg, 
    1, 
    &ret_val);

  NPN_ReleaseVariantValue(&ret_val);
}

void nsScriptableObjectOverwolfReadMemorySample::GW2IsLootAvailable64Task(
	NPObject* callback) {
	if (shutting_down_) {
		return;
	}

	NPVariant arg;
	NPVariant ret_val;
	
	BOOLEAN_TO_NPVARIANT(memWolfReader.IsLootAvailable64(), arg);

	// fire callback
	NPN_InvokeDefault(
		__super::npp_,
		callback,
		&arg,
		1,
		&ret_val);

	NPN_ReleaseVariantValue(&ret_val);
}

void nsScriptableObjectOverwolfReadMemorySample::FinishReadingTask(
  NPObject* callback) {
  if (shutting_down_) {
    return;
  }

  memWolfReader.FinishReadingMemory();
 
  NPVariant arg;
  NPVariant ret_val;

  // fire callback
  NPN_InvokeDefault(
    __super::npp_, 
    callback, 
    &arg, 
    1, 
    &ret_val);

  NPN_ReleaseVariantValue(&ret_val);
}
