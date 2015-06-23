#include "libSkeleton.h"
#include "serializer.h"
#include <json/json.h>



bool CJsonSerializer::Serialize( IJsonSerializable* pObj, std::string& output )
{
	if (pObj == NULL)
		return false;

	Json::Value serializeRoot;
	pObj->Serialize(serializeRoot);

	Json::StyledWriter writer;
	output = writer.write( serializeRoot );

	return true;
}

bool CJsonSerializer::Deserialize( IJsonSerializable* pObj, std::string& input )
{
	if (pObj == NULL)
		return false;

	Json::Value deserializeRoot;
	Json::Reader reader;

	if ( !reader.parse(input, deserializeRoot) )
		return false;

	pObj->Deserialize(deserializeRoot);

	return true;
}