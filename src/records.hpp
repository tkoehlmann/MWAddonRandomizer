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

/**
 * A subrecord
 * Example: WEAP-Records have a WPDT subrecord that describe the weapons' stats.
 */
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

    std::string GetID() const; // Returns the subrecord's ID (i.e. "FNAM")
    RecordDataType GetType();  // The kind of data that this subrecord holds
    size_t GetDataSize();      // Number of data bytes
    size_t GetSubrecordSize(); // The size of the subrecord when being written into a .esp file
    void WriteSubrecord(uint8_t *buf, size_t *remaining_bytes); // Writes the subrecord into some raw memory, make sure
                                                                // that buf has GetDataSize() of free space!!!

    std::shared_ptr<std::vector<uint8_t>> Data; // The stored data, don't do anything stupid with it!

    private:
    std::string m_id;
    RecordDataType m_type;
};

/**
 * A record
 * Example: WEAP records store weapon data, containing some amount of type-specific subrecords
 */
class Record
{
    public:
    Record(std::string record_id);                      // Constructor
    void AddSubrecord(Subrecord subrecord);             // Adds a subrecord to this record
    void ClearSubrecords(std::vector<std::string> ids); // Delete all stored subrecords
    std::vector<std::shared_ptr<Subrecord>>
        GetSubrecords(std::string srid); // Retrieve all subrecords with the given ID
    bool HasSubrecord(std::string srid); // Returns whether this record contains a subrecord with the given ID
    std::string GetID() const;           // Returns this record's ID (i.e. "WEAP")
    std::string Name;                    // The name of this record (i.e. "ebon_plate_cuirass_unique")
    size_t GetRecordSize(); // The size of this record when being written into a .esp file, already contains all the
                            // subrecords' sizes as well
    void WriteRecord(uint8_t *buf, size_t *remaining_bytes); // Writes the record into some raw memory, make sure that
                                                             // buf has GetRecordSize() of free space!!!
    bool Ignored; // All kind of records get extracted from the master and plugin files, but some won't ever be touched.
                  // This is the field to declare that.
    std::shared_ptr<Record>
        Previous_DIAL; // INFO records belong to some DIAL record. Unfortunately, INFO records don't contain any
                       // explicit reference, so this must be set when reading the master files

    private:
    std::string m_id;
    std::vector<std::shared_ptr<Subrecord>> m_subrecords;
};

/**
 * A collection of records of (ideally) a certain type.
 * This will automatically provide a hash map to quickly retrieve records by name (not ID!)
 */
class RecordCollection
{
    public:
    void Insert(const std::shared_ptr<Record> r);               // Inserts a given record into the hash map
    std::shared_ptr<Record> Get(const std::string &name);       // Retrieves the record with the given name
    std::shared_ptr<Record> operator[](const std::string &key); // The same as Get(...)

    // Collection stuff
    std::unordered_map<std::string, std::shared_ptr<Record>>::iterator begin();
    std::unordered_map<std::string, std::shared_ptr<Record>>::iterator end();
    std::unordered_map<std::string, std::shared_ptr<Record>>::iterator find(const std::string &key) const;

    private:
    std::unordered_map<std::string, std::shared_ptr<Record>> m_records;
};

extern std::unordered_map<std::string, std::unordered_map<std::string, RecordDataType>> RecordToSubrecordTypes;

#endif /* __RECORDS_HPP_ */
