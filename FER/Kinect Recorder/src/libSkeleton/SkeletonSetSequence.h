#pragma once
/********************************************************************
	created:	2014/10/01
	filename: 	SkeletonSequence.h
	file path:	libSkeleton\
	file base:	SkeletonRepresentation
	file ext:	h
	author:		Gregorij Kurillo
	
	purpose:	Skeleton sequence representation
	comment:
*********************************************************************/

#include "serializer.h"
#include "SkeletonSet.h"

/** 
* Sequence of skeleton sets
*/
class SkeletonSetSequence: public IJsonSerializable
{
public:
	SkeletonSetSequence();
	~SkeletonSetSequence();

	// vector-like behavior
	size_t size() const { return m_v.size(); }
	void push_back(const SkeletonSet &ps) { m_v.push_back(ps); }
	SkeletonSet &operator [] (size_t n) { return (m_v[n]); }
	const SkeletonSet &operator [] (size_t n) const { return (m_v[n]); }
	SkeletonSet & at(size_t n) { return m_v.at(n); }
	const SkeletonSet & at(size_t n) const { return m_v.at(n); }
	void resize(size_t n) { m_v.resize(n); }
	void reserve(size_t n) { m_v.reserve(n); }
	void clear() { m_v.clear(); }

	// i/o functions
	int load(std::string filename);
	int save(std::string filename, io::SkeletonIOFlags flags = io::POSE_IO_NOFLAGS);


private:
	std::vector<SkeletonSet> m_v;	///< Vector of skeletons
	float version;


public:
	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);

private:

};

