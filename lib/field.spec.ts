import {unlink} from 'fs'
import {join} from 'path'
import * as test from 'tape'
import {Document} from './document'
import {CheckBox, Field, TextField} from './field'

const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf'
    const doc = new Document(filePath),
        page = doc.getPage(0),
        field = page.getField(0)
test('create field instance from existing pdf field', t => {
    t.assert(field !== null)
    t.assert(field instanceof Field)
    console.log(field.getType())
    t.assert(field.getType() === 'TextField')
    t.end()
})
test('Can instantiate a TextField given a field of type TextField', t => {
    const text = new TextField(field)
    t.ok(text)
    t.assert(text.text === '')
    t.end()
})
test('Can instantiate a Checkbox given a field of type CheckBox', t => {
    const fields = page.getFieldsInfo()
    let index = -1
    for (let i = 0; i < fields.length; i++) {
        let type = fields[i].type
        if (type === 'CheckBox') {
            index = i
            break
        }
    }
    const field = new Field(page, index)
    t.assert(field.getType() === 'CheckBox')

    const checkbox = new CheckBox(field)

    t.ok(checkbox)
    t.assert(checkbox.checked === false)
    t.end()
})
