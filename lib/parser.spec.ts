import {ContentsTokenizer} from './parser'
import { join } from 'path';
import { Document } from './document';
import * as test from 'tape'

const filePath = join(__dirname, '../test-documents/test.pdf'),
    outFile = './test.out.pdf',
    doc = new Document(filePath)

test('get contents as string, contents tokenizer', t => {
    doc.on('ready', e => {
        if(e instanceof Error) t.fail()
        const page = doc.getPage(0),
            tokenizer = new ContentsTokenizer(page, doc)
        
        const pageContents = tokenizer.readAllContent()
        t.assert(pageContents.length === 155) // fix this
        t.end()
    })
    .on('error', e => t.fail('failed to parse contents'))
})

