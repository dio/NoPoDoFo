#include "Form.h"
#include "../base/Obj.h"


namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

FunctionReference Form::constructor;

Form::Form(const CallbackInfo& info)
  : ObjectWrap<Form>(info)
  , form(make_unique<PdfAcroForm>(*info[0].As<External<PdfAcroForm>>().Data()))
{
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
  GetForm().SetNeedAppearances(value.As<Boolean>());
}

Napi::Value
Form::GetNeedAppearances(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetForm().GetNeedAppearances());
}

Napi::Value
Form::GetObject(const CallbackInfo& info)
{
  auto obj = GetForm().GetObject();
  auto nObj = External<PdfObject>::New(info.Env(), obj);
  auto objInstance = Obj::constructor.New({ nObj });
  return objInstance;
}

}
