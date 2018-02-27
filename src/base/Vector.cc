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

#include "Vector.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../doc/Document.h"
#include "Obj.h"
#include "Ref.h"

namespace NoPoDoFo {

using namespace std;
using namespace Napi;
using namespace PoDoFo;

FunctionReference Vector::constructor;

Vector::Vector(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() && info[0].IsObject()) {
    document = Document::Unwrap(info[0].As<Object>());
  } else {
    vector = new PdfVecObjects();
  }
}

Vector::~Vector()
{
  HandleScope scope(Env());
  if (vector != nullptr)
    delete vector;
  vector = nullptr;
  document = nullptr;
}

void
Vector::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Vector",
    { InstanceAccessor(
        "document", &Vector::GetParentDocument, &Vector::SetParentDocument),
      InstanceAccessor("reuseObjectNumbers",
                       &Vector::GetCanReuseObjectNumbers,
                       &Vector::SetCanReuseObjectNumbers),
      InstanceMethod("setAutoDelete", &Vector::SetAutoDelete),
      InstanceMethod("getSize", &Vector::GetSize),
      InstanceMethod("getCount", &Vector::GetObjectCount),
      InstanceMethod("getObject", &Vector::GetObject),
      InstanceMethod("getIndex", &Vector::GetIndex),
      InstanceMethod("createObject", &Vector::CreateObject),
      InstanceMethod("removeObject", &Vector::RemoveObject),
      InstanceMethod("GC", &Vector::CollectGarbage),
      InstanceMethod("addFreeObject", &Vector::AddFreeObject),
      InstanceMethod("getFreeObjects", &Vector::GetFreeObjects),
      InstanceMethod("push", &Vector::Push),
      InstanceMethod("sort", &Vector::Sort),
      InstanceMethod("createStream", &Vector::CreateStream),
      InstanceMethod("finish", &Vector::Finish) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Vector", ctor);
}

Napi::Value
Vector::GetParentDocument(const Napi::CallbackInfo&)
{
  return document->Value();
}
void
Vector::SetParentDocument(const Napi::CallbackInfo& info,
                          const Napi::Value& value)
{

  if (!value.IsObject() ||
      !value.As<Napi::Object>().InstanceOf(Document::constructor.Value())) {
    throw Error::New(info.Env(), "must be an instance of Document");
  }
  auto d = Document::Unwrap(value.As<Object>());
  GetVector().SetParentDocument(d->GetDocument());
  document = d;
}
void
Vector::SetAutoDelete(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_boolean });
  GetVector().SetAutoDelete(info[0].As<Boolean>());
}
void
Vector::SetCanReuseObjectNumbers(const Napi::CallbackInfo& info,
                                 const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Error::New(info.Env(), "value must be a boolean");
  }
  GetVector().SetCanReuseObjectNumbers(value.As<Boolean>());
}
Napi::Value
Vector::GetCanReuseObjectNumbers(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetVector().GetCanReuseObjectNumbers());
}
void
Vector::Clear(const Napi::CallbackInfo& info)
{
  try {
    GetVector().Clear();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Vector::GetSize(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetVector().GetSize());
}
Napi::Value
Vector::GetObjectCount(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetVector().GetObjectCount());
}
Napi::Value
Vector::GetObject(const Napi::CallbackInfo& info)
{
  PdfReference ref;
  if (info[0].IsObject() &&
      info[0].As<Object>().InstanceOf(Ref::constructor.Value())) {
    ref = Ref::Unwrap(info[0].As<Object>())->GetRef();
  } else {
    AssertFunctionArgs(info, 1, { napi_number });
    const pdf_objnum i = info[0].As<Number>();
    const pdf_gennum g = static_cast<const pdf_gennum>(
      info.Length() == 2 && info[1].IsNumber() ? info[1].As<Number>() : 0);
    PdfReference ref(i, g);
  }
  return Obj::constructor.New(
    { External<PdfObject>::New(info.Env(), GetVector().GetObject(ref)) });
}
Napi::Value
Vector::GetIndex(const Napi::CallbackInfo& info)
{
  PdfReference ref = Ref::Unwrap(info[0].As<Object>())->GetRef();
  return Number::New(info.Env(), GetVector().GetIndex(ref));
}
Napi::Value
Vector::CreateObject(const Napi::CallbackInfo& info)
{
  EscapableHandleScope scope(info.Env());
  auto v = info[0];
  PdfObject* o;
  if (v.IsBoolean()) {
    PdfVariant var(v.As<Boolean>());
    o = new PdfObject(var);
  } else if (v.IsNumber()) {
    PdfVariant var(v.As<Number>().Int64Value());
    o = new PdfObject(var);
  } else if (v.IsString()) {
    PdfVariant var(v.As<String>());
    o = new PdfObject(var);
  } else {
    throw Error::New(info.Env(),
                     "PdfObject supports boolean, integer, and string types");
  }
  return scope.Escape(
    Obj::constructor.New({ External<PdfObject>::New(info.Env(), o) }));
}
void
Vector::RemoveObject(const Napi::CallbackInfo& info)
{
  PdfReference ref = Ref::Unwrap(info[0].As<Object>())->GetRef();
  GetVector().RemoveObject(ref);
}
void
Vector::CollectGarbage(const Napi::CallbackInfo& info)
{
  PdfObject* target = Obj::Unwrap(info[0].As<Object>())->GetObject();
  GetVector().CollectGarbage(target);
}

void
Vector::AddFreeObject(const CallbackInfo& info)
{
  PdfReference ref = Ref::Unwrap(info[0].As<Object>())->GetRef();
  GetVector().AddFreeObject(ref);
}

Value
Vector::GetFreeObjects(const CallbackInfo& info)
{
  EscapableHandleScope scope(info.Env());
  auto js = Array::New(info.Env());
  auto list = GetVector().GetFreeObjects();
  uint32_t i = 0;
  for (auto& it : list) {
    PdfObject o(it);
    js.Set(i,
           Obj::constructor.New({ External<PdfObject>::New(info.Env(), &o) }));
    ++i;
  }
  return scope.Escape(js);
}

Value
Vector::Push(const CallbackInfo& info)
{
  PdfObject* o = Obj::Unwrap(info[0].As<Object>())->GetObject();
  try {
    GetVector().push_back(o);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
  return this->Value();
}

Value
Vector::Sort(const CallbackInfo& info)
{
}

Value
Vector::CreateStream(const CallbackInfo& info)
{
}

Value
Vector::Finish(const CallbackInfo& info)
{
}
}
