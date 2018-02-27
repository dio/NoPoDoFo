import {existsSync, unlinkSync, writeFile} from 'fs'
import {join} from 'path'
import * as tap from 'tape'
import {Document} from './document'
import {Obj} from "./object";
import {Field} from './field';
import {Painter} from "./painter";
import {Image} from "./image";
import {Page} from "./page";
import {only, Test} from "tape";


const filePath = join(__dirname, '../test-documents/test.pdf'),
    outFile = './test.out.pdf',
    doc = new Document(filePath)
// let page: Page;

doc.on('ready', pdf => {
    let page = pdf.getPage(0)
    const painter = new Painter(doc),
            img = new Image(doc, join(__dirname, '../test-documents/test.jpg'))

        painter.page = page
        painter.drawImage(img, 0, page.height - img.getHeight())
        painter.finishPage()

        doc.write(e => {

            let objs = doc.getObjects()
            for (let i = 0; i < objs.length; i++) {
                let o = objs[i]
                if (o.type === 'Dictionary') {
                    let objDict = o.asObject(),
                        objType = objDict['Type'],
                        objSubType = objDict['SubType']

                    if ((objType && objType.type === 'Name') ||
                        (objSubType && objSubType.type === 'Name')) {

                        if ((objType && objType.asName() === 'XObject') || (objSubType && objSubType.asName() === 'Image')) {
                            let imgObj = o.asObject()['Filter']

                            if (imgObj && imgObj.type === 'Array') {
                                const imgObjArr = imgObj.asArray()
                                if (imgObjArr.length === 1) {
                                    if (imgObjArr[0].type === 'Name') {
                                    // if (imgObj.asArray().at(0).type === 'Name') {
                                        // if (imgObj.asArray().at(0).asName() === 'DCTDecode') {
                                        if (imgObjArr[0].asName() === 'DCTDecode') {
                                            extractImg(o, true)
                                            return
                                        }
                                    }
                                }
                            }
                            else if (imgObj && imgObj.type === 'Name' && imgObj.asName() === 'DCTDecode') {
                                extractImg(o, true)
                                return
                            }
                            else {
                                extractImg(o, false)
                                return
                            }
                        }
                    }
                }
            }
        }, './img.out.pdf')

        function extractImg(obj: Obj, jpg: Boolean) {
            let ext = jpg ? '.jpg' : '.ppm'
            writeFile(`/tmp/test.img.extract.${ext}`, obj.stream, err => {
                if (err instanceof Error)
                    // t.fail()
                // t.assert(existsSync(`/tmp/test.img.extract.${ext}`) === true)
                if (existsSync('./img.out.pdf')) unlinkSync('./img.out.pdf')
                // t.end()
            })
        }
})
let page:Page
// tap('Page Api', sub => {
//      doc.on('ready', e => {
//         page = doc.getPage(0)
//         runAll(sub)
//     })
// })


function pageRotation(test:Function) {

    test('set page rotation', (t:Test) => {
        const originalRotation = page.rotation,
            newRotation = originalRotation + 90
        page.rotation = newRotation
        doc.write(e => {
            const testDoc = new Document(outFile)
            testDoc.on('ready', e => {
                if (e instanceof Error) t.fail()
                const testPage = testDoc.getPage(0)
                t.assert(testPage.rotation === newRotation, 'Page rotation updated')
                unlinkSync(outFile)
                t.end()
            })
        }, outFile)
    })
}

function pageProperties(test:Function) {
    test('page instance properties', (t:Test) => {
        t.assert(page.number === 1, 'page number valid')
        t.assert(page.width > 0, 'page width valid')
        t.assert(page.height > 0, 'page height valid')
        t.end()
    })
}

function pageTrimBox(test:Function) {
    test('get trimbox', (t:Test) => {
        const trimBox = page.trimBox
        t.assert(trimBox.height > 0, 'trimbox height is not null')
        t.assert(trimBox.width > 0, 'trimbox width not null')
        t.assert(trimBox.left === 0, 'trimbox left')
        t.assert(trimBox.bottom === 0, 'trimbox bottom')
        t.end()
    })
}

function pageFields(test:Function) {
    test('get number of fields', (t:Test) => {
        let n = page.getNumFields()
        t.assert(typeof n === 'number')
        // t.assert(page.getNumFields() === 22, 'page get number of fields counts all fields')
        t.end()
    })
}

function pageFieldInfoArray(test:Function) {
    test('get fields info', (t:Test) => {
        try {
            const fields = page.getFieldsInfo()

            // t.assert(fields.length === 22, 'page get field info get\'s info for each field')
            t.assert(fields instanceof Array)
            t.ok(fields)
            t.assert(fields.length > 0)
            t.end()
        }
        catch (e) {
            console.log(e)
            t.fail()
        }

    })
}

function pageGetField(test:Function) {
    test('get field', (t:Test) => {
        const field = page.getField(0)

        t.assert(field instanceof Field, 'is an instance of Field')
        t.end()
    })
}

function pageGetFields(test:Function) {
    test('get fields', (t:Test) => {
        const fields = page.getFields()
        t.assert(Array.isArray(fields), 'returns an array')
        t.assert(fields.length === page.getNumFields(), 'is not an empty array')
        t.assert(fields[0] instanceof Field, 'contains instance(s) of Field')
        t.end()
    })
}

function pageGetAnnotsCount(test:Function) {
    test('page number of annotations', (t:Test) => {
        t.assert(page.getNumAnnots() === 22, 'get\'s all annotations on the page')
        t.end()
    })
}

function pageGetAnnot(test:Function) {
    test('page get annotation', (t:Test) => {
        const annot = page.getAnnotation(0)
        t.ok(annot)
        t.assert(annot.getType() === 'Widget')
        t.end()
    })
}

function pageContents(test:Function) {
    test('page contents', (t:Test) => {
        const contents = page.getContents(false)
        t.assert(contents instanceof Obj, 'is an instance of Obj')
        t.assert(contents.length > 0, 'content is not null or empty')
        t.end()
    })
}

function pageResources(test:Function) {
    test('page resources', (t:Test) => {
        const resources = page.getResources()
        t.assert(resources instanceof Obj, 'is instance ob Obj')
        t.assert(resources.length > 0, 'is not null or empty')
        t.end()
    })
}

function pageAddImg(test:Function) {
    test('add image', (t:Test) => {
        const painter = new Painter(doc),
            img = new Image(doc, join(__dirname, '../test-documents/test.jpg'))

        painter.page = page
        painter.drawImage(img, 0, page.height - img.getHeight())
        painter.finishPage()

        doc.write(e => {

            let objs = doc.getObjects()
            for (let i = 0; i < objs.length; i++) {
                let o = objs[i]
                if (o.type === 'Dictionary') {
                    let objDict = o.asObject(),
                        objType = objDict['Type'],
                        objSubType = objDict['SubType']

                    if ((objType && objType.type === 'Name') ||
                        (objSubType && objSubType.type === 'Name')) {

                        if ((objType && objType.asName() === 'XObject') || (objSubType && objSubType.asName() === 'Image')) {
                            let imgObj = o.asObject()['Filter']

                            if (imgObj && imgObj.type === 'Array') {
                                const imgObjArr = imgObj.asArray()
                                if (imgObjArr.length === 1) {
                                    if (imgObjArr[0].type === 'Name') {
                                    // if (imgObj.asArray().at(0).type === 'Name') {
                                        // if (imgObj.asArray().at(0).asName() === 'DCTDecode') {
                                        if (imgObjArr[0].asName() === 'DCTDecode') {
                                            extractImg(o, true)
                                            return
                                        }
                                    }
                                }
                            }
                            else if (imgObj && imgObj.type === 'Name' && imgObj.asName() === 'DCTDecode') {
                                extractImg(o, true)
                                return
                            }
                            else {
                                extractImg(o, false)
                                return
                            }
                        }
                    }
                }
            }
        }, './img.out.pdf')

        function extractImg(obj: Obj, jpg: Boolean) {
            let ext = jpg ? '.jpg' : '.ppm'
            writeFile(`/tmp/test.img.extract.${ext}`, obj.stream, err => {
                if (err instanceof Error)
                    t.fail()
                t.assert(existsSync(`/tmp/test.img.extract.${ext}`) === true)
                if (existsSync('./img.out.pdf')) unlinkSync('./img.out.pdf')
                t.end()
            })
        }
    })
}

function runTest(a: Function, b:Test) {
    setImmediate(() => {
        global.gc()
        a(b.test)
    })
}

export function runAll(test:Test) {
    [
        pageRotation,
        pageProperties,
        pageTrimBox,
        pageFields,
        pageFieldInfoArray,
        pageGetField,
        pageGetFields,
        pageGetAnnotsCount,
        pageGetAnnot,
        pageContents,
        pageResources,
        pageAddImg
    ].map(i => runTest(i, test))
}

