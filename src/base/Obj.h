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

#ifndef NPDF_OBJ_H
#define NPDF_OBJ_H

#include "Vector.h"

#include <napi.h>
#include <podofo/podofo.h>

#include <utility>

using namespace std;

namespace NoPoDoFo {
class Obj : public Napi::ObjectWrap<Obj>
{
public:
  explicit Obj(const Napi::CallbackInfo&);
  ~Obj();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetStream(const Napi::CallbackInfo&);
  Napi::Value HasStream(const Napi::CallbackInfo&);
  Napi::Value GetObjectLength(const Napi::CallbackInfo&);
  Napi::Value GetDataType(const Napi::CallbackInfo&);
  Napi::Value GetByteOffset(const Napi::CallbackInfo&);
  Napi::Value Write(const Napi::CallbackInfo&);
  void FlateCompressStream(const Napi::CallbackInfo&);
  void DelayedStreamLoad(const Napi::CallbackInfo&);
  Napi::Value GetNumber(const Napi::CallbackInfo&);
  Napi::Value GetReal(const Napi::CallbackInfo&);
  Napi::Value GetString(const Napi::CallbackInfo&);
  Napi::Value GetName(const Napi::CallbackInfo&);
  Napi::Value GetBool(const Napi::CallbackInfo&);
  Napi::Value GetRawData(const Napi::CallbackInfo&);
  Napi::Value GetImmutable(const Napi::CallbackInfo&);
  void SetImmutable(const Napi::CallbackInfo&, const Napi::Value&);
  void Clear(const Napi::CallbackInfo&);
  Napi::Value Eq(const Napi::CallbackInfo&);

  PoDoFo::PdfObject* GetObject()
  {
    return obj == nullptr ? vector->GetVector().GetObject(
                              PoDoFo::PdfReference(objnum, gennum))
                          : obj;
  }

private:
  PoDoFo::PdfObject* obj = nullptr;
  Vector* vector = nullptr;
  PoDoFo::pdf_objnum objnum = 0;
  PoDoFo::pdf_gennum gennum = 0;
};
}
#endif
