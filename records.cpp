#include <cstring>

#include "records.hpp"
#include "iohelpers.hpp"

Subrecord::Subrecord()
{
    throw "Fun fact: this never happens, yet GCC won't compile if this constructor doesn't exist";
}

Subrecord::Subrecord(std::string record_id, std::string subrecord_id, RecordDataType type, FILE *f, size_t *bytes_read)
{
    std::string str;
    size_t s;

    m_id = subrecord_id;
    m_type = type;

    switch (m_type)
    {
    case RecordDataType::Data:
        s = io::read_dword(f, bytes_read);
        m_data = (uint8_t*)malloc(s);
        m_data_size = s;
        if (!fread(m_data, 1, s, f))
            throw "Error in Subrecord::Subrecord";
        *bytes_read += s;
        break;
    case RecordDataType::Float:
        m_data = (uint8_t*)malloc(sizeof(float));
        m_data_size = sizeof(float);
        *(float*)m_data = io::read_float(f, bytes_read);
        break;
    case RecordDataType::Int8:
        m_data = (uint8_t*)malloc(sizeof(int8_t));
        m_data_size = sizeof(uint8_t);
        *(int8_t*)m_data = io::read_byte(f, bytes_read);
        break;
    case RecordDataType::Int16:
        m_data = (uint8_t*)malloc(sizeof(int16_t));
        m_data_size = sizeof(uint16_t);
        *(int16_t*)m_data = io::read_word(f, bytes_read);
        break;
    case RecordDataType::Int32:
        m_data = (uint8_t*)malloc(sizeof(int32_t));
        m_data_size = sizeof(int32_t);
        *(int32_t*)m_data = io::read_dword(f, bytes_read);
        break;
    case RecordDataType::Int64:
        m_data = (uint8_t*)malloc(2 * sizeof(int32_t));
        m_data_size = 2 * sizeof(int32_t);
        *(int32_t*)m_data = io::read_dword(f, bytes_read);
        *((int32_t*)m_data + 1) = io::read_dword(f, bytes_read);
        break;
    case RecordDataType::String:
        str = io::read_string(f, bytes_read);
        m_data = (uint8_t*)calloc(str.length() + 1, sizeof(char));
        m_data_size = str.length() + 1;
        strcpy((char*)m_data, str.c_str());
        break;
    default:
        break;
    }
}

Subrecord::~Subrecord()
{
#ifdef DEBUG
    printf("deleting \"%s\" (%d)...", GetID().c_str(), (int)GetType());
#endif

    free(m_data);
}

Subrecord::Subrecord(const Subrecord &other)
    : m_id(other.m_id), m_type(other.m_type), m_data_size(other.m_data_size)
{
#ifdef DEBUG
    printf("CC'ing Subrecord \"%s\" (%d)...\n", GetID().c_str(), (int)GetType());
#endif

    uint8_t *data = (uint8_t *)malloc(m_data_size);
    memcpy(data, other.m_data, m_data_size);
    m_data = data;
}

Subrecord& Subrecord::operator=(const Subrecord rhs)
{
#ifdef DEBUG
    printf("=ing Subrecord \"%s\" (%d)...\n", GetID().c_str(), (int)GetType());
#endif

    m_id = rhs.m_id;
    m_type = rhs.m_type;
    m_data_size = rhs.m_data_size;
    m_data = rhs.m_data;

    return *this;
}

std::string Subrecord::GetID()
{
    return m_id;
}

RecordDataType Subrecord::GetType()
{
    return m_type;
}

uint8_t* Subrecord::GetData()
{
    return m_data;
}

void Subrecord::SetData(uint8_t *data, size_t bytes)
{
    if (bytes != m_data_size)
        auto ptr = realloc(m_data, bytes);
    memcpy(m_data, data, bytes);
}

size_t Subrecord::GetSize()
{
    return m_data_size;
}

Record::Record(std::string record_id)
{
    m_id = record_id;
}

void Record::AddSubrecord(Subrecord subrecord)
{
    m_subrecords.insert({ subrecord.GetID(), subrecord });
}

std::string Record::GetID()
{
    return m_id;
}

Subrecord& Record::operator[](std::string srid)
{
    if (!HasSubrecord(srid))
        throw "Unknown subrecord " + srid + " in record " + GetID();
    return m_subrecords[srid];
}

bool Record::HasSubrecord(std::string srid)
{
    return m_subrecords.find(srid) != m_subrecords.end();
}

std::unordered_map<std::string, std::unordered_map<std::string, RecordDataType>>
    RecordToSubrecordTypes =
        {
            {"TES3", {
                         {"HEDR", RecordDataType::Data},
                         {"MAST", RecordDataType::String},
                         {"DATA", RecordDataType::Data},
                     }},
            {"GMST", {
                         {"NAME", RecordDataType::String},
                         {"STRV", RecordDataType::String},
                         {"INTV", RecordDataType::Int32},
                         {"FLTV", RecordDataType::Float},
                     }},
            {"GLOB", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::Int8},
                         {"FLTV", RecordDataType::Float},
                     }},
            {"CLAS", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"CLDT", RecordDataType::Data},
                         {"DESC", RecordDataType::String},
                     }},
            {"FACT", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"RNAM", RecordDataType::String},
                         {"FADT", RecordDataType::Data},
                         {"ANAM", RecordDataType::String},
                         {"INTV", RecordDataType::Int32},
                     }},
            {"RACE", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"RADT", RecordDataType::Data},
                         {"NPCS", RecordDataType::String},
                         {"DESC", RecordDataType::String},
                     }},
            {"SOUN", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"DATA", RecordDataType::Data},
                     }},
            {"SKIL", {
                         {"INDX", RecordDataType::Int32},
                         {"SKDT", RecordDataType::Data},
                         {"DESC", RecordDataType::String},
                     }},
            {"MGEF", {
                         {"INDX", RecordDataType::Int32},
                         {"MEDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"PTEX", RecordDataType::String},
                         {"CVFX", RecordDataType::String},
                         {"BVFX", RecordDataType::String},
                         {"HVFX", RecordDataType::String},
                         {"AVFX", RecordDataType::String},
                         {"DESC", RecordDataType::String},
                         {"CSND", RecordDataType::String},
                         {"BSND", RecordDataType::String},
                         {"HSND", RecordDataType::String},
                         {"ASND", RecordDataType::String},
                     }},
            {"SCPT", {
                         {"SCHT", RecordDataType::Data},
                         {"SCVR", RecordDataType::String},
                         {"SCDT", RecordDataType::String}, // It's *technically* data, but it can be read as a string since we're discarding it anyway and it's more convenient this way
                         {"SCTX", RecordDataType::String},
                     }},
            {"REGN", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"WEAT", RecordDataType::Data},
                         {"BNAM", RecordDataType::String},
                         {"CNAM", RecordDataType::Data},
                         {"SNAM", RecordDataType::Data},
                     }},
            {"BSGN", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"TNAM", RecordDataType::String},
                         {"DESC", RecordDataType::String},
                         {"NPCS", RecordDataType::String},
                     }},
            {"LTEX", {
                         {"NAME", RecordDataType::String},
                         {"INTV", RecordDataType::Data},
                         {"DATA", RecordDataType::String},
                     }},
            {"STAT", {
                         {"NAME", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                     }},
            {"DOOR", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"SCIP", RecordDataType::String},
                         {"SNAM", RecordDataType::String},
                         {"ANAM", RecordDataType::String},
                     }},
            {"MISC", {
                         {"NAME", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MCDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"ENAM", RecordDataType::Data},
                         {"SCRI", RecordDataType::String},
                     }},
            {"WEAP", {
                         {"NAME", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"WPDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"ENAM", RecordDataType::Data},
                         {"SCRI", RecordDataType::String},
                     }},
            {"CONT", {
                         {"NAME", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"CNDT", RecordDataType::Float},
                         {"FLAG", RecordDataType::Int32}, // Actually a 4-byte bit field, but more convenient to read out like this
                         {"NPCO", RecordDataType::Data},
                     }},
            {"SPEL", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"SPDT", RecordDataType::Data},
                         {"ENAM", RecordDataType::Data},
                     }},
            {"CREA", {
                         {"NAME", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"NPDT", RecordDataType::Data},
                         {"FLAG", RecordDataType::Data},
                         {"SCRI", RecordDataType::String},
                         {"NPCO", RecordDataType::Data},
                         {"AIDT", RecordDataType::Data},
                         {"AI_W", RecordDataType::Data},
                         {"AI_T", RecordDataType::Data},
                         {"AI_F", RecordDataType::Data},
                         {"AI_E", RecordDataType::Data},
                         {"AI_A", RecordDataType::Data},
                         {"XSCL", RecordDataType::Float},
                     }},
            {"BODY", {
                         {"BYDT", RecordDataType::Int32}, // Actually four single bytes, but more convenient to read out like this
                     }},
            {"LIGH", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"LHDT", RecordDataType::Data},
                         {"SCPT", RecordDataType::String},
                         {"ITEX", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"SNAM", RecordDataType::String},
                     }},
            {"ENCH", {
                         {"NAME", RecordDataType::String},
                         {"ENDT", RecordDataType::Data},
                         {"ENAM", RecordDataType::Data},
                     }},
            {"NPC_", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"RNAM", RecordDataType::String},
                         {"ANAM", RecordDataType::String},
                         {"BNAM", RecordDataType::String},
                         {"CNAM", RecordDataType::String},
                         {"KNAM", RecordDataType::String},
                         {"NPDT", RecordDataType::Data},
                         {"FLAG", RecordDataType::Int32}, // Actually a 4-byte bit field, but more convenient to read out like this
                         {"NPCO", RecordDataType::Data},
                         {"NPCS", RecordDataType::String},
                         {"AIDT", RecordDataType::Data},
                         {"AI_W", RecordDataType::Data},
                         {"AI_T", RecordDataType::Data},
                         {"AI_F", RecordDataType::Data},
                         {"AI_E", RecordDataType::Data},
                         {"AI_A", RecordDataType::Data},
                         {"CNDT", RecordDataType::String},
                         {"DODT", RecordDataType::Data},
                         {"DNAM", RecordDataType::String},
                         {"XSCL", RecordDataType::Float},
                     }},
            {"ARMO", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"AODT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"INDX", RecordDataType::Int8},
                         {"BNAM", RecordDataType::String},
                         {"CNAM", RecordDataType::String},
                         {"ENAM", RecordDataType::Data},
                         {"SCRI", RecordDataType::String},
                     }},
            {"CLOT", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"CTDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"INDX", RecordDataType::Int8},
                         {"BNAM", RecordDataType::String},
                         {"CNAM", RecordDataType::String},
                         {"ENAM", RecordDataType::Data},
                         {"SCRI", RecordDataType::String},
                     }},
            {"REPA", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"RIDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"SCRI", RecordDataType::String},
                     }},
            {"ACTI", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"SCRI", RecordDataType::String},
                     }},
            {"APPA", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"AADT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"SCRI", RecordDataType::String},
                     }},
            {"LOCK", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"LKDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"SCRI", RecordDataType::String},
                     }},
            {"PROB", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"PBDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"SCRI", RecordDataType::String},
                     }},
            {"INGR", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"IRDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"SCRI", RecordDataType::String},
                     }},
            {"BOOK", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"BKDT", RecordDataType::Data},
                         {"ITEX", RecordDataType::String},
                         {"SCRI", RecordDataType::String},
                     }},
            {"ALCH", {
                         {"NAME", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"MODL", RecordDataType::String},
                         {"ALDT", RecordDataType::Data},
                         {"ENAM", RecordDataType::Data},
                         {"TEXT", RecordDataType::String},
                         {"SCRI", RecordDataType::String},
                     }},
            {"LEVI", {
                         {"NAME", RecordDataType::String},
                         {"DATA", RecordDataType::Int32}, // Actually two int16s
                         {"NNAM", RecordDataType::Int8},
                         {"INDX", RecordDataType::Int32},
                         {"INAM", RecordDataType::String},
                         {"INTV", RecordDataType::Int16},
                     }},
            {"LEVC", {
                         {"NAME", RecordDataType::String},
                         {"DATA", RecordDataType::Int32},
                         {"NNAM", RecordDataType::Int8},
                         {"INDX", RecordDataType::Int32},
                         {"CNAM", RecordDataType::String},
                         {"INTV", RecordDataType::Int16},
                     }},
            {"CELL", {
                         {"NAME", RecordDataType::String},

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
                     }},
            {"LAND", {
                         {"INTV", RecordDataType::Int64}, // Actually two int32s
                         {"DATA", RecordDataType::Int32},
                         {"VNML", RecordDataType::Data},
                         {"VHGT", RecordDataType::Data},
                         {"WNAM", RecordDataType::Data},
                         {"VCLR", RecordDataType::Data},
                         {"VTEX", RecordDataType::Data},
                     }},
            {"PGRD", {/* Nothing to see here, folks! */}},
            {"SNDG", {
                         {"NAME", RecordDataType::String},
                         {"DATA", RecordDataType::Int32},
                         {"SNAM", RecordDataType::String},
                         {"CNAM", RecordDataType::String},
                     }},
            {"DIAL", {
                         {"NAME", RecordDataType::String},
                         {"DATA", RecordDataType::Int8},
                     }},
            {"INFO", {
                         {"INAM", RecordDataType::String},
                         {"PNAM", RecordDataType::String},
                         {"NNAM", RecordDataType::String},
                         {"DATA", RecordDataType::Data},
                         {"ONAM", RecordDataType::String},
                         {"RNAM", RecordDataType::String},
                         {"CNAM", RecordDataType::String},
                         {"FNAM", RecordDataType::String},
                         {"ANAM", RecordDataType::String},
                         {"DNAM", RecordDataType::String},
                         {"NAME", RecordDataType::String},
                         {"SNAM", RecordDataType::String},
                         {"QSTN", RecordDataType::Int8},
                         {"QSTF", RecordDataType::Int8},
                         {"QSTR", RecordDataType::Int8},
                         {"SCVR", RecordDataType::String},
                         {"INTV", RecordDataType::Int64}, // Actually two int32s
                         {"FLTV", RecordDataType::Int64},
                         {"BNAM", RecordDataType::String},
                     }},
};
