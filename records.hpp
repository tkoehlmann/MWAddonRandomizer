#ifndef __RECORDS_HPP_
#define __RECORDS_HPP_

#include <string>
#include <vector>
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
    // General constructors
    Subrecord();
    Subrecord(std::string subrecord_id, RecordDataType type, FILE *f, size_t *bytes_read);

    // Constructors for manually creating subrecords
    Subrecord(std::string subrecord_id, uint8_t *data, size_t len_bytes);
    Subrecord(std::string subrecord_id, std::string s);

    ~Subrecord();
    Subrecord(const Subrecord &other); // cc
    Subrecord &operator=(const Subrecord rhs); // ac

    std::string GetID();
    RecordDataType GetType();
    // We have a getter+setter so that the ownership of the data is clear (it's this object!)
    uint8_t* GetData();
    void SetData(uint8_t *data, size_t bytes); // data will be memcpy'd in here, you can free() your copy
    size_t GetDataSize();
    size_t GetSubrecordSize();
    void WriteSubrecord(uint8_t *buf, size_t *remaining_bytes);

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
    void ClearNonIDSubrecords();
    std::vector<Subrecord> GetSubrecords(std::string srid);
    bool HasSubrecord(std::string srid);
    std::string GetID();
    size_t GetRecordSize();
    void WriteRecord(uint8_t *buf, size_t *remaining_bytes);
    bool Ignored;
    //size_t Size; // Only used by the esmloader! Don't use anywhere else! Use GetRecordSize() instead!

private:
    std::string m_id;
    std::vector<Subrecord> m_subrecords;
};

extern std::unordered_map<std::string, std::unordered_map<std::string, RecordDataType>> RecordToSubrecordTypes;

#endif /* __RECORDS_HPP_ */
