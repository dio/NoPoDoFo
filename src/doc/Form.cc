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

#include "Form.h"
#include "../base/Obj.h"


namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

FunctionReference Form::constructor; // NOLINT

Form::Form(const CallbackInfo& info)
  : ObjectWrap<Form>(info)
{
  doc = Document::Unwrap(info[0].As<Object>());
  if(!doc->GetDocument()->GetAcroForm(false)) {
    throw Error::New(info.Env(), "Acroform does not exist on document");
  }
}

Form::~Form() {
  HandleScope scope(Env());
  doc = nullptr;
}

void
Form::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(env,
                              "Form",
                              { InstanceMethod("getObject", &Form::GetObject),
                                InstanceAccessor("needAppearances",
                                                 &Form::GetNeedAppearances,
                                                 &Form::SetNeedAppearances) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Form", ctor);
}
void
Form::SetNeedAppearances(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsEmpty()) {
    throw Error::New(info.Env(), "value required");
  }
  if (!value.IsBoolean()) {
    throw Error::New(info.Env(), "requires boolean value type");
  }
  GetForm()->SetNeedAppearances(value.As<Boolean>());
}

Napi::Value
Form::GetNeedAppearances(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetForm()->GetNeedAppearances());
}

Napi::Value
Form::GetObject(const CallbackInfo& info)
{
  EscapableHandleScope scope(info.Env());
  auto obj = GetForm()->GetObject();
  auto nObj = External<PdfObject>::New(info.Env(), obj);
  auto objInstance = Obj::constructor.New({ nObj });
  return scope.Escape(objInstance);
}

}
