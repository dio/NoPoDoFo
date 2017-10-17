import { Obj } from './object';
import { Page } from './page';
import { Encrypt, EncryptInitOption } from './encrypt';

export const __mod = require('../build/Release/npdf.node')

export interface IDocument {
    _instance: any
    encrypt: Encrypt

    load(file: string, update?: boolean): void

    getPageCount(): number

    getPage(pageN: number): Page

    getObjects(): Array<Obj>

    mergeDocument(doc: string): void

    deletePage(pageIndex: number): void

    getVersion(): number

    isLinearized(): boolean

    write(file: string): void

    createEncrypt(option: EncryptInitOption): Encrypt

    getTrailer():Obj

    freeObjMem(target:Obj): void
}

/**
 * @class Document
 * @example
 * import {Document} from 'npdf'
 * const doc = new Document('/path/to/file')
 * const pageCount = doc.getPageCount()
 * const page = doc.getPage(0) // pages are zero indexed
 * const mergeDocs = doc.mergeDocument('/path/to/other/doc')
 * doc.setPassword('secret')
 * doc.write('/file/destination')
 * const fileBuffer = fs.readFile('/file/destination', (e,d) => e ? e : d)
 * // do something with the file or the file buffer
 */
export class Document implements IDocument {

    _instance: any
    private _loaded: boolean = false;
    private _pageCount: number
    private _password: string

    get pageCount() {
        return this._pageCount
    }

    set password(value: string) {
        this._password = value;
    }

    get password() {
        throw Error("Can not get password from loaded document")
    }

    set encrypt(instance: Encrypt) {
        if (instance.option) this._instance.encrypt = instance.option
        else {
            throw Error("Set document encrypt with an instance of Encrypt with the optional EncryptInitOption defined at construction")
        }
    }

    get encrypt() {
        const instance = this._instance.encrypt
        return new Encrypt(instance)
    }
    /**
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @param update
     * @returns void
     */
    constructor(file: string, update: boolean = false) {
        this._instance = new __mod.Document()
        if (file) {
            this._instance.load(file, update)
            this._pageCount = this._instance.getPageCount()
            this._loaded = true
        }
    }

    /**
     * @description load pdf file
     * @param {string} file - pdf file path
     * @param update
     */
    load(file: string, update: boolean = false): void {
        this._instance.load(file, update)
        this._loaded = true
    }

    getPageCount(): number {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.getPageCount()
    }

    getPage(pageN: number): Page {
        if (pageN > this.pageCount || pageN < 0) {
            throw new RangeError("pageN out of range")
        }
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        const page: Page = this._instance.getPage(pageN)
        return new Page(page);
    }

    getObjects(): Array<Obj> {
        const objects: Array<any> = this._instance.getObjects()
        return objects.map(value => {
            return new Obj(value)
        })
    }

    /**
     * @description Append doc to the end of the loaded doc
     * @param {string} doc - pdf file path
     * @param password
     */
    mergeDocument(doc: string, password?: string): void {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        password !== null ? this._instance.mergeDocument(doc, password) : this._instance.mergeDocument(doc)
    }

    deletePage(pageIndex: number): void {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        if (this.pageCount < pageIndex || pageIndex < 0) {
            throw new RangeError('page index out of range')
        }
        this._instance.deletePage(pageIndex)
    }

    getVersion(): number {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.getVersion()
    }

    isLinearized(): boolean {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.isLinearized()
    }


    write(file: string): void {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        this._instance.write(file)
    }

    createEncrypt(option: EncryptInitOption): Encrypt {
        const encryptInstance = this._instance.createEncrypt(option)
        return new Encrypt(encryptInstance)
    }

    getTrailer(): Obj {
        let objInit =  this._instance.getTrailer()
        return new Obj(objInit)
    }

    freeObjMem(target:Obj): void {
        this._instance.freeObjMem(target._instance)
    }
}