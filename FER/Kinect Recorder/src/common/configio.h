#ifndef CONFIG_IO_H_INCLUDED
#define CONFIG_IO_H_INCLUDED

#include <vector>
#include <string>
#include <sstream>

#include <cstdio>
#include <map>

#define BeginEnumDefinition(EnumName) struct EnumerationMap##EnumName : public EnumerationMap { EnumerationMap##EnumName() { enum_type_name = #EnumName;

#define EnumItem(EnumItem) forward_map.insert(forward_map_type::value_type(#EnumItem, EnumItem)); backward_map.insert(backward_map_type::value_type(EnumItem, #EnumItem));

#define EndEnumDefinition(EnumName) }} EnumerationMapInstance##EnumName;


// ENUM name values support class
struct EnumerationMap
{
public:
   typedef std::map<std::string, int> forward_map_type;
   typedef std::map<int, std::string> backward_map_type;
   forward_map_type forward_map;
   backward_map_type backward_map;
   const char *enum_type_name;
public:
   EnumerationMap()
   {
      enum_type_name = "";
   }

   void dump()
      {
         printf("Enum '%s':\n", enum_type_name);
         for (forward_map_type::iterator it=forward_map.begin();it!=forward_map.end();it++)
            printf("%s -> %d\n", it->first.c_str(), it->second);

         for (backward_map_type::iterator jt=backward_map.begin();jt!=backward_map.end();jt++)
            printf("%d -> %s\n", jt->first, jt->second.c_str());
      }
   // check if exist's such value
   bool is_value(int value)
      {
         return (backward_map.find(value)!=backward_map.end());
      }

   // check if exist's such value name
   bool is_valuename(const char *name)
      {
         return (forward_map.find(name)!=forward_map.end());
      }

   const char *get_name(int value)
   {
      backward_map_type::iterator it=backward_map.find(value);
      if (it!=backward_map.end())
         return it->second.c_str();
      else
         printf("Invalid enum '%s' value %d\n", enum_type_name, value);
      return "Invalid value";
   }

   int get_value(const char *name)
   {
      forward_map_type::iterator it=forward_map.find(name);
      if (it!=forward_map.end())
         return (int)it->second;
      else
         printf("Invalid enum '%s' item name %s\n", enum_type_name, name);
      return -1;
   }
};

using namespace std;

/// \brief A class allowing for easy reading/writing of configuration files

/// A derived structure calls ADD_PARAMETER macro to register its members
/// which are then read/written to a file by callinf load or save member function
/// Usage: 1. derive a structure, its members holding required parameters,
///        2. In the constructor, invoke ADD_PARAMETER macro for each of the parameters
///        3. use methods load and save for file reading/writing
/// See Parameters struct for an example of usage, and a lafs.cfg file for the file format

struct ConfigIO
{
   enum ParamType { UNKNOWN, INT, UNSIGNED, BOOL, DOUBLE, STRING, ENUM };

   struct ParamInfo   /// holds name of, and pointer to one registered member variable of the derived class
   {
      ParamInfo (ParamType t, const char *n, EnumerationMap *m = 0) : type (t), name (n), enum_map(m) { };
      ParamType type;
      const char *name;
      EnumerationMap *enum_map;

      union
      {
         int *pInt;
         unsigned *pUnsigned;
         bool *pBool;
         double *pDouble;
         string *pString;

         int vInt;
         unsigned vUnsigned;
         bool vBool;
         double vDouble;
      };
   };

   /// Return string of the parameter type
   static std::string getTypeAsString(ParamInfo param) 
   {
	   switch (param.type)
	   {
	   case INT:
		   return "Int";
	   case UNSIGNED:
		   return "Unsigned";
	   case BOOL:
		   return "Bool";
	   case DOUBLE:
		   return "Double";
	   case STRING:
		   return "String";
	   case ENUM:
		   return "Enum";
	   default:
		   return "Unknown";
	   }
   }
   
   /// Return valu as string
   static std::string getValueAsString(ParamInfo param) 
   {
	   std::stringstream ss;
	   switch (param.type)
	   {
	   case INT:
		   ss << *param.pInt;
		   break;
	   case UNSIGNED:
		   ss << *param.pUnsigned;
		   break;
	   case BOOL:
		   ss << *param.pBool;
		   break;
	   case DOUBLE:
		   ss << *param.pDouble;
		   break;
	   case STRING:
		   ss << *param.pString;
		   break;
	   case ENUM:
		   ss << *param.pInt;
		   break;
	   default:
		   ss << "";
	   }
	   return ss.str();
   }

   vector <ParamInfo> parameters;    ///< A container of registered members, which are written/read from/to the file
   vector <ParamInfo> checks;        ///< A container of registered members, which are written/read from/to the file and compared at runtime to compile options

   virtual bool save (const char *filename);    ///< Save the configuration file
   virtual bool load (const char *filename);    ///< Load and parse the configuration file
   virtual void print ();    ///< Print out all parameters
   virtual bool parseArgs (int nArgs, const char **argv);  ///< indexed from 0, meant to interpret command line parameters: parseArgs (argc - 1, argv + 1) to skip program name

   virtual ~ConfigIO() {};

   void addParameter (int &param, const char *name);
   void addParameter (unsigned &param, const char *name);
   void addParameter (bool &param, const char *name);
   void addParameter (double &param, const char *name);
   void addParameter (string &param, const char *name);

   void addCheck (int param, const char *name);
   void addCheck (unsigned param, const char *name);
   void addCheck (bool param, const char *name);
   void addCheck (double param, const char *name);

   void addEnumParameter (int &param, const char *name, EnumerationMap *m);

protected:
	#define ADD_PARAMETER(parameter) addParameter (parameter, #parameter)
	#define ADD_PARAMETER_VAL(parameter,defaultValue) { addParameter (parameter, #parameter); parameter = defaultValue; }
	#define ADD_ENUM_PARAMETER(EnumName, parameter) addEnumParameter (*(int *) &parameter, #parameter, &EnumerationMapInstance##EnumName)
	#define ADD_CHECK(parameter) addCheck (parameter, #parameter)

   void trimWhiteSpace (char *ptr);
   void loadParam (const char *line);

};



#endif // CONFIG_IO_H_INCLUDED
