#pragma once
#include <stdio.h>

namespace Envy
{
	namespace SourceEngine
	{
		enum class SendPropType 
		{
			DPT_Int = 0,
			DPT_Float,
			DPT_Vector,
			DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
			DPT_String,
			DPT_Array,     // An array of the base types (can't be of datatables).
			DPT_DataTable,
			DPT_Int64,
			DPT_NUMSendPropTypes
		};

		class RecvTable;
		class RecvProp;
		class CRecvProxyData;
		class DVariant;

		class RecvProp 
		{
		public:

			char*                    m_pVarName;
			SendPropType             m_RecvType;
			int                      m_Flags;
			int                      m_StringBufferSize;
			bool                     m_bInsideArray;
			const void*              m_pExtraData;
			RecvProp*                m_pArrayProp;
			void*                    m_ArrayLengthProxy;
			void*                    m_ProxyFn;
			void*                    m_DataTableProxyFn;
			RecvTable*               m_pDataTable;
			int                      m_Offset;
			int                      m_ElementStride;
			int                      m_nElements;
			const char*              m_pParentArrayPropName;
		};

		class RecvTable 
		{
		public:
			RecvProp*                m_pProps;
			int                      m_nProps;
			void*                    m_pDecoder;
			char*                    m_pNetTableName;
			bool                     m_bInitialized;
			bool                     m_bInMainList;
		};
		class DVariant
		{
		public:
			DVariant() { m_Type = SendPropType::DPT_Float; }
			DVariant(float val) { m_Type = SendPropType::DPT_Float; m_Float = val; }

			const char *ToString()
			{
				static char text[128];

				switch (m_Type)
				{
				case SendPropType::DPT_Int:
					sprintf_s(text, sizeof(text), "%i", m_Int);
					break;
				case SendPropType::DPT_Float:
					sprintf_s(text, sizeof(text), "%.3f", m_Float);
					break;
				case SendPropType::DPT_Vector:
					sprintf_s(text, sizeof(text), "(%.3f,%.3f,%.3f)",
						m_Vector[0], m_Vector[1], m_Vector[2]);
					break;
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
				case DPT_Quaternion:
					sprintf_s(text, sizeof(text), "(%.3f,%.3f,%.3f %.3f)",
						m_Vector[0], m_Vector[1], m_Vector[2], m_Vector[3]);
					break;
#endif
				case SendPropType::DPT_String:
					if (m_pString)
						return m_pString;
					else
						return "NULL";
					break;
				case SendPropType::DPT_Array:
					sprintf_s(text, sizeof(text), "Array");
					break;
				case SendPropType::DPT_DataTable:
					sprintf_s(text, sizeof(text), "DataTable");
					break;
				default:
					sprintf_s(text, sizeof(text), "DVariant type %i unknown", m_Type);
					break;
				}

				return text;
			}

			union
			{
				float	m_Float;
				int 	m_Int;
				char	*m_pString;
				void	*m_pData;	// For DataTables.
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
				float	m_Vector[4];
#else
				float	m_Vector[3];
#endif
				__int64	m_Int64;
			};
			SendPropType	m_Type;
		};

		class CRecvProxyData
		{
		public:
			const RecvProp	*m_pRecvProp;		// The property it's receiving.

			DVariant		m_Value;			// The value given to you to store.

			int				m_iElement;			// Which array element you're getting.

			int				m_ObjectID;			// The object being referred to.
		};
	}
}