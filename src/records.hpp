#ifndef __RECORDS_HPP_
#define __RECORDS_HPP_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class RecordDataType
{
    String,
    Float,
    Int8,
    Int16,
    Int32,
    Int64,
    Data
};


class Subrecord
{
    public:
    // General constructors
    Subrecord();
    Subrecord(std::string subrecord_id, RecordDataType type, FILE *f, size_t *bytes_read);
    Subrecord(Subrecord *sr);

    // Constructors for manually creating subrecords
    Subrecord(std::string subrecord_id, uint8_t *data, size_t len_bytes);
    Subrecord(std::string subrecord_id, std::string s);

    Subrecord(const Subrecord &other);         // cc
    Subrecord &operator=(const Subrecord rhs); // ac

    std::string GetID() const;
    RecordDataType GetType();
    size_t GetDataSize();
    size_t GetSubrecordSize();
    void WriteSubrecord(uint8_t *buf, size_t *remaining_bytes); // Make sure that buf has GetDataSize() of free space!!!

    std::shared_ptr<std::vector<uint8_t>> Data; // The stored data, don't do anything stupid with it!

    private:
    std::string m_id;
    RecordDataType m_type;
};

class Record
{
    public:
    Record(std::string record_id);
    void AddSubrecord(Subrecord subrecord);
    void ClearSubrecords(std::vector<std::string> ids);
    std::vector<std::shared_ptr<Subrecord>> GetSubrecords(std::string srid);
    bool HasSubrecord(std::string srid);
    std::string GetID() const;
    std::string Name;
    size_t GetRecordSize();
    void WriteRecord(uint8_t *buf, size_t *remaining_bytes);
    bool Ignored;
    Record *Previous_DIAL;

    private:
    std::string m_id;
    std::vector<std::shared_ptr<Subrecord>> m_subrecords;
};

int64_t HasRecordWithName(std::vector<Record *> &records, std::string id);

extern std::unordered_map<std::string, std::unordered_map<std::string, RecordDataType>> RecordToSubrecordTypes;

#endif /* __RECORDS_HPP_ */
