//
// Created by red on 9/11/17.
//

#ifndef NPDF_FORM_H
#define NPDF_FORM_H

#include "Document.h"

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Form : public Napi::ObjectWrap<Form>
{
public:
  explicit Form(const Napi::CallbackInfo&);
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env&, Napi::Object&);
  void SetNeedAppearances(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetNeedAppearances(const Napi::CallbackInfo&);
  Napi::Value GetObject(const Napi::CallbackInfo&);
  PoDoFo::PdfAcroForm& GetForm() { return *form; }

private:
  std::unique_ptr<PoDoFo::PdfAcroForm> form;
};
}
#endif
