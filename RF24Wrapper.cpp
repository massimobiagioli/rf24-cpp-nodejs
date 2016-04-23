#include "RF24Wrapper.hpp"

namespace RF24Addon {

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Array;

#define DEFAULT_RADIO_CHANNEL 0x4C
#define DEFAULT_GPIO_CE_PIN 25
#define DEFAULT_GPIO_IRQ_PIN 0
#define DEFAULT_PAYLOAD_SIZE 10
#define DEFAULT_LOOP_DELAY 1000
#define DEFAULT_RETRIES_DELAY 15
#define DEFAULT_RETRIES_COUNT 15
#define DEFAULT_AUTO_ACK 0
#define DEFAULT_DATA_RATE 0

#define STATE_INIT 0
#define STATE_RX 1
#define STATE_SWITCH_DEVICE 2

#define TX_MSG_OK "OK"
#define TX_MSG_KO "KO"

Persistent<Function> RF24Wrapper::constructor;

RF24Wrapper::RF24Wrapper() {
}

RF24Wrapper::~RF24Wrapper() {
}

void RF24Wrapper::Init(Local<Object> exports) {    
  Isolate* isolate = Isolate::GetCurrent();
  
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "RF24Wrapper"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  // Map Methods
  NODE_SET_PROTOTYPE_METHOD(tpl, "initRadio", InitRadio);
  NODE_SET_PROTOTYPE_METHOD(tpl, "loop", Loop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "tx", Tx);
  
  // Map Getter/Setter
  NODE_SET_PROTOTYPE_METHOD(tpl, "getChannel", GetChannel);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setChannel", SetChannel);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getGpioCEPin", GetGpioCEPin);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setGpioCEPin", SetGpioCEPin);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getGpioIRQPin", GetGpioIRQPin);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setGpioIRQPin", SetGpioIRQPin);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPayloadSize", GetPayloadSize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setPayloadSize", SetPayloadSize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getLoopDelay", GetLoopDelay);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setLoopDelay", SetLoopDelay);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getRetriesDelay", GetRetriesDelay);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setRetriesDelay", SetRetriesDelay);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getRetriesCount", GetRetriesCount);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setRetriesCount", SetRetriesCount);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getAutoAck", GetAutoAck);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setAutoAck", SetAutoAck);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getDataRate", GetDataRate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setDataRate", SetDataRate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getReadPipes", GetReadPipes);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setReadPipes", SetReadPipes);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getWritePipes", GetWritePipes);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setWritePipes", SetWritePipes);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "RF24Wrapper"), tpl->GetFunction());
}

void RF24Wrapper::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.IsConstructCall()) {    
    RF24Wrapper* obj = new RF24Wrapper();	          
    RF24Wrapper::SetDefaultValue(obj);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {    
    const int argc = 0;
    Local<Value> argv[] = {};
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void RF24Wrapper::SetDefaultValue(RF24Wrapper* obj) {  
  obj->channel_ = DEFAULT_RADIO_CHANNEL;
  obj->gpioCEPin_ = DEFAULT_GPIO_CE_PIN;
  obj->gpioIRQPin_ = DEFAULT_GPIO_IRQ_PIN;
  obj->payloadSize_ = DEFAULT_PAYLOAD_SIZE;
  obj->loopDelay_ = DEFAULT_LOOP_DELAY;
  obj->retriesDelay_ = DEFAULT_RETRIES_DELAY;
  obj->retriesCount_ = DEFAULT_RETRIES_COUNT;
  obj->autoAck_ = DEFAULT_AUTO_ACK;
  obj->dataRate_ = DEFAULT_DATA_RATE;
  obj->machineState_ = STATE_INIT;
  obj->currentDevice_ = 0;
}

// -------------------------------------
// Methods
// -------------------------------------

void RF24Wrapper::InitRadio(const FunctionCallbackInfo<Value>& args) {      
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());      
  obj->radio_ = new RF24(obj->gpioCEPin_, obj->gpioIRQPin_);        
  obj->radio_->begin();      
  
  obj->radio_->setRetries(obj->retriesDelay_, obj->retriesCount_);  
  obj->radio_->setChannel(obj->channel_);
  obj->radio_->setAutoAck(obj->autoAck_);    
  
  switch (obj->dataRate_) {
    case 0:
      obj->radio_->setDataRate(RF24_1MBPS);
      break;
    case 1:
      obj->radio_->setDataRate(RF24_2MBPS);
      break;                
    default:
      obj->radio_->setDataRate(RF24_250KBPS);
      break;  
  }    
   
  obj->radio_->startListening();             
  obj->radio_->printDetails();    
}

void RF24Wrapper::Loop(const FunctionCallbackInfo<Value>& args) {  
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());      
  Isolate* isolate = args.GetIsolate();
  char request[obj->payloadSize_];  
  const int argc = 1;
  Local<Function> rxCallback = Local<Function>::Cast(args[0]);
    
  while(1) {
    switch (obj->machineState_) {
      
      case STATE_INIT:
        obj->radio_->openWritingPipe(obj->writePipes_[obj->currentDevice_]);
        obj->radio_->openReadingPipe(1, obj->readPipes_[obj->currentDevice_]);
        obj->machineState_ = STATE_RX;        
        break;
      
      case STATE_RX:        
        if (obj->radio_->available()) {
          obj->radio_->read(request, obj->payloadSize_);        
          Local<Value> argv[argc] = { String::NewFromUtf8(isolate, request) };
          rxCallback->Call(Null(isolate), argc, argv);                    
        }                            
        break;
            
      case STATE_SWITCH_DEVICE:        
        if (obj->currentDevice_ < (obj->nReadPipes_ - 1)) {
          (obj->currentDevice_)++;
        } else {
          obj->currentDevice_ = 0;
        }
        obj->machineState_ = STATE_INIT;
        delay(obj->loopDelay_);
        break;        
    }                
  }  
}

void RF24Wrapper::Tx(const FunctionCallbackInfo<Value>& args) {      
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());      
  bool status;   
  const int argc = 1;
  const char *callbackReturnMsg;
  Local<Function> txCallback = Local<Function>::Cast(args[1]);  
  
  obj->radio_->stopListening();  
  
  v8::String::Utf8Value responseObj(args[0]->ToString());
  const char *response = *responseObj;
    
  status = obj->radio_->write(response, obj->payloadSize_);
  if (status) {    
    callbackReturnMsg = TX_MSG_OK;
  } else {
    callbackReturnMsg = TX_MSG_KO;
  }
  Local<Value> argv[argc] = { String::NewFromUtf8(isolate, callbackReturnMsg) };
  txCallback->Call(Null(isolate), argc, argv);  
  
  obj->radio_->startListening();  
  obj->machineState_ = STATE_SWITCH_DEVICE;                
}

// -------------------------------------
// Getter/Setter
// -------------------------------------

void RF24Wrapper::GetChannel(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->channel_));
}

void RF24Wrapper::SetChannel(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->channel_ = args[0]->NumberValue();
}

void RF24Wrapper::GetGpioCEPin(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->gpioCEPin_));
}

void RF24Wrapper::SetGpioCEPin(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->gpioCEPin_ = args[0]->NumberValue();
}

void RF24Wrapper::GetGpioIRQPin(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->gpioIRQPin_));
}

void RF24Wrapper::SetGpioIRQPin(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->gpioIRQPin_ = args[0]->NumberValue();
}

void RF24Wrapper::GetPayloadSize(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->payloadSize_));
}

void RF24Wrapper::SetPayloadSize(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->payloadSize_ = args[0]->NumberValue();
}

void RF24Wrapper::GetLoopDelay(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->loopDelay_));
}

void RF24Wrapper::SetLoopDelay(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->loopDelay_ = args[0]->NumberValue();
}

void RF24Wrapper::GetRetriesDelay(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->retriesDelay_));
}

void RF24Wrapper::SetRetriesDelay(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->retriesDelay_ = args[0]->NumberValue();
}

void RF24Wrapper::GetRetriesCount(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->retriesCount_));
}

void RF24Wrapper::SetRetriesCount(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->retriesCount_ = args[0]->NumberValue();
}

void RF24Wrapper::GetAutoAck(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->autoAck_));
}

void RF24Wrapper::SetAutoAck(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->autoAck_ = args[0]->NumberValue();
}

void RF24Wrapper::GetDataRate(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());  
  args.GetReturnValue().Set(Number::New(isolate, obj->dataRate_));
}

void RF24Wrapper::SetDataRate(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  obj->dataRate_ = args[0]->NumberValue();
}

void RF24Wrapper::GetReadPipes(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  Local<Array> pipes = Array::New(isolate);
  for (unsigned int i = 0; i < obj->nReadPipes_; i++ ) {        
    pipes->Set(i, Number::New(isolate, obj->readPipes_[i]));
  }
  args.GetReturnValue().Set(pipes);  
}

void RF24Wrapper::SetReadPipes(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  Local<Array> pipes = Local<Array>::Cast(args[0]);  
  obj->nReadPipes_ = pipes->Length();    
  obj->readPipes_ = new uint64_t[obj->nReadPipes_];
  for (unsigned int i = 0; i < obj->nReadPipes_; i++) {
    obj->readPipes_[i] = (uint64_t) (pipes->Get(i)->NumberValue());    
  }      
}

void RF24Wrapper::GetWritePipes(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  Local<Array> pipes = Array::New(isolate);
  for (unsigned int i = 0; i < obj->nWritePipes_; i++ ) {        
    pipes->Set(i, Number::New(isolate, obj->writePipes_[i]));
  }
  args.GetReturnValue().Set(pipes);
}

void RF24Wrapper::SetWritePipes(const FunctionCallbackInfo<Value>& args) {
  RF24Wrapper* obj = ObjectWrap::Unwrap<RF24Wrapper>(args.Holder());
  Local<Array> pipes = Local<Array>::Cast(args[0]);    
  obj->nWritePipes_ = pipes->Length();    
  obj->writePipes_ = new uint64_t[obj->nWritePipes_];
  for (unsigned int i = 0; i < obj->nWritePipes_; i++) {
    obj->writePipes_[i] = (uint64_t) (pipes->Get(i)->NumberValue());    
  }      
}

}