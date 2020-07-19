#include "records.hpp"

#include "iohelpers.hpp"

#include <algorithm>
#include <cinttypes>
#include <cstdint>
#include <cstring>

Subrecord::Subrecord()
{
    throw "Fun fact: this never happens, yet GCC won't compile if this constructor doesn't exist";
}

const std::string TARGET = "FNAM";

Subrecord::Subrecord(std::string subrecord_id, RecordDataType type, FILE *f, size_t *bytes_read)
{
    std::string str;

    m_id   = subrecord_id;
    m_type = type;
    size_t data_size;

    if (m_type != RecordDataType::String)
        data_size = io::read_dword(f, bytes_read); // Subrecord length, reading
    else
    {
        str       = io::read_string(f, bytes_read);
        data_size = str.length() + 1;
    }

    Data = std::make_shared<std::vector<uint8_t>>(data_size);
    uint8_t buf[data_size];

    switch (m_type)
    {
        case RecordDataType::Data:
            if (!fread(buf, 1, data_size, f))
                throw "Error in Subrecord::Subrecord";
            *bytes_read += data_size;
            break;
        case RecordDataType::Float:
            *(float *)buf = io::read_float(f, bytes_read);
            break;
        case RecordDataType::Int8:
            *(int8_t *)buf = io::read_byte(f, bytes_read);
            break;
        case RecordDataType::Int16:
            *(int16_t *)buf = io::read_word(f, bytes_read);
            break;
        case RecordDataType::Int32:
            *(int32_t *)buf = io::read_dword(f, bytes_read);
            break;
        case RecordDataType::Int64:
            *(int32_t *)buf       = io::read_dword(f, bytes_read);
            *(int32_t *)(buf + 4) = io::read_dword(f, bytes_read);
            break;
        case RecordDataType::String:
            strcpy((char *)buf, str.c_str());
            buf[data_size - 1] = '\0';
            break;
        default:
            break;
    }

    for (size_t i = 0; i < data_size; ++i)
        Data->at(i) = buf[i];
}

Subrecord::Subrecord(std::string subrecord_id, uint8_t *data, size_t len_bytes)
{
    m_id   = subrecord_id;
    m_type = RecordDataType::Data;
    Data   = std::make_shared<std::vector<uint8_t>>(len_bytes);
    for (size_t i = 0; i < len_bytes; ++i)
        Data->at(i) = data[i];
}

Subrecord::Subrecord(Subrecord *sr)
{
    // if (m_id == TARGET)
    //     printf("Constructor 0x%" PRIxPTR " -> 0x%" PRIxPTR "\n", (intptr_t)sr, (intptr_t)this);
    m_id   = sr->GetID();
    m_type = sr->GetType();
    Data   = std::make_shared<std::vector<uint8_t>>(sr->GetDataSize());
    *Data  = *sr->Data;
}

Subrecord::Subrecord(std::string subrecord_id, std::string s)
{
    // if (m_id == TARGET)
    //     printf("Constructor of 0x%" PRIxPTR "\n",(intptr_t)this);
    m_id             = subrecord_id;
    m_type           = RecordDataType::String;
    size_t data_size = s.length() + 1;
    Data             = std::make_shared<std::vector<uint8_t>>(data_size);
    const char *cstr = s.c_str();
    for (size_t i = 0; i < data_size - 1; ++i)
        Data->at(i) = cstr[i];
    Data->at(data_size - 1) = '\0';
}

Subrecord::Subrecord(const Subrecord &other) : m_id(other.m_id), m_type(other.m_type)
{
    // if (m_id == TARGET)
    //     printf("Copy-constructor 0x%" PRIxPTR " -> 0x%" PRIxPTR "\n", (intptr_t)&other, (intptr_t) this);
    Data  = std::make_shared<std::vector<uint8_t>>(other.Data->size());
    *Data = *other.Data;
}

Subrecord &Subrecord::operator=(const Subrecord rhs)
{
    // if (m_id == TARGET)
    //     printf("operator=  0x%" PRIxPTR " -> 0x%" PRIxPTR "\n", (intptr_t)&rhs, (intptr_t) this);
    m_id   = rhs.m_id;
    m_type = rhs.m_type;
    Data   = std::make_shared<std::vector<uint8_t>>(rhs.Data->size());
    *Data  = *rhs.Data;

    return *this;
}

std::string Subrecord::GetID() const
{
    return m_id;
}

RecordDataType Subrecord::GetType()
{
    return m_type;
}

size_t Subrecord::GetDataSize()
{
    return Data->size();
}

size_t Subrecord::GetSubrecordSize()
{
    return 4 + 4 + Data->size(); // (char[4])ID + (long)size of data, data
}

void Subrecord::WriteSubrecord(uint8_t *buf, size_t *remaining_bytes)
{
    size_t sz = GetSubrecordSize();
    if (*remaining_bytes < sz)
        throw "Not enough free space to write " + GetID() + " data";

    size_t data_size = Data->size();
    io::write_bytes(buf + 0, (uint8_t *)GetID().c_str(), 4);
    io::write_dword(buf + 4, data_size);
    for (size_t i = 0; i < data_size; i++)
        buf[8 + i] = Data->at(i);
    // io::write_bytes(buf + 8, Data.get()->data(), m_data_size);

    *remaining_bytes -= sz;
}


Record::Record(std::string record_id)
{
    m_id = record_id;
}

void Record::AddSubrecord(Subrecord subrecord)
{
    m_subrecords.push_back(std::make_shared<Subrecord>(subrecord));
    if (subrecord.GetID() == "NAME")
    {
        size_t sz = subrecord.Data->size();
        char name[sz + 1];
        for (size_t i = 0; i < sz; i++)
            name[i] = subrecord.Data->at(i);
        name[sz] = '\0';
        Name = std::string(name);
    }
}


void Record::ClearSubrecords(std::vector<std::string> ids)
{
    m_subrecords.erase(std::remove_if(m_subrecords.begin(), m_subrecords.end(),
                                      [ids](std::shared_ptr<Subrecord> sr) {
                                          return std::find(ids.begin(), ids.end(), sr->GetID()) != ids.end();
                                      }),
                       m_subrecords.end());
}

std::string Record::GetID() const
{
    return m_id;
}

std::vector<std::shared_ptr<Subrecord>> Record::GetSubrecords(std::string srid)
{
    std::vector<std::shared_ptr<Subrecord>> result;
    for (std::shared_ptr<Subrecord> sr : m_subrecords)
        if (sr->GetID() == srid || srid == "*")
            result.push_back(sr);
    return result;
}

bool Record::HasSubrecord(std::string srid)
{
    return std::find_if(m_subrecords.begin(), m_subrecords.end(),
                        [srid](std::shared_ptr<Subrecord> sr) { return sr->GetID() == srid; }) != m_subrecords.end();
}

size_t Record::GetRecordSize()
{
    size_t sz = 16;
    for (std::shared_ptr<Subrecord> subrecord : m_subrecords)
        sz += subrecord->GetSubrecordSize();
    return sz;
}


void Record::WriteRecord(uint8_t *buf, size_t *remaining_bytes)
{
    size_t sz = GetRecordSize();
    if (*remaining_bytes < sz)
        throw "Not enough free space to write " + GetID() + " data";

    uint8_t tmp[4] = { 0, 0, 0, 0 };
    io::write_bytes(buf + 0, (uint8_t *)GetID().c_str(), 4);
    io::write_dword(buf + 4, sz - 16); // Size is without header which is always 12 bytes
    io::write_bytes(buf + 8, tmp, 4);
    io::write_bytes(buf + 12, tmp, 4);
    *remaining_bytes -= 16;

    size_t offset = 16;
    for (std::shared_ptr<Subrecord> subrecord : m_subrecords)
    {
        size_t srsz = subrecord->GetSubrecordSize();
        subrecord->WriteSubrecord(buf + offset, remaining_bytes);
        offset += srsz;
    }
}

int64_t HasRecordWithName(std::vector<Record *> &records, std::string id)
{
    int64_t i = 0;
    for (auto r : records)
    {
        if (r->Name == id)
            return i;
        ++i;
    }
    return -1;
}

std::unordered_map<std::string, std::unordered_map<std::string, RecordDataType>> RecordToSubrecordTypes = {
    { "TES3",
      {
          { "HEDR", RecordDataType::Data },
          { "MAST", RecordDataType::String },
          { "DATA", RecordDataType::Data },
      } },
    { "GMST",
      {
          { "NAME", RecordDataType::String },
          { "STRV", RecordDataType::String },
          { "INTV", RecordDataType::Int32 },
          { "FLTV", RecordDataType::Float },
      } },
    { "GLOB",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::Int8 },
          { "FLTV", RecordDataType::Float },
      } },
    { "CLAS",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "CLDT", RecordDataType::Data },
          { "DESC", RecordDataType::String },
      } },
    { "FACT",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "RNAM", RecordDataType::String },
          { "FADT", RecordDataType::Data },
          { "ANAM", RecordDataType::String },
          { "INTV", RecordDataType::Int32 },
      } },
    { "RACE",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "RADT", RecordDataType::Data },
          { "NPCS", RecordDataType::String },
          { "DESC", RecordDataType::String },
      } },
    { "SOUN",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "DATA", RecordDataType::Data },
      } },
    { "SKIL",
      {
          { "INDX", RecordDataType::Int32 },
          { "SKDT", RecordDataType::Data },
          { "DESC", RecordDataType::String },
      } },
    { "MGEF",
      {
          { "INDX", RecordDataType::Int32 },
          { "MEDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "PTEX", RecordDataType::String },
          { "CVFX", RecordDataType::String },
          { "BVFX", RecordDataType::String },
          { "HVFX", RecordDataType::String },
          { "AVFX", RecordDataType::String },
          { "DESC", RecordDataType::String },
          { "CSND", RecordDataType::String },
          { "BSND", RecordDataType::String },
          { "HSND", RecordDataType::String },
          { "ASND", RecordDataType::String },
      } },
    { "SCPT",
      {
          { "SCHT", RecordDataType::Data },
          { "SCVR", RecordDataType::String },
          { "SCDT", RecordDataType::String }, // It's *technically* data, but it can be read as a string since we're
                                              // discarding it anyway and it's more convenient this way
          { "SCTX", RecordDataType::String },
      } },
    { "REGN",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "WEAT", RecordDataType::Data },
          { "BNAM", RecordDataType::String },
          { "CNAM", RecordDataType::Data },
          { "SNAM", RecordDataType::Data },
      } },
    { "BSGN",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "TNAM", RecordDataType::String },
          { "DESC", RecordDataType::String },
          { "NPCS", RecordDataType::String },
      } },
    { "LTEX",
      {
          { "NAME", RecordDataType::String },
          { "INTV", RecordDataType::Data },
          { "DATA", RecordDataType::String },
      } },
    { "STAT",
      {
          { "NAME", RecordDataType::String },
          { "MODL", RecordDataType::String },
      } },
    { "DOOR",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "SCIP", RecordDataType::String },
          { "SNAM", RecordDataType::String },
          { "ANAM", RecordDataType::String },
      } },
    { "MISC",
      {
          { "NAME", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MCDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "ENAM", RecordDataType::Data },
          { "SCRI", RecordDataType::String },
      } },
    { "WEAP",
      {
          { "NAME", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "WPDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "ENAM", RecordDataType::Data },
          { "SCRI", RecordDataType::String },
      } },
    { "CONT",
      {
          { "NAME", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "CNDT", RecordDataType::Float },
          { "FLAG", RecordDataType::Int32 }, // Actually a 4-byte bit field, but more convenient to read out like this
          { "NPCO", RecordDataType::Data },
      } },
    { "SPEL",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "SPDT", RecordDataType::Data },
          { "ENAM", RecordDataType::Data },
      } },
    { "CREA",
      {
          { "NAME", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "NPDT", RecordDataType::Data },
          { "FLAG", RecordDataType::Data },
          { "SCRI", RecordDataType::String },
          { "NPCO", RecordDataType::Data },
          { "AIDT", RecordDataType::Data },
          { "AI_W", RecordDataType::Data },
          { "AI_T", RecordDataType::Data },
          { "AI_F", RecordDataType::Data },
          { "AI_E", RecordDataType::Data },
          { "AI_A", RecordDataType::Data },
          { "XSCL", RecordDataType::Float },
      } },
    { "BODY",
      {
          { "BYDT", RecordDataType::Int32 }, // Actually four single bytes, but more convenient to read out like this
      } },
    { "LIGH",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "LHDT", RecordDataType::Data },
          { "SCPT", RecordDataType::String },
          { "ITEX", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "SNAM", RecordDataType::String },
      } },
    { "ENCH",
      {
          { "NAME", RecordDataType::String },
          { "ENDT", RecordDataType::Data },
          { "ENAM", RecordDataType::Data },
      } },
    { "NPC_",
      {
          { "NAME", RecordDataType::String }, { "FNAM", RecordDataType::String }, { "MODL", RecordDataType::String },
          { "RNAM", RecordDataType::String }, { "ANAM", RecordDataType::String }, { "BNAM", RecordDataType::String },
          { "CNAM", RecordDataType::String }, { "KNAM", RecordDataType::String }, { "NPDT", RecordDataType::Data },
          { "FLAG", RecordDataType::Int32 }, // Actually a 4-byte bit field, but more convenient to read out like this
          { "NPCO", RecordDataType::Data },   { "NPCS", RecordDataType::String }, { "AIDT", RecordDataType::Data },
          { "AI_W", RecordDataType::Data },   { "AI_T", RecordDataType::Data },   { "AI_F", RecordDataType::Data },
          { "AI_E", RecordDataType::Data },   { "AI_A", RecordDataType::Data },   { "CNDT", RecordDataType::String },
          { "DODT", RecordDataType::Data },   { "DNAM", RecordDataType::String }, { "XSCL", RecordDataType::Float },
      } },
    { "ARMO",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "AODT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "INDX", RecordDataType::Int8 },
          { "BNAM", RecordDataType::String },
          { "CNAM", RecordDataType::String },
          { "ENAM", RecordDataType::Data },
          { "SCRI", RecordDataType::String },
      } },
    { "CLOT",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "CTDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "INDX", RecordDataType::Int8 },
          { "BNAM", RecordDataType::String },
          { "CNAM", RecordDataType::String },
          { "ENAM", RecordDataType::Data },
          { "SCRI", RecordDataType::String },
      } },
    { "REPA",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "RIDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "SCRI", RecordDataType::String },
      } },
    { "ACTI",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "SCRI", RecordDataType::String },
      } },
    { "APPA",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "AADT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "SCRI", RecordDataType::String },
      } },
    { "LOCK",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "LKDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "SCRI", RecordDataType::String },
      } },
    { "PROB",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "PBDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "SCRI", RecordDataType::String },
      } },
    { "INGR",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "IRDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "SCRI", RecordDataType::String },
      } },
    { "BOOK",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "BKDT", RecordDataType::Data },
          { "ITEX", RecordDataType::String },
          { "SCRI", RecordDataType::String },
      } },
    { "ALCH",
      {
          { "NAME", RecordDataType::String },
          { "FNAM", RecordDataType::String },
          { "MODL", RecordDataType::String },
          { "ALDT", RecordDataType::Data },
          { "ENAM", RecordDataType::Data },
          { "TEXT", RecordDataType::String },
          { "SCRI", RecordDataType::String },
      } },
    { "LEVI",
      {
          { "NAME", RecordDataType::String },
          { "DATA", RecordDataType::Int32 }, // Actually two int16s
          { "NNAM", RecordDataType::Int8 },
          { "INDX", RecordDataType::Int32 },
          { "INAM", RecordDataType::String },
          { "INTV", RecordDataType::Int16 },
      } },
    { "LEVC",
      {
          { "NAME", RecordDataType::String },
          { "DATA", RecordDataType::Int32 },
          { "NNAM", RecordDataType::Int8 },
          { "INDX", RecordDataType::Int32 },
          { "CNAM", RecordDataType::String },
          { "INTV", RecordDataType::Int16 },
      } },
    { "CELL",
      {
          { "NAME", RecordDataType::String },

          // What the heck is going on here?

          /*{ "DATA", RecordDataType::Data },
           *      { "RGNN", RecordDataType::String },
           *      { "NAM0", RecordDataType::Int32 },
           *      // Ext. cell
           *      { "NAM5", RecordDataType::Int32 },
           *      // Int. cell
           *      { "WHGT", RecordDataType::Float },
           *      { "AMBI", RecordDataType::Data },
           *      // Refs
           *      { "FRMR", RecordDataType:: },
           *      { "XSCL", RecordDataType:: },
           *      { "DELE", RecordDataType:: },
           *      { "DODT", RecordDataType:: },
           *      { "DNAM", RecordDataType:: },
           *      { "FLTV", RecordDataType:: },
           *      { "KNAM", RecordDataType:: },
           *      { "TNAM", RecordDataType:: },
           *      { "UNAM", RecordDataType:: },
           *      { "ANAM", RecordDataType:: },
           *      { "BNAM", RecordDataType:: },
           *      { "INTV", RecordDataType:: },
           *      { "NAM9", RecordDataType:: },
           *      { "XSOL", RecordDataType:: },
           *      { "DATA", RecordDataType:: }, // ???
           */
      } },
    { "LAND",
      {
          { "INTV", RecordDataType::Int64 }, // Actually two int32s
          { "DATA", RecordDataType::Int32 },
          { "VNML", RecordDataType::Data },
          { "VHGT", RecordDataType::Data },
          { "WNAM", RecordDataType::Data },
          { "VCLR", RecordDataType::Data },
          { "VTEX", RecordDataType::Data },
      } },
    { "PGRD", { /* Nothing to see here, folks! */ } },
    { "SNDG",
      {
          { "NAME", RecordDataType::String },
          { "DATA", RecordDataType::Int32 },
          { "SNAM", RecordDataType::String },
          { "CNAM", RecordDataType::String },
      } },
    { "DIAL",
      {
          { "NAME", RecordDataType::String },
          { "DATA", RecordDataType::Int8 },
      } },
    { "INFO",
      {
          { "INAM", RecordDataType::String }, { "PNAM", RecordDataType::String }, { "NNAM", RecordDataType::String },
          { "DATA", RecordDataType::Data },   { "ONAM", RecordDataType::String }, { "RNAM", RecordDataType::String },
          { "CNAM", RecordDataType::String }, { "FNAM", RecordDataType::String }, { "ANAM", RecordDataType::String },
          { "DNAM", RecordDataType::String }, { "NAME", RecordDataType::String }, { "SNAM", RecordDataType::String },
          { "QSTN", RecordDataType::Int8 },   { "QSTF", RecordDataType::Int8 },   { "QSTR", RecordDataType::Int8 },
          { "SCVR", RecordDataType::String }, { "INTV", RecordDataType::Int32 }, // Actually two int32s
          { "FLTV", RecordDataType::Int32 },  { "BNAM", RecordDataType::String },
      } },
    /* Tribunal */
    { "SSCR",
      {
          { "NAME", RecordDataType::String }, { "DATA", RecordDataType::String }, // Some non-NULL-terminated string
      } }
};
