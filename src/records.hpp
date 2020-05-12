#ifndef __RECORDS_HPP_
#define __RECORDS_HPP_

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

    std::string GetID();
    RecordDataType GetType();
    std::unique_ptr<uint8_t[]> GetData();
    void SetData(std::unique_ptr<uint8_t[]> data, size_t bytes);
    size_t GetDataSize();
    size_t GetSubrecordSize();
    void WriteSubrecord(uint8_t *buf, size_t *remaining_bytes);

    private:
    std::string m_id;
    RecordDataType m_type;
    std::unique_ptr<uint8_t[]> m_data;
    size_t m_data_size;
};

class Record
{
    public:
    Record(std::string record_id);
    void AddSubrecord(std::unique_ptr<Subrecord> subrecord);
    void ClearSubrecords(std::vector<std::string> ids);
    std::vector<std::unique_ptr<Subrecord>> GetSubrecords(std::string srid);
    bool HasSubrecord(std::string srid);
    std::string GetID();
    std::string GetName();
    size_t GetRecordSize();
    void WriteRecord(uint8_t *buf, size_t *remaining_bytes);
    bool Ignored;
    Record *Previous_DIAL;

    private:
    std::string m_id;
    std::vector<std::unique_ptr<Subrecord>> m_subrecords;
};

int64_t HasRecordWithName(std::vector<Record *> &records, std::string id);

extern std::unordered_map<std::string, std::unordered_map<std::string, RecordDataType>> RecordToSubrecordTypes;

#endif /* __RECORDS_HPP_ */
