#pragma once

#include <json/json.h>

/************************************************************************/
/* Serializer for JSONCPP                                               */
/* http://www.danielsoltyka.com/programming/2011/04/15/simple-class-serialization-with-jsoncpp/
/************************************************************************/
class IJsonSerializable
{
public:
	virtual ~IJsonSerializable( void ) {};
	virtual void Serialize( Json::Value& root ) =0;
	virtual void Deserialize( Json::Value& root) =0;
};

class CJsonSerializer
{
public:
	static bool Serialize( IJsonSerializable* pObj, std::string& output );
	static bool Deserialize( IJsonSerializable* pObj, std::string& input );

private:
	CJsonSerializer( void ) {};
};