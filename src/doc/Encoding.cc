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

#include "Encoding.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Dictionary.h"
#include "Font.h"

using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference Encoding::constructor; // NOLINT

Encoding::Encoding(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , font(Font::Unwrap(info[0].As<Object>()))
{
}
Encoding::~Encoding()
{
  HandleScope scope(Env());
  font = nullptr;
}
void
Encoding::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Encoding",
    { InstanceMethod("addToDictionary", &Encoding::AddToDictionary),
      InstanceMethod("convertToUnicode", &Encoding::ConvertToUnicode),
      InstanceMethod("convertToEncoding", &Encoding::ConvertToEncoding),
      InstanceMethod("data", &Encoding::GetData) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Encoding", ctor);
}
Napi::Value
Encoding::AddToDictionary(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto wrap = info[0].As<Object>();
  Dictionary* d = Dictionary::Unwrap(wrap);
  PdfDictionary dict = d->GetDictionary();
  GetEncoding()->AddToDictionary(dict);
  return info.Env().Undefined();
}
Napi::Value
Encoding::ConvertToUnicode(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_object });
  string content = info[0].As<String>().Utf8Value();
  Font* font = Font::Unwrap(info[1].As<Object>());
  PdfString buffer =
    GetEncoding()->ConvertToUnicode(PdfString(content), font->GetPoDoFoFont());
  return String::New(info.Env(), buffer.GetStringUtf8());
}
Napi::Value
Encoding::ConvertToEncoding(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_object });
  string content = info[0].As<String>().Utf8Value();
  Font* font = Font::Unwrap(info[1].As<Object>());
  PdfRefCountedBuffer buffer =
    GetEncoding()->ConvertToEncoding(PdfString(content), font->GetPoDoFoFont());
  return String::New(info.Env(), buffer.GetBuffer());
}
Napi::Value
Encoding::GetData(const Napi::CallbackInfo& info)
{
  return info.Env().Undefined();
}
}
