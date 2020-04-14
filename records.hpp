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
    Subrecord();
    Subrecord(std::string record_id, std::string subrecord_id, RecordDataType type, FILE *f, size_t *bytes_read);
    ~Subrecord();
    Subrecord(const Subrecord &other); // cc
    Subrecord &operator=(const Subrecord rhs); // ac

    std::string GetID();
    RecordDataType GetType();
    // We have a getter+setter so that the ownership of the data is clear (it's this object!)
    uint8_t* GetData();
    void SetData(uint8_t *data, size_t bytes); // data will be memcpy'd in here, you can free() your copy
    size_t GetSize();

private:
    std::string m_id;
    RecordDataType m_type;
    uint8_t *m_data;
    size_t m_data_size;
};

class Record {
public:
    Record(std::string record_id);
    void AddSubrecord(Subrecord subrecord);
    Subrecord& operator[](std::string srid);
    bool HasSubrecord(std::string srid);
    std::string GetID();
    bool Ignored;
    size_t Size;

private:
    std::string m_id;
    std::unordered_map<std::string, Subrecord> m_subrecords;
};

extern std::unordered_map<std::string, std::unordered_map<std::string, RecordDataType>> RecordToSubrecordTypes;

#endif /* __RECORDS_HPP_ */
