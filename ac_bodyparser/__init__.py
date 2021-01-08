import ac_bodyparser.impl as LIB


class MultipartBody(object):
    def __init__(self, boundary, body):
        if isinstance(body, str):
            body = body.encode('utf-8')

        self.__body = body
        self.__ptr = LIB.ParseBody(boundary, self.__body)
        self.__chunks = {}

    def delete(self):
        LIB.Delete(self.__ptr)

        self.delete = lambda: None

    def __del__(self):
        self.delete()

    def chunk(self, name):
        if name in self.__chunks:
            return self.__chunks[name]

        chunk_ptr = LIB.GetChunk(self.__ptr, name)

        if chunk_ptr == 0:
            self.__chunks[name] = None

        else:
            self.__chunks[name] = Chunk(chunk_ptr)

        return self.__chunks[name]


class Chunk(object):
    def __init__(self, ptr):
        self.__ptr = ptr
        self.__headers = {}

    def content(self):
        content = LIB.GetChunkContent(self.__ptr)

        self.content = lambda: content

        return content

    def get(self, header):
        values = self.get_all(header)

        if len(values) == 0:
            return None

        return values[0]

    def get_all(self, header):
        header = header.lower()

        if header in self.__headers:
            return headers[header]

        self.__headers[header] = LIB.GetChunkHeaders(self.__ptr, header)

        return self.__headers[header]
