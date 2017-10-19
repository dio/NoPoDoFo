import { Obj } from "./object";

/**
 * @class Arr wraps PdfArray
 * The underlying pdf array is using vector<PdfObject>
 * The methods declared on Arr are methods PoDoFo has optimized for
 * Pdf array's.
 * If you would like to use Arr as an instance of a javascript array call ToArray()
 */
export class Arr {
    dirty: boolean;
    private _array: Array<Obj>;

    get length() {
        return this._instance.length
    }

    set length(value: number) {
        throw Error("Can not set length")
    }

    constructor(public _instance: any) { }

    /**
     * Converts PdfArray to javascript array.
     * Any modifications to javascript array will NOT be persisted to PdfArray.
     * Use for lookup only
     * @returns {Array<Obj>}
     */
    toArray(): Array<Obj> {
        const init: Array<any> = this._instance.toArray()
        this._array = init.map(i => new Obj(i))

        const proxy = new Proxy(this, {
            get(target, prop) {
                if (prop instanceof Number || typeof prop === 'number') {
                    if (prop > -1 && prop < target._array.length)
                        return target._array[prop]
                }
                else return target._array
            },
            set(target, prop, value) {
                let success = false
                target.push(value)
                target._array.push(value)
                return success
            }
        })
        return proxy._array
    }

    push(value: Obj): void {
        this._instance.push(value)
    }

    /**
     * Search PdfArray for key
     * @param {string} key
     * @returns {boolean}
     */
    contains(key: string): boolean {
        return this._instance.contains(key)
    }

    /**
     * Find index of key
     * @param {string} key
     * @returns {number}
     */
    indexOf(key: string): number {
        return this._instance.indexOf(key)
    }

    /**
     * Writes the array to a file. Useful for debugging
     * @param {string} output
     */
    write(output: string): void {
        this._instance.write(output)
    }

    /**
     * Retrieve the PdfObject at index
     * @param {number} index
     * @returns {Obj}
     */
    at(index: number): Obj {
        const item = this._instance.getIndex(index)
        return new Obj(item)
    }
}