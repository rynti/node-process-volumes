# Process Volumes

API to check how much volume is emitted by other applications.

## Usage

Using this library is as easy as one function call:
`require('process-volumes').getProcessVolumes()`

This returns an array with objects, each object representing an
individual process with attributes for the process ID, process name and
their currently emitted volume.

## Data Structure

The data returned by getProcessVolumes() looks like this:

```json
[
  { processId: 0, processName: '', peakVolume: 0 },
  { processId: 1234, processName: 'example.exe', peakVolume: 0.75 },
  { processId: 1834, processName: 'sample.exe', peakVolume: 0.13 }
]
```

## Example

```js
// This application prints all applications with their currently emitted
// volume and process names.
var processVolumes = require('process-volumes');

var processes = processVolumes.getProcessVolumes();
console.log('Current applications with their volumes:');
processes.forEach(function (process) {
  if (process.processName) {
    console.log(process.processName + ': ' + process.peakVolume);
  } else {
    console.log('(Unknown): ' + process.peakVolume);
  }
});
```

## Support

Currently only a Windows implementation has been created. Contributions
for other operating systems are welcome :)

## License

This project is released under the terms of the MIT License:

```
The MIT License (MIT)

Copyright (c) 2016 Robert Böhm

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
