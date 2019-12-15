#include "PbConvertor.h"

bool PbConvertor::struct2Pb(const char *&pStruct, google::protobuf::Message *pPb)
{
    if (NULL == pStruct || NULL == pPb)
        return false;

    const google::protobuf::Descriptor *pDescriptor = pPb->GetDescriptor();
    if (NULL == pDescriptor)
        return false;
    const google::protobuf::Reflection *pReflection =pPb->GetReflection();
    if (NULL == pReflection)
        return false;

    for (size_t i = 0; i < pDescriptor->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor *pFieldDescriptor = pDescriptor->field(i);
        if (NULL == pFieldDescriptor)
            return false;

        switch (pFieldDescriptor->cpp_type())
        {
#define _CONVERT(type, pbType, setMethod, addMethod) \
            case google::protobuf::FieldDescriptor::pbType: \
            { \
                if (pFieldDescriptor->is_repeated()) \
                { \
                    if (i > 0) \
                    { \
                        size_t size; \
                        if (getSize(pReflection, pPb, pDescriptor->field(i - 1), size)) \
                        { \
                            if (size > 0) \
                            { \
                                type *addr = *(type **)pStruct; \
                                if (NULL != addr) \
                                { \
                                    for (size_t j = 0; j < size; ++j) \
                                        pReflection->addMethod(pPb, pFieldDescriptor, *(addr + j)); \
                                } \
                            } \
                        } \
                    } \
                    pStruct += sizeof(void *); \
                } \
                else \
                { \
                    if (google::protobuf::FieldDescriptor::CPPTYPE_STRING != pFieldDescriptor->cpp_type() || NULL != *(type *)pStruct) \
                        pReflection->setMethod(pPb, pFieldDescriptor, *(type *)pStruct); \
                    pStruct += sizeof(type); \
                } \
                break; \
            }

            _CONVERT(double, CPPTYPE_DOUBLE, SetDouble, AddDouble)
            _CONVERT(float, CPPTYPE_FLOAT, SetFloat, AddFloat)
            _CONVERT(int32_t, CPPTYPE_INT32, SetInt32, AddInt32)
            _CONVERT(int64_t, CPPTYPE_INT64, SetInt64, AddInt64)
            _CONVERT(uint32_t, CPPTYPE_UINT32, SetUInt32, AddUInt32)
            _CONVERT(uint64_t, CPPTYPE_UINT64, SetUInt64, AddUInt64)
            _CONVERT(bool, CPPTYPE_BOOL, SetBool, AddBool)
            _CONVERT(char *, CPPTYPE_STRING, SetString, AddString)
            _CONVERT(int, CPPTYPE_ENUM, SetEnumValue, AddEnumValue)
#undef _CONVERT

            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            {
                if (pFieldDescriptor->is_repeated())
                {
                    if (i > 0)
                    {
                        size_t size;
                        if (getSize(pReflection, pPb, pDescriptor->field(i - 1), size))
                        {
                            if (size > 0)
                            {
                                const char *addr = *(char **)pStruct;
                                for (size_t j = 0; j < size; ++j)
                                {
                                    if (!struct2Pb(addr, pReflection->AddMessage(pPb, pFieldDescriptor)))
                                        return false;
                                }
                            }
                        }
                    }
                    pStruct += sizeof(void *);
                }
                else
                {
                    if (!struct2Pb(pStruct, pReflection->MutableMessage(pPb, pFieldDescriptor)))
                        return false;
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }
    return true;
}

bool PbConvertor::struct2serializedPb(const char *pStruct, const std::string &pbTypeName, char *&pSerializedPb, size_t &serializedPbSize)
{
    if (NULL == pStruct)
    {
        pSerializedPb = NULL;
        return false;
    }

    const google::protobuf::Descriptor *pDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(pbTypeName);
    if (NULL == pDescriptor)
        return false;
    const google::protobuf::Message *pPrototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
    if (NULL == pPrototype)
        return false;
    google::protobuf::Message *pPb = pPrototype->New();
    if (NULL == pPb)
        return false;

    bool bRet = false;
    if (struct2Pb(pStruct, pPb))
    {
        serializedPbSize = pPb->ByteSizeLong();
        pSerializedPb = new char[serializedPbSize];
        if (pPb->SerializeToArray(pSerializedPb, serializedPbSize))
            bRet = true;
        else
        {
            delete [] pSerializedPb;
            pSerializedPb = NULL;
        }
    }
    delete pPb;
    return bRet;
}

bool PbConvertor::pb2struct(const google::protobuf::Message *pPb, MemTree &stru)
{
    if (NULL == pPb)
        return false;

    const google::protobuf::Descriptor *pDescriptor = pPb->GetDescriptor();
    if (NULL == pDescriptor)
        return false;
    const google::protobuf::Reflection *pReflection =pPb->GetReflection();
    if (NULL == pReflection)
        return false;

    for (size_t i = 0; i < pDescriptor->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor *pFieldDescriptor = pDescriptor->field(i);
        if (NULL == pFieldDescriptor)
            return false;

        switch (pFieldDescriptor->cpp_type())
        {
#define _CONVERT(type, pbType, getMethod, getRepeatedMethod) \
            case google::protobuf::FieldDescriptor::pbType: \
            { \
                if (pFieldDescriptor->is_repeated()) \
                { \
                    size_t newMemSize = stru.memSize + sizeof(void *); \
                    char *p = (char *)realloc(stru.pMem, newMemSize); \
                    if (NULL == p) \
                        return false; \
                    *(type **)(p + stru.memSize) = NULL; \
                    if (i > 0) \
                    { \
                        size_t size; \
                        if (getSize(pReflection, pPb, pDescriptor->field(i - 1), size)) \
                        { \
                            if (size > 0) \
                            { \
                                size_t memSize = size * sizeof(type); \
                                char *p1 = (char *)malloc(memSize); \
                                if (NULL == p1) \
                                    return false; \
                                for (size_t j = 0; j < size; ++j) \
                                    ((type *)p1)[j] = pReflection->getRepeatedMethod(*pPb, pFieldDescriptor, j); \
                                *(type **)(p + stru.memSize) = (type *)p1; \
                                stru.vecChild.emplace_back(p1, memSize); \
                            } \
                        } \
                    } \
                    stru.pMem = p; \
                    stru.memSize = newMemSize; \
                } \
                else \
                { \
                    size_t newMemSize = stru.memSize + sizeof(type); \
                    void *p = realloc(stru.pMem, newMemSize); \
                    if (NULL == p) \
                        return false; \
                    *(type *)(p + stru.memSize) = pReflection->getMethod(*pPb, pFieldDescriptor); \
                    stru.pMem = (char *)p; \
                    stru.memSize = newMemSize; \
                } \
                break; \
            }

            _CONVERT(double, CPPTYPE_DOUBLE, GetDouble, GetRepeatedDouble)
            _CONVERT(float, CPPTYPE_FLOAT, GetFloat, GetRepeatedFloat)
            _CONVERT(int32_t, CPPTYPE_INT32, GetInt32, GetRepeatedInt32)
            _CONVERT(int64_t, CPPTYPE_INT64, GetInt64, GetRepeatedInt64)
            _CONVERT(uint32_t, CPPTYPE_UINT32, GetUInt32, GetRepeatedUInt32)
            _CONVERT(uint64_t, CPPTYPE_UINT64, GetUInt64, GetRepeatedUInt64)
            _CONVERT(bool, CPPTYPE_BOOL, GetBool, GetRepeatedBool)
            _CONVERT(int, CPPTYPE_ENUM, GetEnumValue, GetRepeatedEnumValue)
#undef _CONVERT

            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            {
                if (pFieldDescriptor->is_repeated())
                {
                    size_t newMemSize = stru.memSize + sizeof(void *);
                    char *p = (char *)realloc(stru.pMem, newMemSize);
                    if (NULL == p)
                        return false;
                    *(char ***)(p + stru.memSize) = NULL;
                    if (i > 0)
                    {
                        size_t size;
                        if (getSize(pReflection, pPb, pDescriptor->field(i - 1), size))
                        {
                            if (size > 0)
                            {
                                size_t memSize = size * sizeof(char *);
                                char *p1 = (char *)malloc(memSize);
                                if (NULL == p1)
                                    return false;
                                *(char ***)(p + stru.memSize) = (char **)p1;
                                stru.vecChild.emplace_back(p1, memSize);
                                MemTree &memTree = stru.vecChild.back();
                                for (size_t j = 0; j < size; ++j)
                                {
                                    std::string str;
                                    str = pReflection->GetRepeatedStringReference(*pPb, pFieldDescriptor, j, &str);
                                    size_t memSize2 = str.size() + 1;
                                    char *p2 = (char *)calloc(memSize2, 1);
                                    if (NULL == p2)
                                        return false;
                                    strcpy(p2, str.c_str());
                                    ((char **)p1)[j] = p2;
                                    memTree.vecChild.emplace_back(p2, memSize2);
                                }
                            }
                        }
                    }
                    stru.pMem = p;
                    stru.memSize = newMemSize;
                }
                else
                {
                    size_t newMemSize = stru.memSize + sizeof(char *);
                    void *p = realloc(stru.pMem, newMemSize);
                    if (NULL == p)
                        return false;
                    std::string str;
                    str = pReflection->GetStringReference(*pPb, pFieldDescriptor, &str);
                    size_t memSize = str.size() + 1;
                    char *p1 = (char *)calloc(memSize, 1);
                    if (NULL == p1)
                        return false;
                    strcpy(p1, str.c_str());
                    *(char **)(p + stru.memSize) = p1;
                    stru.vecChild.emplace_back(p1, memSize);
                    stru.pMem = (char *)p;
                    stru.memSize = newMemSize;
                }
                break;
            }

            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            {
                if (pFieldDescriptor->is_repeated())
                {
                    size_t newMemSize = stru.memSize + sizeof(void *);
                    char *p = (char *)realloc(stru.pMem, newMemSize);
                    if (NULL == p)
                        return false;
                    *(void **)(p + stru.memSize) = NULL;
                    if (i > 0)
                    {
                        size_t size;
                        if (getSize(pReflection, pPb, pDescriptor->field(i - 1), size))
                        {
                            if (size > 0)
                            {
                                stru.vecChild.emplace_back((char *)NULL, 0);
                                MemTree &memTree = stru.vecChild.back();
                                for (size_t j = 0; j < size; ++j)
                                {
                                    const google::protobuf::Message &childPb = pReflection->GetRepeatedMessage(*pPb, pFieldDescriptor, j);
                                    if (!pb2struct(&childPb, memTree))
                                        return false;
                                }
                                *(void **)(p + stru.memSize) = (void *)memTree.pMem;
                            }
                        }
                    }
                    stru.pMem = p;
                    stru.memSize = newMemSize;
                }
                else
                {
                    const google::protobuf::Message &childPb = pReflection->GetMessage(*pPb, pFieldDescriptor);
                    if (!pb2struct(&childPb, stru))
                        return false;
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }
    return true;
}

bool PbConvertor::serializedPb2struct(const std::string &pbTypeName, const char *pSerializedPb, const size_t serializedPbSize, MemTree &stru)
{
    if (NULL == pSerializedPb)
        return false;

    const google::protobuf::Descriptor *pDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(pbTypeName);
    if (NULL == pDescriptor)
        return false;
    const google::protobuf::Message *pPrototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
    if (NULL == pPrototype)
        return false;
    google::protobuf::Message *pPb = pPrototype->New();
    if (NULL == pPb)
        return false;

    bool bRet = false;
    if (pPb->ParseFromArray(pSerializedPb, serializedPbSize))
        bRet = pb2struct(pPb, stru);
    delete pPb;
    return bRet;
}

bool PbConvertor::getSize(const google::protobuf::Reflection *pReflection, const google::protobuf::Message *pPb, const google::protobuf::FieldDescriptor *pFieldDescriptor, size_t &size)
{
    if (NULL == pReflection || NULL == pFieldDescriptor)
        return false;

    bool bRet = true;
    if (google::protobuf::FieldDescriptor::CPPTYPE_INT32 == pFieldDescriptor->cpp_type())
        size = pReflection->GetInt32(*pPb, pFieldDescriptor);
    else if (google::protobuf::FieldDescriptor::CPPTYPE_INT64 == pFieldDescriptor->cpp_type())
        size = pReflection->GetInt64(*pPb, pFieldDescriptor);
    else if (google::protobuf::FieldDescriptor::CPPTYPE_UINT32 == pFieldDescriptor->cpp_type())
        size = pReflection->GetUInt32(*pPb, pFieldDescriptor);
    else if (google::protobuf::FieldDescriptor::CPPTYPE_UINT64 == pFieldDescriptor->cpp_type())
        size = pReflection->GetUInt64(*pPb, pFieldDescriptor);
    else
        bRet = false;
    return bRet;
}
