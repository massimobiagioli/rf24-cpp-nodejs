#include <node.h>
#include "RF24Wrapper.hpp"

namespace RF24Addon {

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports) {
	RF24Wrapper::Init(exports);
}

NODE_MODULE(RF24Addon, InitAll)

}	// end namespace 