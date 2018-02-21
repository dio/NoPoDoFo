import {IDocument, __mod} from './document'
import {Obj, NPDFInternal} from "./object";

export class Form {

    get needAppearances() {return this._instance.needAppearances}
    set needAppearances(value:boolean) {this._instance.needAppearances = value}

    constructor(private _instance: NPDFInternal) { }

    getObject():Obj {
        return new Obj(this._instance.getObject())
    }
}