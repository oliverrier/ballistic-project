#pragma once
#include <string>

class RTTI
{
public:
	virtual size_t TypeIdInstance() const = 0;

	virtual bool Is(const std::string& name) const
	{
		return false;
	}

	virtual bool Is(const size_t& id) const
	{
		return false;
	}

	template<typename T>
	T* As()
	{
		return (Is(T::TypeIdClass())) ? static_cast<T*>(this) : nullptr;
	}

	template<typename T>
	T* As() const
	{
		return (Is(T::TypeIdClass())) ? static_cast<T*>(this) : nullptr;
	}

	virtual RTTI* QueryParent(const size_t& id)
	{
		return nullptr;
	}

	virtual RTTI* QueryParent(const size_t& id) const
	{
		return nullptr;
	}
};

#define DECLARE_RTTI(ClassType, ParentType)								        	 \
	public:																			 \
		static const std::string TypeName()											 \
		{																			 \
			return {#ClassType};													 \
		}																			 \
																					 \
		size_t TypeIdInstance() const override										 \
		{																			 \
			return ClassType::TypeIdClass();										 \
		}																			 \
																					 \
		static const size_t TypeIdClass()											 \
		{																			 \
			static int id = 0;														 \
			return (size_t)(&id);													 \
		}																			 \
																					 \
		bool Is(const size_t& id) const override									 \
		{																			 \
			return (id == TypeIdClass()) ? true : ParentType::Is(id);				 \
		}																			 \
																					 \
		bool Is(const std::string& name) const override								 \
		{																			 \
			return (name == TypeName()) ? true : ParentType::Is(name);				 \
		}																			 \
																					 \
		RTTI* QueryParent(const size_t& id) override								 \
		{																			 \
			return (id == TypeIdClass()) ? (RTTI*)this : ParentType::QueryParent(id);\
		}																			 \
																					 \
		RTTI* QueryParent(const size_t& id) const override							 \
		{																			 \
			return (id == TypeIdClass()) ? (RTTI*)this : ParentType::QueryParent(id);\
		}																			 \
