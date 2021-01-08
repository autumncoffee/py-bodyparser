#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ac-library/http/utils/multipart.hpp>
#include <ac-common/utils/string.hpp>
#include <string>
#include <vector>
#include <cstdint>

namespace NACPyBodyParser {
    using namespace NAC;

    using TBodyParts = std::vector<NHTTP::TBodyPart>;

    PyObject* ParseBody(PyObject*, PyObject* args) {
        const char* boundary;
        const char* content;
        Py_ssize_t contentLength;

        if (!PyArg_ParseTuple(args, "ss#", &boundary, &content, &contentLength)) {
            return nullptr;
        }

        return Py_BuildValue("K", (uintptr_t)new TBodyParts(NHTTPUtils::ParseParts(boundary, contentLength, content)));
    }

    PyObject* Delete(PyObject*, PyObject* args) {
        unsigned long long int ptr;

        if (!PyArg_ParseTuple(args, "K", &ptr)) {
            return nullptr;
        }

        delete (TBodyParts*)ptr;

        Py_RETURN_NONE;
    }

    namespace {
        NHTTP::TBodyPart* FindChunk(TBodyParts& list, const std::string& name) {
            for (auto&& part : list) {
                const auto& params = part.ContentDispositionParams();
                auto it = params.find(std::string("name"));

                if (it == params.end()) {
                    continue;
                }

                if (it->second == name) {
                    return &part;
                }
            }

            return nullptr;
        }
    }

    PyObject* GetChunk(PyObject*, PyObject* args) {
        unsigned long long int ptr;
        const char* chunkName;

        if (!PyArg_ParseTuple(args, "Ks", &ptr, &chunkName)) {
            return nullptr;
        }

        return Py_BuildValue("K", (uintptr_t)FindChunk(*(TBodyParts*)ptr, chunkName));
    }

    PyObject* GetChunkContent(PyObject*, PyObject* args) {
        unsigned long long int ptr;

        if (!PyArg_ParseTuple(args, "K", &ptr)) {
            return nullptr;
        }

        auto& chunk = *(NHTTP::TBodyPart*)ptr;

        return Py_BuildValue("y#", chunk.Content(), chunk.ContentLength());
    }

    PyObject* GetChunkHeaders(PyObject*, PyObject* args) {
        unsigned long long int ptr;
        const char* headerName_;

        if (!PyArg_ParseTuple(args, "Ks", &ptr, &headerName_)) {
            return nullptr;
        }

        std::string headerName(headerName_);
        NStringUtils::ToLower(headerName);

        const auto* values = ((NHTTP::TBodyPart*)ptr)->HeaderValues(headerName);

        if (values) {
            auto* rv = PyList_New(values->size());

            for (size_t i = 0; i < values->size(); ++i) {
                PyList_SetItem(rv, i, Py_BuildValue("s", values->at(i).c_str()));
            }

            return rv;
        }

        return PyList_New(0);
    }
}

static PyMethodDef ACPyBodyParserFuncs[] = {
    {"ParseBody", (PyCFunction)NACPyBodyParser::ParseBody, METH_VARARGS, ""},
    {"Delete", (PyCFunction)NACPyBodyParser::Delete, METH_VARARGS, ""},
    {"GetChunk", (PyCFunction)NACPyBodyParser::GetChunk, METH_VARARGS, ""},
    {"GetChunkContent", (PyCFunction)NACPyBodyParser::GetChunkContent, METH_VARARGS, ""},
    {"GetChunkHeaders", (PyCFunction)NACPyBodyParser::GetChunkHeaders, METH_VARARGS, ""},

    {nullptr, nullptr, 0, nullptr}
};

static PyModuleDef ACPyBodyParserModule = {
    PyModuleDef_HEAD_INIT,
    "impl",
    nullptr,
    -1,
    ACPyBodyParserFuncs
};

PyMODINIT_FUNC PyInit_impl(void) {
    return PyModule_Create(&ACPyBodyParserModule);
}
