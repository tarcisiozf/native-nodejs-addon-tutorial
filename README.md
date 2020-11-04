# How to write native addons for Node.JS

Firstly, I'd like to begin by trying to convenience you on why you would do that in the first place. 
Due to it's flexibility and safety checks on runtime, in most cases writing JS is easier that writing a C/C++ code.

Well, I can think of two special reasons:

* In case you need to integrate a external library written in C/C++/Go/Rust in your codebase.
* You may want to rewrite some of the modules in C++ for performance reasons, since it has less overhead.

I hope those reasons have convinced you, so lets begin to write some code...

## 1. The native code

For the sake of simplicity, our native code will be a simple function that adds two numbers:

_file: my-func.c_
```cpp
int sum(int a, int b) {
  return a + b;
}
```

## 2. Wrapping the code

Take a look at the comments in this wrapper file:

_wrapper.cpp_
```c++
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

    // This is the wrapper function responsible for bridging the V8 call to our functions
    // After we get the result, we need to wrap it into a V8 value and set as the returned value
    void FuncSum(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Context> ctx = isolate->GetCurrentContext();

        // Fetches the call arguments and converts it into an int
        int a = (int) args[0]->Int32Value(ctx).ToChecked();
        int b = (int) args[1]->Int32Value(ctx).ToChecked();

        // Call our function and store the result
        int result = sum(a, b);

        // Wraps our result inside a V8 value
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
```

## 3. Integrating with Node.JS
#### 3.1. Compilation

First, we need to add a node dependency for us to be able to compile C/C++ code:

```bash
npm install --save-dev node-gyp
```

Now we can configure GYP to compile our wrapper:

_file: binding.gyp_
```json
{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "wrapper.cpp" ]
    }
  ]
}
```

To trigger the compilation we can create a npm script, to do so let's add the following entry in our _package.json_

```json
"scripts": {
  "build": "node-gyp configure build"
  // other scripts...
}
```

Everytime you change a native code you need to call the build script again to generate a compatible code, since we are not going to change the sum function or the wrapper again for now, lets compile it:

```bash
npm run build
```

If this succeeds, it's going to generate a _build_ folder in the root dir.

#### 3.2. Exporting the code

Now we can create a JS file for us to expose the compiled addon:

_file: index.js_
```javascript
module.exports = require('./build/Release/addon');
```
## 4. Demo

And to test and that this works, lets create a small example script:

_file: demo.js_
```javascript
const MyNativeAddon = require('./index');

console.log(MyNativeAddon.sum(30, 12));
// the output should be '42'
```