#include "PbConvertor.h"
#include "Test.pb.h"
#include "Test.h"

int main()
{
    testNamespace::TestMessage testMessage;

    testNamespace::TestChildMessage testChildMessage;
    testChildMessage.int32Field = -123;
    testChildMessage.stringField = "Hello, Kobe Bryant.";
    testChildMessage.testGrandchildMessageField.doubleField = 1.234;
    testChildMessage.testGrandchildMessageField.repeatedStringLengthField = 3;
    char *arr0[3] = {"How are you?", "Fine, thank you.", "And you?"};
    testChildMessage.testGrandchildMessageField.repeatedStringField = arr0;

    testMessage.doubleField = 1.234;
    testMessage.floatField = 1.23;
    testMessage.int32Field = -123;
    testMessage.int64Field = -1234;
    testMessage.uint32Field = 123;
    testMessage.uint64Field = 1234;
    testMessage.boolField = true;
    testMessage.stringField = "Hello, 科比.布莱恩特.";
    testMessage.testEnumField = testNamespace::enum3;
    testMessage.testChildMessageField = testChildMessage;
    testMessage.repeatedDoubleLengthField = 3;
    double arr1[3] = {1.234, 5.678, 9.101112};
    testMessage.repeatedDoubleField = arr1;
    testMessage.repeatedStringLengthField = 3;
    char *arr2[3] = {"abc", "def", "ghi"};
    testMessage.repeatedStringField = arr2;
    testMessage.repeatedTestChildMessageLengthField = 3;
    testNamespace::TestChildMessage arr3[3];
    arr3[0] = testChildMessage;
    arr3[1] = testChildMessage;
    arr3[2] = testChildMessage;
    testMessage.repeatedTestChildMessageField = arr3;

    char *pSerializedPb;
    size_t serializedPbSize;
    if (PbConvertor::struct2serializedPb((const char *)&testMessage, "testPackage.TestMessage", pSerializedPb, serializedPbSize))
    {
        testPackage::TestMessage testMessagePb;
        bool bRet = testMessagePb.ParseFromArray(pSerializedPb, serializedPbSize);

        PbConvertor::MemTree stru;
        if (PbConvertor::serializedPb2struct("testPackage.TestMessage", pSerializedPb, serializedPbSize, stru))
        {
            testNamespace::TestMessage *pTestMessage = (testNamespace::TestMessage *)stru.pMem;
            stru.release();
        }
        delete [] pSerializedPb;
    }

    return 0;
}
