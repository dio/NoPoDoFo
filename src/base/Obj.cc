/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 *
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Obj.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Vector.h"

namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

FunctionReference Obj::constructor; // NOLINT

void
Obj::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Obj",
    { InstanceAccessor("stream", &Obj::GetStream, nullptr),
      InstanceAccessor("type", &Obj::GetDataType, nullptr),
      InstanceAccessor("length", &Obj::GetObjectLength, nullptr),
      InstanceAccessor("immutable", &Obj::GetImmutable, &Obj::SetImmutable),
      InstanceMethod("hasStream", &Obj::HasStream),
      InstanceMethod("getOffset", &Obj::GetByteOffset),
      InstanceMethod("write", &Obj::Write),
      InstanceMethod("flateCompressStream", &Obj::FlateCompressStream),
      InstanceMethod("delayedStreamLoad", &Obj::DelayedStreamLoad),
      InstanceMethod("getBool", &Obj::GetBool),
      InstanceMethod("getNumber", &Obj::GetNumber),
      InstanceMethod("getReal", &Obj::GetReal),
      InstanceMethod("getString", &Obj::GetString),
      InstanceMethod("getName", &Obj::GetName),
      InstanceMethod("getRawData", &Obj::GetRawData),
      InstanceMethod("clear", &Obj::Clear),
      InstanceMethod("eq", &Obj::Eq) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Obj", ctor);
}

Obj::Obj(const Napi::CallbackInfo& info)
  : ObjectWrap<Obj>(info)
{
  if (info[0].Type() == napi_external) {
    obj = info[0].As<External<PdfObject>>().Data();
  } else if (info[0].IsObject() &&
             info[0].As<Object>().InstanceOf(Vector::constructor.Value()) &&
             info.Length() >= 2 && info[1].IsNumber()) {
    vector = Vector::Unwrap(info[0].As<Object>());
    objnum = info[1].As<Number>();
    if (info.Length() == 3 && info[2].IsNumber()) {
      gennum = info[2].As<Number>().Uint32Value();
    }
  }
}

Obj::~Obj()
{
  HandleScope scope(Env());
  if (obj != nullptr)
    delete obj;
  obj = nullptr;
  vector = nullptr;
}

void
Obj::Clear(const Napi::CallbackInfo& info)
{
  try {
    GetObject()->Clear();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Obj::GetStream(const CallbackInfo& info)
{
  try {
    auto pStream = dynamic_cast<PdfMemStream*>(GetObject()->GetStream());
    auto stream = pStream->Get();
    auto length = pStream->GetLength();
    auto value =
      Buffer<char>::Copy(info.Env(), stream, static_cast<size_t>(length));
    return value;
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

Napi::Value
Obj::HasStream(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetObject()->HasStream());
}

Napi::Value
Obj::GetObjectLength(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(),
                           GetObject()->GetObjectLength(ePdfWriteMode_Default));
}

Napi::Value
Obj::GetImmutable(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetObject()->GetImmutable());
}
void
Obj::SetImmutable(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsBoolean()) {
    try {
      GetObject()->SetImmutable(value.As<Boolean>());
    } catch (PdfError& err) {
      ErrorHandler(err, info);
    }
  }
}

Napi::Value
Obj::GetDataType(const CallbackInfo& info)
{
  string js;
  if (GetObject()->IsArray()) {
    js = "Array";
  } else if (GetObject()->IsBool()) {
    js = "Boolean";
  } else if (GetObject()->IsDictionary()) {
    js = "Dictionary";
  } else if (GetObject()->IsEmpty()) {
    js = "Empty";
  } else if (GetObject()->IsHexString()) {
    js = "HexString";
  } else if (GetObject()->IsNull()) {
    js = "Null";
  } else if (GetObject()->IsNumber()) {
    js = "Number";
  } else if (GetObject()->IsName()) {
    js = "Name";
  } else if (GetObject()->IsRawData()) {
    js = "RawData";
  } else if (GetObject()->IsReal()) {
    js = "Real";
  } else if (GetObject()->IsReference()) {
    js = "Reference";
  } else if (GetObject()->IsString()) {
    js = "String";
  } else {
    js = "Unknown";
  }
  return Napi::String::New(info.Env(), js);
}

Napi::Value
Obj::Eq(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_object });
  auto wrap = info[0].As<Object>();
  if (!wrap.InstanceOf(Obj::constructor.Value())) {
    throw Error::New(info.Env(), "Must be an instance of NoPoDoFo Obj");
  }
  auto value = Obj::Unwrap(wrap);
  return Boolean::New(info.Env(), value->GetObject() == this->GetObject());
}

void
Obj::FlateCompressStream(const CallbackInfo& info)
{
  try {
    GetObject()->FlateCompressStream();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Obj::DelayedStreamLoad(const CallbackInfo& info)
{
  try {
    GetObject()->DelayedStreamLoad();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Obj::GetNumber(const CallbackInfo& info)
{
  if (!GetObject()->IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a number");
  }
  return Number::New(info.Env(), GetObject()->GetNumber());
}

Napi::Value
Obj::GetReal(const CallbackInfo& info)
{
  if (!GetObject()->IsReal()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a number");
  }

  return Number::New(info.Env(), GetObject()->GetReal());
}

Napi::Value
Obj::GetString(const CallbackInfo& info)
{
  if (!GetObject()->IsString()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a String");
  }

  return String::New(info.Env(), GetObject()->GetString().GetStringUtf8());
}

Napi::Value
Obj::GetName(const CallbackInfo& info)
{
  if (!GetObject()->IsName()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a string");
  }
  try {
    string name = GetObject()->GetName().GetName();
    return String::New(info.Env(), name);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

Napi::Value
Obj::GetBool(const CallbackInfo& info)
{
  if (!GetObject()->IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj not accessible as a boolean");
  }
  return Boolean::New(info.Env(), GetObject()->GetBool());
}

Napi::Value
Obj::GetRawData(const CallbackInfo& info)
{
  if (!GetObject()->IsRawData()) {
    throw Napi::Error::New(info.Env(), "Obj not accessible as a buffer");
  }
  string data = GetObject()->GetRawData().data();
  return Buffer<char>::Copy(info.Env(), data.c_str(), data.length());
}

class ObjOffsetAsync : public Napi::AsyncWorker
{
public:
  ObjOffsetAsync(Napi::Function& cb, Obj* obj, string arg)
    : Napi::AsyncWorker(cb)
    , obj(obj)
    , arg(std::move(arg))
  {
  }

protected:
  void Execute() override
  {
    try {
      auto o = const_cast<PdfObject*>(obj->GetObject());
      value = o->GetByteOffset(arg.c_str(), ePdfWriteMode_Default);
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Napi::Error& err) {
      SetError(err.Message());
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), Napi::Number::New(Env(), value) });
  }

private:
  Obj* obj;
  string arg;
  long value = -1;
};

Napi::Value
Obj::GetByteOffset(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_function });
  string arg = info[0].As<String>().Utf8Value();
  auto cb = info[1].As<Function>();
  ObjOffsetAsync* worker = new ObjOffsetAsync(cb, this, arg);
  worker->Queue();
  return info.Env().Undefined();
}

class ObjWriteAsync : public Napi::AsyncWorker
{
public:
  ObjWriteAsync(Napi::Function& cb, Obj* obj, string dest)
    : AsyncWorker(cb)
    , obj(obj)
    , arg(std::move(dest))
  {
  }

protected:
  void Execute() override
  {
    try {
      PdfOutputDevice device(arg.c_str());
      obj->GetObject()->WriteObject(&device, ePdfWriteMode_Default, nullptr);
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Napi::Error& err) {
      SetError(err.Message());
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), arg) });
  }

private:
  Obj* obj;
  string arg;
};

Napi::Value
Obj::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_function });
  auto cb = info[1].As<Function>();
  ObjWriteAsync* worker =
    new ObjWriteAsync(cb, this, info[0].As<String>().Utf8Value());
  worker->Queue();
  return info.Env().Undefined();
}
}
