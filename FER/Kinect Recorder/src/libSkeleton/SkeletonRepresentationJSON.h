#pragma once
/********************************************************************
	created:	2014/09/30
	filename: 	SkeletonRepresentationJSON.h
	file path:	libSkeleton\
	file base:	SkeletonRepresentationJSON
	file ext:	h
	author:		Gregorij Kurillo
	
	purpose:	JSON Serializer of the skeleton structures for network communications
	comment:	This is not working yet.... 
*********************************************************************/

#include "serializer.h"
#include "Skeleton.h"
#include <Eigen/Geometry>

/*********************************************************************************/
/* This class reflects Stepan's PoseRepresentation class using Eigen::Vector     */
/*********************************************************************************/
class SkeletonRepresentationJSON : public SkeletonRepresentation, IJsonSerializable
{
public:
	SkeletonRepresentationJSON(double version = 1.0);
	~SkeletonRepresentationJSON(void);

	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);

private:
	Eigen::Vector3f jsonGetPoint3(const Json::Value &jsonValue);
	Json::Value jsonAddPoint3(Eigen::Vector3f point);

};


class SkeletonRepresentationSetJSON : public SkeletonSet, IJsonSerializable
{
public:
	SkeletonRepresentationSetJSON() {}

	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);


};


class SkeletonRepresentationSequenceJSON : public SkeletonSet, IJsonSerializable
{
public:
	SkeletonRepresentationSequenceJSON();

	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);


};

