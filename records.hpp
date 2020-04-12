#ifndef __RECORDS_HPP_
#define __RECORDS_HPP_

#include <string>
#include <unordered_map>

enum class RecordDataType {
    String,
    Float,
    Int8,
    Int16,
    Int32,
    Int64,
    Data
};


class Subrecord {
public:
    Subrecord(std::string record_id, std::string subrecord_id, RecordDataType type, FILE *f, size_t *bytes_read);
    ~Subrecord();
    std::string GetID();
    RecordDataType GetType();
    void* GetData();          // replace with get_int8, get_int16, ... ?

private:
    std::string m_id;
    RecordDataType m_type;
    void *m_data;
};

class Record {
public:
    Record(std::string record_id);
    ~Record();
    void AddSubrecord(Subrecord *subrecord);
    std::string GetID();
    bool Ignored;

    size_t Size;

private:
    std::string m_id;
    std::unordered_map<std::string, Subrecord*> *m_subrecords;
};


extern std::unordered_map<std::string, std::unordered_map<std::string, RecordDataType>> RecordToSubrecordTypes;

#endif /* __RECORDS_HPP_ */
