# **PbConvertor-C++ convertor between protobuf message and C struct**
PbConvertor is used to convert protobuf message to C struct or C struct to protobuf message. PbConvertor is mainly based on protobuf reflection.
## **Dependency**
- C++11 or higher version
- protobuf
## **Usage**
Suppose your *.pb.h, *.pb.cc and C struct definition are prepared.
1. Refer to [C struct definition requirements](#anchor0) to modify your C struct definition.
2. Add PbConvertor.h and PbConvertor.cpp into your C++ project.
3. Use the following methods in PbConvertor class to convert protobuf message to C struct or C struct to protobuf message.
```
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
```
4. A usage [example](https://github.com/hardxuyp/PbConvertor/tree/master/Test) is provided.
### **C struct definition requirements**
<span id="anchor0"></span>
C struct definition must be one byte aligned using **#pragma pack(1)** and **#pragma pack()**.
<br>C struct member type must correspond to protobuf message field type according to the following table. The other C struct member types not listed in the following table are not allowed.

| protobuf message field type | C struct member type |
| :-------------------------- | :------------------- |
| double                      | double               |
| float                       | float                |
| int32                       | int32_t              |
| int64                       | int64_t              |
| uint32                      | uint32_t             |
| uint64                      | uint64_t             |
| bool                        | bool                 |
| string<br>Note:<br>UTF-8 encoding is required. | char * (not char[N])<br>Note:<br>1. UTF-8 encoding is required.<br>2. NULL value of char * member will be converted to "" of the protobuf message string field. |
| enum                        | enum : int           |
| message                     | struct<br>Note:<br>struct member must meet [C struct definition requirements](#anchor0). |
| repeated element_type<br>Note:<br>element_type must meet [C struct definition requirements](#anchor0). | element_type * (not element_type[N])<br>Note:<br>**1. The element amount of the array must be provided by the previous member whose type is int32_t/int64_t/uint32_t/uint64_t.**<br>2. element_type must meet [C struct definition requirements](#anchor0). |
## **Attention**
Your C++ project must have a multi-threaded configuration, otherwise protobuf will not work well.
