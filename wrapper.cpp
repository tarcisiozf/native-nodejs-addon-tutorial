// We need to import V8's headers (V8 is the virtual machine that node uses to run javascript)
#include <node.h>
// Lets import our function
#include "./my-func.c"

namespace native_addon_example {
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::Value;
    using v8::Context;
    using v8::Exception;
    using v8::Number;

    // This is the wrapper function responsible to bridge all the V8 call arguments to our "sum" function
    // After we get the result, we need to wrap it in a v8 value and set as the returned value
    void FuncSum(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Context> ctx = isolate->GetCurrentContext();

        // Fetches the call arguments and convert it into a int
        int a = (int) args[0]->Int32Value(ctx).ToChecked();
        int b = (int) args[1]->Int32Value(ctx).ToChecked();

        // Call our function and store the result
        int result = sum(a, b);

        // Wraps our result inside a v8 value
        Local<Value> output = Number::New(isolate, result);

        // Set as the returned value
        args.GetReturnValue().Set(output);
    }

    // In here we can bind a name to our wrapper function, in this case "sum" is how the js code will find it
    void init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "sum", FuncSum);
    }

    // Exposes all methods of this node module
    NODE_MODULE(NODE_GYP_MODULE_NAME, init)
}