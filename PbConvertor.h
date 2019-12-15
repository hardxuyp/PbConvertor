#ifndef PBCONVERTOR_H
#define PBCONVERTOR_H

/**
 * @file PbConvertor.h
 * @brief PbConvertor is used to convert protobuf message to C struct or C struct to protobuf message.
 * @author xuyp
 * @date 2019.12
 */

#include <string>
#include <vector>
#include <google/protobuf/message.h>

class PbConvertor
{
public:
    struct MemTree
    {
        char *pMem;
        size_t memSize;
        std::vector<MemTree> vecChild;

        MemTree()
        {
            pMem = NULL;
            memSize = 0;
        }

        MemTree(char *pMem, size_t memSize)
        {
            this->pMem = pMem;
            this->memSize = memSize;
        }

        void release()
        {
            for (auto &child : vecChild)
                child.release();
            if (NULL != pMem)
                free(pMem);
        }
    };

    /**
     * @brief convert C struct to protobuf message.
     * @param pStruct pointer to C struct.
     * @param pPb pointer to protobuf message.
     * @return whether successful.
     */
    static bool struct2Pb(const char *&pStruct, google::protobuf::Message *pPb);

    /**
     * @brief convert C struct to serialized protobuf message.
     * @param pStruct pointer to C struct.
     * @param pbTypeName protobuf message type name.
     * @param pSerializedPb pointer to serialized protobuf message which must be released by user using delete [].
     * @param serializedPbSize serialized protobuf message size.
     * @return whether successful.
     */
    static bool struct2serializedPb(const char *pStruct, const std::string &pbTypeName, char *&pSerializedPb, size_t &serializedPbSize);

    /**
     * @brief convert protobuf message to C struct.
     * @param pPb pointer to protobuf message.
     * @param stru memory tree related to C struct, and pointer to C struct equals to stru.pMem.
     *        Memory tree must be released by user using stru.release().
     * @return whether successful.
     */
    static bool pb2struct(const google::protobuf::Message *pPb, MemTree &stru);

    /**
     * @brief convert serialized protobuf message to C struct.
     * @param pbTypeName protobuf message type name.
     * @param pSerializedPb pointer to serialized protobuf message.
     * @param serializedPbSize serialized protobuf message size.
     * @param stru memory tree related to C struct, and pointer to C struct equals to stru.pMem.
     *        Memory tree must be released by user using stru.release().
     * @return whether successful.
     */
    static bool serializedPb2struct(const std::string &pbTypeName, const char *pSerializedPb, const size_t serializedPbSize, MemTree &stru);

private:
    static bool getSize(const google::protobuf::Reflection *pReflection, const google::protobuf::Message *pPb, const google::protobuf::FieldDescriptor *pFieldDescriptor, size_t &size);
};

#endif // PBCONVERTOR_H
