#pragma once
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable : 4200)

struct TypeDescriptor
{
	const void* pVFTable;
	void* spare;
	char name[0];
};

struct PMD
{
	int mdisp;
	int pdisp;
	int vdisp;
};

struct RTTIBaseClassDescriptor
{
	TypeDescriptor* pTypeDescriptor;
	DWORD numContainedBases;
	PMD memberDisplacement;
	DWORD attributes;
}; typedef RTTIBaseClassDescriptor* RTTIBaseClassArray;

struct RTTIClassHierarchyDescriptor
{
	DWORD signature;
	DWORD attributes;
	DWORD numBaseClasses;
	RTTIBaseClassArray* pBaseClassArray;
};

struct RTTICompleteObjectLocator
{
	DWORD signature; //always zero ? 
	DWORD offset;    //offset of this vtable in the complete class
	DWORD cdOffset;  //constructor displacement offset
	TypeDescriptor* pTypeDescriptor; //TypeDescriptor of the complete class
	RTTIClassHierarchyDescriptor* pClassDescriptor; //describes inheritance hierarchy
};
#pragma warning(pop)