#include "configio.h"
#include <assert.h>
#include <iostream>
#include <fstream>


#ifdef _MSC_VER
  #define strnicmp _strnicmp
#else
  #define strnicmp strncasecmp
#endif

void ConfigIO :: addParameter (int &param, const char *name)
{
  ParamInfo p (INT, name);
  p.pInt = &param;
  parameters.push_back (p);
}

void ConfigIO :: addEnumParameter (int &param, const char *name, EnumerationMap *m)
{
   ParamInfo p (ENUM, name, m);
   p.pInt = &param;
   parameters.push_back (p);
}

void ConfigIO :: addParameter (unsigned &param, const char *name)
{
  ParamInfo p (UNSIGNED, name);
  p.pUnsigned = &param;
  parameters.push_back (p);
}

void ConfigIO :: addParameter (bool &param, const char *name)
{
  ParamInfo p (BOOL, name);
  p.pBool = &param;
  parameters.push_back (p);
}

void ConfigIO :: addParameter (double &param, const char *name)
{
  ParamInfo p (DOUBLE, name);
  p.pDouble = &param;
  parameters.push_back (p);
}

void ConfigIO :: addParameter (string &param, const char *name)
{
  ParamInfo p (STRING, name);
  p.pString = &param;
  parameters.push_back (p);
}

void ConfigIO :: addCheck (int param, const char *name)
{
  ParamInfo p (INT, name);
  p.vInt = param;
  checks.push_back (p);
}

void ConfigIO :: addCheck (unsigned param, const char *name)
{
  ParamInfo p (UNSIGNED, name);
  p.vUnsigned = param;
  checks.push_back (p);
}

void ConfigIO :: addCheck (bool param, const char *name)
{
  ParamInfo p (BOOL, name);
  p.vBool = param;
  checks.push_back (p);
}

void ConfigIO :: addCheck (double param, const char *name)
{
  ParamInfo p (DOUBLE, name);
  p.vDouble = param;
  checks.push_back (p);
}

bool ConfigIO :: save (const char *filename)
{
  ofstream stream (filename);
  if (!stream) return false;

  stream << "# " << endl;
  for (size_t f = 0; f < parameters.size (); f ++)
      {
        if (parameters [f].type == INT)
            stream << parameters [f].name << " = " << *parameters [f].pInt << endl;
        if (parameters [f].type == UNSIGNED)
            stream << parameters [f].name << " = " << *parameters [f].pUnsigned << endl;
        if (parameters [f].type == BOOL)
            stream << parameters [f].name << " = " << *parameters [f].pBool << endl;
        if (parameters [f].type == DOUBLE)
            stream << parameters [f].name << " = " << *parameters [f].pDouble << endl;
        if (parameters [f].type == STRING)
            stream << parameters [f].name << " = \"" << parameters [f].pString -> c_str () << "\"" << endl;
        if (parameters [f].type == ENUM)
           stream << parameters [f].name << " = " << parameters[f].enum_map->get_name( *parameters [f].pInt ) << endl;
      }
  for (size_t f = 0; f < checks.size (); f ++)
      {
        if (checks [f].type == INT)
            stream << checks [f].name << " = " << checks [f].vInt << endl;
        if (checks [f].type == UNSIGNED)
            stream << checks [f].name << " = " << checks [f].vUnsigned << endl;
        if (checks [f].type == BOOL)
            stream << checks [f].name << " = " << checks [f].vBool << endl;
        if (checks [f].type == DOUBLE)
            stream << checks [f].name << " = " << checks [f].vDouble << endl;
      }
  return true;
}

void ConfigIO :: trimWhiteSpace (char *ptr)
{
  char *dest = ptr;
  bool inString = false;
  while (*ptr)
        {
          if (*ptr == '"') inString = !inString;
          if (!isspace (*ptr) || inString) *dest ++ = *ptr;
          ptr ++;
        }
  *dest = '\0';
}


/**
* Pass input arguments, note that argv[0] from the command line should be removed
*/
bool ConfigIO :: parseArgs (int nArgs, const char **argv)
{
	if (nArgs > 0)
		if (_strcmpi(argv [0], "help") == 0 || _strcmpi(argv [0], "?") == 0 || _strcmpi(argv [0], "/h") == 0 || _strcmpi(argv [0], "/?") == 0) {
			std::cout << "------------------------------------------" << std::endl;
			std::cout << "Valid parameters for this application: " << std::endl;
			for (size_t f = 0; f < parameters.size (); f ++)
				std::cout	<< " * " 
				<< getTypeAsString(parameters[f]) 
				<< " "
				<< parameters[f].name 
				<< " ["
				<< getValueAsString(parameters[f]) 
				<< "]" 
				<< std::endl;

			std::cout << "------------------------------------------" << std::endl;
			exit(0);
		}


	for (int i = 0; i < nArgs; i ++)
		loadParam (argv [i]);


	return true;
}


bool ConfigIO :: load (const char *filename)
{
  ifstream stream (filename);
  if (!stream) return false;

  char line [10240];
  while (!stream.eof ())
        {
          stream.getline (line, sizeof (line));
          trimWhiteSpace (line);
          if (line [0] == '\0') continue;
          if (line [0] == '#') continue;
          if (line [0] == '%') continue;
          if (line [0] == ';') continue;

          const char *div = strchr (line, '=');
          if (div == NULL) continue;

          loadParam (line);
        }

  return true;
}


void ConfigIO :: print(void)
{
	for (size_t f = 0; f < parameters.size (); f ++)
	{
		std::cout << parameters[f].name << ": [";
		if (parameters[f].type == BOOL)
			std::cout << *parameters[f].pBool;
		else if (parameters[f].type == INT)
			std::cout << *parameters[f].pInt;
		else if (parameters[f].type == UNSIGNED)
			std::cout << *parameters[f].pUnsigned;
		else if (parameters[f].type == STRING)
			std::cout << *parameters[f].pString;
		else if (parameters[f].type == DOUBLE)
			std::cout << *parameters[f].pDouble;
		else
			std::cout << "Unknown";
		std::cout << "]" << std::endl;
	}
}



void ConfigIO :: loadParam (const char *param)
{
  char line [10240];
  char paramName [10240];
  strncpy_s (line, param, sizeof (line));

  char *div = strchr (line, '=');

  for (size_t f = 0; f < parameters.size (); f ++)
      {
        strcpy_s (paramName, parameters [f].name);
        trimWhiteSpace (paramName);
        if (strnicmp (paramName, line, strlen (paramName)) == 0)
           {
             if (parameters [f].type == INT) *parameters [f].pInt = atoi (div + 1);
             if (parameters [f].type == UNSIGNED) *parameters [f].pUnsigned = atoi (div + 1);
             if (parameters [f].type == BOOL) {
				 std::string boolValue = std::string(div+1);
				 if (boolValue.compare("true") == 0)
					 *parameters [f].pBool = true;
				 else if (boolValue.compare("false") == 0)
					 *parameters [f].pBool = false;
				 else
				 {
					 *parameters [f].pBool = !! atoi (div + 1);
				 }

			 }
             if (parameters [f].type == DOUBLE) *parameters [f].pDouble = atof (div + 1);
             if (parameters [f].type == STRING)
                {
//                  if (div [1] != '"') continue;
//                  if (strchr (div + 2, '"') == NULL) continue;
//                  *strchr (div + 2, '"') = '\0';
//                  *parameters [f].pString = div + 2;
                  if (div [1] == '"')
                     {
                       if (strchr (div + 2, '"') == NULL) continue;
                       *strchr (div + 2, '"') = '\0';
                       *parameters [f].pString = div + 2;
                     }
                  else if (div [1] == '\'')
                     {
                       if (strchr (div + 2, '\'') == NULL) continue;
                       *strchr (div + 2, '\'') = '\0';
                       *parameters [f].pString = div + 2;
                     }
                  else 
                     {  
//                        for (char *p = div + 2; *p != '\0'; p ++)  // already trimmed
//                             if (isspace (*p)) *p = '\0';
                       *parameters [f].pString = div + 1;
                     }
                }
             if (parameters [f].type == ENUM)
             {
                if (strtol(div+1, (char **)NULL, 10)==0)
                {
                   // it is not a number
                   if (parameters [f].enum_map->is_valuename(div+1))
                      *parameters [f].pInt = parameters [f].enum_map->get_value(div+1);
                   else
                      printf("Warning: Invalid enum value name '%s' of '%s', leaving default value.\n",
                             div+1, parameters [f].name); // leave default value_type
                } else
                   *parameters [f].pInt = atoi (div + 1);
             }
           }
      }

  for (size_t f = 0; f < checks.size (); f ++)
      {
        strcpy_s (paramName, checks [f].name);
        trimWhiteSpace (paramName);
        if (strnicmp (paramName, line, strlen (paramName)) == 0)
           {
             if (checks [f].type == INT)
                {
                  int v = atoi (div + 1);
                  if (checks [f].vInt != v) cerr << "Config-file parameter " << paramName << " (" << v << ") does not match to compile time value " << checks [f].vInt << endl;
                  assert (checks [f].vInt == v);
                }
             if (checks [f].type == UNSIGNED)
                {
                  unsigned v = atoi (div + 1);
                  if (checks [f].vUnsigned != v) cerr << "Config-file parameter " << paramName << " (" << v << ") does not match to compile time value " << checks [f].vUnsigned << endl;
                  assert (checks [f].vUnsigned == v);
                }
             if (checks [f].type == BOOL)
                {
                  bool v = !! atoi (div + 1);
                  if (checks [f].vBool != v) cerr << "Config-file parameter " << paramName << " (" << v << ") does not match to compile time value " << checks [f].vBool << endl;
                  assert (checks [f].vBool == v);
                }
             if (checks [f].type == DOUBLE)
                {
                  double v = atof (div + 1);
                  if (checks [f].vDouble != v) cerr << "Config-file parameter " << paramName << " (" << v << ") does not match to compile time value " << checks [f].vDouble << endl;
                  assert (checks [f].vDouble == v);
                }
           }
      }
}






//////////////////////////////////////////////

/*
struct Test : ConfigIO
{
  Test (void);

  double d;
  int i;
  string str;
};


Test :: Test (void)
{
  ADD_PARAMETER (d);
  ADD_PARAMETER (i);
  ADD_PARAMETER (str);
}


void main (void)
{
  Test t;
  t.d = 123.456;
  t.i = 312;
  t.str = "shit";
  t.load ("test2.cfg");
  t.save ("test.cfg");
}
*/
