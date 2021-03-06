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
import {Obj} from "./object";
import {Document} from './document'


export class Ref {

    get objectNumber() {
        return this._instance.objectNumber
    }

    set objectNumber(value: number) {
        this._instance.objectNumber = value
    }

    get generation() {
        return this._instance.generation
    }

    set(value: number) {
        this._instance.generation = value
    }

    constructor(private _instance: any) {
    }

    write(output: string): void {
        if (output === null) throw Error('output must be a valid output file path')
        this._instance.write(output)
    }

    /**
     * Check if ref points to an indirect object. Object is indirect if both object and generation numbers are not 0
     */
    isIndirect(): boolean {
        return this._instance.isIndirect()
    }

    toString(): string {
        return this._instance.toString()
    }

    /**
     * @desc If Ref is an indirect object (most likely) you can "de-reference" the reference to an object.
     * @todo: This should be possible without needing to pass the document
     * @todo Replace error message with link to documentation
     * @param {Document} document - the document that contains the reference
     * @returns {Obj}
     */
    deRef(document:Document): Obj {
        if(document instanceof Document === false) {
            throw TypeError('Ref.deRef requires a Document instance. For more information please see the docs')
        }
        const instance = this._instance.getObject((document as any)._instance)
        return new Obj(instance)
    }

}