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

#ifndef NPDF_TEXTFIELD_H
#define NPDF_TEXTFIELD_H

#include "../ValidateArguments.h"
#include "Field.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <string>

namespace NoPoDoFo {
using namespace std;
class TextField : public Napi::ObjectWrap<TextField>
{
public:
  static Napi::FunctionReference constructor;
  explicit TextField(const Napi::CallbackInfo&);
  ~TextField();
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetText(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetText(const Napi::CallbackInfo&);

  unique_ptr<PoDoFo::PdfTextField> GetField()
  {
    return make_unique<PoDoFo::PdfTextField>(
      *new PoDoFo::PdfTextField(field->GetField()));
  }

private:
  Field* field;
};
}
#endif // NPDF_TEXTFIELD_H
