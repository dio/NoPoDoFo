//
// Created by red on 9/6/17.
//

#ifndef NPDF_PDFMEMDOCUMENT_H
#define NPDF_PDFMEMDOCUMENT_H

#include <boost/filesystem.hpp>
#include <napi.h>
#include <podofo/podofo.h>

#include "ErrorHandler.h"
#include "Page.h"

using namespace boost;
using namespace Napi;
using namespace std;

class Document : public Napi::ObjectWrap<Document>
{
public:
  explicit Document(const CallbackInfo& callbackInfo); // constructor
  ~Document()
  {
    free(_buffer);
    delete _document;
  }
  string originPdf;

  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::HandleScope scope(env);
    Function ctor = DefineClass(
      env,
      "Document",
      { InstanceMethod("load", &Document::Load),
        //        InstanceAccessor("acroForm", &Document::GetAcroForm, nullptr),
        InstanceMethod("getPageCount", &Document::GetPageCount),
        InstanceMethod("getPage", &Document::GetPage),
        InstanceMethod("mergeDocument", &Document::MergeDocument),
        InstanceMethod("deletePage", &Document::DeletePage),
        InstanceAccessor("password", nullptr, &Document::SetPassword),
        //                    InstanceMethod("setPassword",
        //                    &Document::SetPassword),
        InstanceMethod("getVersion", &Document::GetVersion),
        InstanceMethod("isLinearized", &Document::IsLinearized),
        InstanceMethod("getWriteMode", &Document::GetWriteMode),
        InstanceMethod("setEncrypt", &Document::SetEncrypted),
        InstanceMethod("write", &Document::Write) });

    target.Set("Document", ctor);
  }

  Napi::Value Load(const CallbackInfo&);
  Napi::Value GetPageCount(const CallbackInfo&);
  Napi::Value GetPage(const CallbackInfo&);
  void MergeDocument(const CallbackInfo&);
  void DeletePage(const CallbackInfo&);
  void SetPassword(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetVersion(const CallbackInfo&);
  Napi::Value IsLinearized(const CallbackInfo&);
  Napi::Value Write(const CallbackInfo&);
  PoDoFo::PdfMemDocument* GetDocument() { return _document; }
  Napi::Value GetWriteMode(const CallbackInfo&);
  void SetEncrypted(const CallbackInfo&);

private:
  PoDoFo::PdfMemDocument* _document;
  void* _buffer;
};

#endif // NPDF_PDFMEMDOCUMENT_H
