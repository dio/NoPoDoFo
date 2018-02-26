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

#include "Rect.h"


namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

FunctionReference Rect::constructor; // NOLINT

Rect::Rect(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() == 1 && info[0].Type() == napi_external) {
    const PdfRect* rectPtr = info[0].As<External<PdfRect>>().Data();
    rect = new PdfRect(*rectPtr);
  }
  else if (info.Length() == 4) {
    double left, bottom, width, height;
    for (uint8_t i = 0; i < info.Length(); i++) {
      if (!info[i].IsNumber()) {
        throw Napi::Error::New(info.Env(),
                               "Rect requires (number, number, number, number) "
                               "as constructor parameters.");
      }
    }
    left = info[0].As<Number>();
    bottom = info[1].As<Number>();
    width = info[2].As<Number>();
    height = info[3].As<Number>();
    rect = new PoDoFo::PdfRect(left, bottom, width, height);
  }
}
Rect::~Rect()
{
  if (rect != nullptr) {
    HandleScope scope(Env());
    delete rect;
    rect = nullptr;
  }
}
void
Rect::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Rect",
    { InstanceAccessor("left", &Rect::GetLeft, &Rect::SetLeft),
      InstanceAccessor("bottom", &Rect::GetBottom, &Rect::SetBottom),
      InstanceAccessor("width", &Rect::GetWidth, &Rect::SetWidth),
      InstanceAccessor("height", &Rect::GetHeight, &Rect::SetHeight),
      InstanceMethod("intersect", &Rect::Intersect)

    });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Rect", ctor);
}
void
Rect::Intersect(const CallbackInfo& info)
{
  if (info.Length() != 1) {
    throw Napi::Error::New(info.Env(),
                           "Intersect requires a single argument of type Rect");
  }
  auto rectObj = info[0].As<Object>();
  rect->Intersect(Rect::Unwrap(rectObj)->GetRect());
}
Napi::Value
Rect::GetWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect->GetWidth());
}
Napi::Value
Rect::GetHeight(const CallbackInfo& info)
{

  return Napi::Number::New(info.Env(), rect->GetHeight());
}
Napi::Value
Rect::GetLeft(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect->GetLeft());
}
Napi::Value
Rect::GetBottom(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect->GetBottom());
}
void
Rect::SetWidth(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double width = value.As<Number>();
  rect->SetWidth(width);
}
void
Rect::SetHeight(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double height = value.As<Number>();
  rect->SetHeight(height);
}
void
Rect::SetLeft(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double left = value.As<Number>();
  rect->SetLeft(left);
}
void
Rect::SetBottom(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double bottom = value.As<Number>();
  rect->SetBottom(bottom);
}
}
