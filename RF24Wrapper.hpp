#ifndef RF24_WRAPPER_H
#define RF24_WRAPPER_H

#include <node.h>
#include <node_object_wrap.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdint.h>
#include <RF24/RF24.h>

namespace RF24Addon {

class RF24Wrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  explicit RF24Wrapper();
  ~RF24Wrapper();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);    
  
  // Methods
  static void InitRadio(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Loop(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Tx(const v8::FunctionCallbackInfo<v8::Value>& args);  
    
  // Getter/Setter
  static void GetChannel(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetChannel(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetGpioCEPin(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetGpioCEPin(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetGpioIRQPin(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetGpioIRQPin(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetPayloadSize(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetPayloadSize(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetLoopDelay(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetLoopDelay(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetRetriesDelay(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetRetriesDelay(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetRetriesCount(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetRetriesCount(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetAutoAck(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetAutoAck(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetDataRate(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetDataRate(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetReadPipes(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetReadPipes(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetWritePipes(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetWritePipes(const v8::FunctionCallbackInfo<v8::Value>& args);
  
  static v8::Persistent<v8::Function> constructor;
  
  // Utils
  static void SetDefaultValue(RF24Wrapper* obj);
  
  // Private
  int channel_;
  int gpioCEPin_;
  int gpioIRQPin_;
  int payloadSize_;
  int loopDelay_;
  int retriesDelay_;
  int retriesCount_;
  int autoAck_;
  int dataRate_;
  unsigned int nReadPipes_;
  unsigned int nWritePipes_;
  unsigned int machineState_;
  unsigned int currentDevice_;
  uint64_t* readPipes_;
  uint64_t* writePipes_;
  RF24* radio_;
};

}  

#endif