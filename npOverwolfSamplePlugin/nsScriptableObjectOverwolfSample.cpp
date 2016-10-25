/*
  Overwolf Sample Plugin
  Copyright (c) 2014 Ovefwolf Ltd.
*/
#include "nsScriptableObjectOverwolfSample.h"
#include "utils/Thread.h"

#define REGISTER_METHOD(name, method) { \
  methods_[NPN_GetStringIdentifier(name)] = &method; \
}

const char kAPropretyName[] = "sampleProperty";

nsScriptableObjectOverwolfSample::nsScriptableObjectOverwolfSample(NPP npp) :
  nsScriptableObjectBase(npp),
  sample_property_(0.0),
  shutting_down_(false) {
}

nsScriptableObjectOverwolfSample::~nsScriptableObjectOverwolfSample(void) {
  shutting_down_ = true;
  
  if (thread_.get()) {
    thread_->Stop();
  }
}

bool nsScriptableObjectOverwolfSample::Init() {
  REGISTER_METHOD("echo", nsScriptableObjectOverwolfSample::Echo);
  REGISTER_METHOD("add", nsScriptableObjectOverwolfSample::Add);

  sample_property_ = 42;

  thread_.reset(new utils::Thread());
  return thread_->Start();
}

bool nsScriptableObjectOverwolfSample::HasMethod(NPIdentifier name) {
#ifdef _DEBUG
  NPUTF8* name_utf8 = NPN_UTF8FromIdentifier(name);
  NPN_MemFree((void*)name_utf8);
#endif

  // does the method exist?
  return (methods_.find(name) != methods_.end());
}

bool nsScriptableObjectOverwolfSample::Invoke(
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
bool nsScriptableObjectOverwolfSample::HasProperty(NPIdentifier name) {
  return (NPN_GetStringIdentifier(kAPropretyName) == name);
}

// NOTE: you could also implement SetProperty which receives a callback
// function (as an NPObject) and then you can trigger this callback
// with nsScriptableObjectOverwolfSample::TriggerCallbackOnMainThread.
// This allows you to implement a "global" event for the plugin
bool nsScriptableObjectOverwolfSample::GetProperty(
  NPIdentifier name, NPVariant *result) {

  DOUBLE_TO_NPVARIANT(sample_property_, *result);
  return true;
}

bool nsScriptableObjectOverwolfSample::SetProperty(
  NPIdentifier name, const NPVariant *value) {
  NPN_SetException(this, "this property is read-only!");
  return true;
}


/************************************************************************/
/* Public methods
/************************************************************************/
bool nsScriptableObjectOverwolfSample::Echo(
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
      &nsScriptableObjectOverwolfSample::EchoTask, 
      this, 
      message, 
	  caption,
	  offset,
      NPVARIANT_TO_OBJECT(args[3])));
}

bool nsScriptableObjectOverwolfSample::Add(
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
      &nsScriptableObjectOverwolfSample::AddTask, 
      this,  
      NPVARIANT_TO_OBJECT(args[0])));
}

bool nsScriptableObjectOverwolfSample::FinishReading(
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
      &nsScriptableObjectOverwolfSample::FinishReadingTask, 
      this, 
      NPVARIANT_TO_OBJECT(args[0])));
}
/************************************************************************/
/* Separate thrad implementations for public functions
/************************************************************************/
// this is not called on the main thread - so we need to 
// use NPN_PluginThreadAsyncCall
void nsScriptableObjectOverwolfSample::EchoTask(
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

	//current addr is 1D1B8FB 
	/*
	<?xml version="1.0" encoding="utf-8"?>
<CheatTable>
  <CheatEntries>
    <CheatEntry>
      <ID>1</ID>
      <Description>"No description"</Description>
      <LastState Value="0" RealAddress="7FF79FB6B8FB"/>
      <VariableType>Byte</VariableType>
      <Address>"Gw2-64.exe"+1D1B8FB</Address>
    </CheatEntry>
  </CheatEntries>
</CheatTable>
*/
/*
	<?xml version="1.0" encoding="utf-8"?>
<CheatTable>
  <CheatEntries>
    <CheatEntry>
      <ID>0</ID>
      <Description>"No description"</Description>
      <LastState Value="64" RealAddress="7FF79FB6B8FB"/>
      <VariableType>Byte</VariableType>
      <Address>"Gw2-64.exe"+1D1B8FB</Address>
    </CheatEntry>
  </CheatEntries>
</CheatTable>
*/
  INT32_TO_NPVARIANT(InitReadMemory(message.c_str(), caption.c_str(), offset),arg);
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

void nsScriptableObjectOverwolfSample::AddTask(
  NPObject* callback) {
  if (shutting_down_) {
    return;
  }

  NPVariant arg;
  NPVariant ret_val;

  DOUBLE_TO_NPVARIANT(, arg);

  // fire callback
  NPN_InvokeDefault(
    __super::npp_, 
    callback, 
    &arg, 
    1, 
    &ret_val);

  NPN_ReleaseVariantValue(&ret_val);
}

void nsScriptableObjectOverwolfSample::FinishReadingTask(
  NPObject* callback) {
  if (shutting_down_) {
    return;
  }

  FinishReadingMemory();
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
