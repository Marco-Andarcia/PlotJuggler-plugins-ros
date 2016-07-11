#ifndef ROSTYPEPARSER_H
#define ROSTYPEPARSER_H

#include <vector>
#include <string.hpp>
#include <map>
#include <boost/utility/string_ref.hpp>

namespace RosTypeParser{

typedef sso23::string String;


typedef struct
{
    String type_name;
    String field_name;

} RosTypeField;

class RosType
{
public:
    std::string full_name;
    std::vector<RosTypeField> fields;

    RosType() {}

    RosType( std::string name):
        full_name(name) { }
};

typedef std::map<String, RosType> RosTypeMap;

typedef struct{
    std::map<String, double> value_renamed;
    std::map<String, double> value;
    std::map<String, String> name_id;

}RosTypeFlat;

//------------------------------
std::ostream& operator<<(std::ostream& s, const RosTypeFlat& c);

void parseRosTypeDescription(
        const std::string & type_name,
        const std::string & msg_definition,
        RosTypeMap* type_map);

void printRosTypeMap( const RosTypeMap& type_map );
void printRosType(const RosTypeMap& type_map, const std::string& type_name, int indent = 0 );

void buildRosFlatType(const RosTypeMap& type_map,
                       const String &type_name,
                       String prefix,
                       uint8_t **buffer_ptr,
                       RosTypeFlat* flat_container);


class SubstitutionRule{
public:
    SubstitutionRule( std::string pattern, std::string name_location, std::string substitution);

    std::string pattern_suf;
    std::string pattern_pre;

    std::string location_suf;
    std::string location_pre;

    std::string substitution_suf;
    std::string substitution_pre;
private:

};

typedef std::map<std::string, SubstitutionRule> SubstitutionRuleSet;

void applyNameTransform(const std::vector<SubstitutionRule> &rules,
                         RosTypeFlat* container);

}

#endif // ROSTYPEPARSER_H
