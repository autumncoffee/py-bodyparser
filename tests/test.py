#!/usr/bin/env python

from ac_bodyparser import MultipartBody

boundary = '9dfe29a1d8d5ccaec52e1ca1589f0e02'
partName = 'test_part'
headerName = 'X-Custom-Header'
headerValue = 'header+value'
data = '''
Lorem ipsum dolor sit amet, consectetur adipiscing
elit, sed do eiusmod tempor incididunt ut labore et
dolore magna aliqua. Ut enim ad minim veniam, quis
nostrud exercitation ullamco laboris nisi ut aliquip
ex ea commodo consequat. Duis aute irure dolor in
reprehenderit in voluptate velit esse cillum dolore
eu fugiat nulla pariatur. Excepteur sint occaecat
cupidatat non proident, sunt in culpa qui officia
deserunt mollit anim id est laborum.
'''

rawBody = f'--{boundary}\r\nContent-Disposition: form-data; name="{partName}"; filename="{partName}"\r\n{headerName}: {headerValue}\r\n\r\n{data}\r\n--{boundary}--\r\n'

body = MultipartBody(boundary, rawBody)
chunk = body.chunk(f'"{partName}"')

assert chunk is not None, f'Chunk {partName} must exist'

assert chunk.content().decode('utf-8') == data, 'Chunk content must match expectations'

assert chunk.get(headerName) == headerValue, f'Value of header {headerName} must match expectations'

body.delete() # This frees memory!

print('OK')
