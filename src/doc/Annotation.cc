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

#include "Annotation.h"
#include "../ErrorHandler.h"

namespace NoPoDoFo {

FunctionReference Annotation::constructor; // NOLINT

Annotation::Annotation(const CallbackInfo& info)
  : ObjectWrap(info)
  , page(Page::Unwrap(info[0].As<Object>()))
  , annotIndex(info[1].As<Number>())
{
}
Annotation::~Annotation()
{
  HandleScope scope(Env());
  page = nullptr;
}
void
Annotation::SetFlags(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::TypeError::New(
      info.Env(), "SetFlag must be an instance of NpdfAnnotationType");
  }
  int jsValue = info[0].As<Number>();
  auto flag = static_cast<PoDoFo::EPdfAnnotationFlags>(jsValue);
  GetAnnotation()->SetFlags(flag);
}

Napi::Value
Annotation::GetFlags(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetAnnotation()->GetFlags());
}

Napi::Value
Annotation::HasAppearanceStream(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetAnnotation()->HasAppearanceStream());
}

void
Annotation::SetBorderStyle(const CallbackInfo& info)
{
  if (info.Length() != 3) {
    throw Napi::Error::New(info.Env(),
                           "Border requires three arguments of type number."
                           "Number - horizontal corner radius"
                           "Number - vertical corner radius"
                           "Number - width");
  }
  double horizontal = info[0].As<Number>();
  double vertical = info[1].As<Number>();
  double width = info[2].As<Number>();
  GetAnnotation()->SetBorderStyle(horizontal, vertical, width);
}

void
Annotation::SetTitle(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsString()) {
    throw Napi::TypeError::New(
      info.Env(), "SetTitle requires a single argument of type string.");
  }
  try {
    string title = info[0].As<String>().Utf8Value();
    GetAnnotation()->SetTitle(PdfString(title));
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PoDoFo PdfError: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

Napi::Value
Annotation::GetTitle(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), GetAnnotation()->GetTitle().GetString());
}

void
Annotation::SetContent(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsEmpty()) {
    throw Napi::Error::New(info.Env(),
                           "SetContent requires string \"value\" argument.");
  }
  string content = value.As<String>().Utf8Value();
  GetAnnotation()->SetContents(content);
}

Napi::Value
Annotation::GetContent(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), GetAnnotation()->GetContents().GetString());
}

void
Annotation::SetDestination(const CallbackInfo& info, const Napi::Value& value)
{
  throw Napi::Error::New(info.Env(), "Not implemented.");
}

Napi::Value
Annotation::GetDestination(const CallbackInfo& info)
{
  throw Napi::Error::New(info.Env(), "Not implemented.");
}

Value
Annotation::HasDestination(const CallbackInfo& info)
{
  throw Napi::Error::New(info.Env(), "Not implemented.");
}

void
Annotation::SetAction(const CallbackInfo& info)
{
  auto actionObj = info[0].As<Object>();
  auto o = info[1].As<Object>();
  if (!o.InstanceOf(Document::constructor.Value())) {
    throw Error::New(info.Env(), "Requires instance of Document");
  }
  auto doc = Document::Unwrap(o)->GetDocument();
  int type = actionObj.Get("type").As<Number>();
  string uri = actionObj.Get("uri").As<String>().Utf8Value();
  auto flag = static_cast<PoDoFo::EPdfAction>(type);
  try {
    PdfAction action(flag, doc);
    action.SetURI(uri);
    GetAnnotation()->SetAction(action);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Annotation::GetAction(const CallbackInfo& info)
{
  PdfAction* currentAction = GetAnnotation()->GetAction();
  if (currentAction->HasScript()) {
    return Napi::String::New(info.Env(),
                             currentAction->GetScript().GetString());
  } else if (currentAction->HasURI()) {
    return Napi::String::New(info.Env(), currentAction->GetURI().GetString());
  } else {
    throw Napi::Error::New(info.Env(), "Action not set");
  }
}

Napi::Value
Annotation::HasAction(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetAnnotation()->HasAction());
}

void
Annotation::SetOpen(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::Error::New(info.Env(), "Requires Boolean type");
  }
  GetAnnotation()->SetOpen(value.As<Boolean>());
}

Napi::Value
Annotation::GetOpen(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetAnnotation()->GetOpen());
}

void
Annotation::SetColor(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsArray()) {
    auto jsValue = value.As<Array>();
    int rgb[3];
    for (uint8_t i = 0; i < jsValue.Length(); i++) {
      rgb[i] = jsValue.Get(i).As<Number>();
    }
    GetAnnotation()->SetColor(rgb[0], rgb[1], rgb[2]);
  } else {
    throw Napi::TypeError::New(info.Env(),
                               "Requires RGB color: [Number, Number, Number]");
  }
}

Napi::Value
Annotation::GetColor(const CallbackInfo& info)
{
  auto rgbArray = Napi::Array::New(info.Env());
  auto pdfRgb = GetAnnotation()->GetColor();
  if (pdfRgb.size() != 3) {
  }
  const double r = pdfRgb[0].GetNumber();
  const double g = pdfRgb[1].GetNumber();
  const double b = pdfRgb[2].GetNumber();
  rgbArray.Set(1, Napi::Number::New(info.Env(), r));
  rgbArray.Set(2, Napi::Number::New(info.Env(), g));
  rgbArray.Set(3, Napi::Number::New(info.Env(), b));
  return rgbArray;
}

Napi::Value
Annotation::GetType(const CallbackInfo& info)
{
  string jsType;
  switch (GetAnnotation()->GetType()) {
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_3D: {
      jsType = "3D";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Caret: {
      jsType = "Caret";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Circle: {
      jsType = "Circle";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_FileAttachement: {
      jsType = "FileAttachment";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_FreeText: {
      jsType = "FreeText";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Highlight: {
      jsType = "HighLight";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Ink: {
      jsType = "Ink";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Line: {
      jsType = "Line";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Link: {
      jsType = "Link";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Movie: {
      jsType = "Movie";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_PolyLine: {
      jsType = "PolyLine";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Polygon: {
      jsType = "Polygon";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Popup: {
      jsType = "Popup";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_PrinterMark: {
      jsType = "PrintMark";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_RichMedia: {
      jsType = "RichMedia";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Screen: {
      jsType = "Screen";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Sound: {
      jsType = "Sound";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Square: {
      jsType = "Square";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Squiggly: {
      jsType = "Squiggly";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Stamp: {
      jsType = "Stamp";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_StrikeOut: {
      jsType = "StikeOut";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Text: {
      jsType = "Text";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_TrapNet: {
      jsType = "TrapNet";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Unknown: {
      jsType = "Unknown";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Underline: {
      jsType = "Underline";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Watermark: {
      jsType = "Watermark";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_WebMedia: {
      jsType = "WebMedia";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Widget: {
      jsType = "Widget";
      break;
    }
  }
  return Napi::String::New(info.Env(), jsType);
}

void
Annotation::SetQuadPoints(const CallbackInfo& info, const Napi::Value& value)
{
  throw Error::New(info.Env(), "unimplemented");
}

Napi::Value
Annotation::GetQuadPoints(const CallbackInfo& info)
{
  return info.Env().Undefined();
}

void
Annotation::SetFileAttachment(const CallbackInfo& info)
{
  throw Error::New(info.Env(), "unimplemented");
}

Napi::Value
Annotation::HasFileAttachment(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetAnnotation()->HasFileAttachement());
}

}
