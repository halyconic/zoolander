#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
	/*
	 * Initial code, based on getRelInfo and createRel
	 *
	 * Are the right errors used?
	 *
	 * Initialize scan
	 */
	Status status;
	RID rid;
	Record rec;
	InsertFileScan* ifs;

	// if (relation.empty()) return BADCATPARM;

	if (relation.empty() || attrCnt < 1) return BADCATPARM;

	ifs = new InsertFileScan(ATTRCATNAME, status);
	if (status != OK) return status;

	/*
	 * Create record using catalog and offsets
	 */

	rec = Record();
	rec.length = 0;

//	// find total size of record needed
//	for (int i = 0; i < attrCnt; i++)
//	{
//		const attrInfo curr_info = attrList[i];
//
//		if (curr_info.attrLen < 0)
//			return INVALIDRECLEN;
//
//		// Do I need to switch b/w types here and set values?
//		rec.length += curr_info.attrLen;
//	}

	// find total size of record needed
	for (int i = 0; i < attrCnt; i++)
	{
		const attrInfo curr_info = attrList[i];

		switch (curr_info.attrType)
		{
		case INTEGER:
			rec.length += sizeof(int);
			break;
		case FLOAT:
			rec.length += sizeof(float);
			break;
		case STRING:
			ASSERT(curr_info.attrLen >= 0);
			rec.length += curr_info.attrLen;
			break;
		default:
			return BADATTRTYPE;
			break;
		}
	}

	// copy attributes into rec
	rec.data = new char[rec.length];
	int data_index = 0;
	char* data_arr = (char*) rec.data;
	for (int i = 0; i < attrCnt; i++)
	{
		const attrInfo curr_info = attrList[i];

		if (curr_info.attrType == INTEGER)
		{
			const int value_int = atoi((char*) curr_info.attrValue);
			memcpy(&data_arr[data_index], &value_int, sizeof(int));
			data_index += sizeof(int);
		}
		else if (curr_info.attrType == FLOAT)
		{
			const int value_float = atof((char*) curr_info.attrValue);
			memcpy(&data_arr[data_index], &value_float, sizeof(float));
			data_index += sizeof(float);
		}
		else if (curr_info.attrType == STRING)
		{
			ASSERT(curr_info.attrLen >= 0);
			memcpy(&data_arr[data_index], &curr_info.attrValue, curr_info.attrLen);
			data_index += curr_info.attrLen;
		}

		// Assert valid index
		if (data_index > rec.length)
			return INVALIDRECLEN;
	}

	/*
	 * Execute scan
	 */

	status = ifs->insertRecord(rec, rid);
	if (status != OK) return status;

	delete ifs;
	
	// part 6
	return OK;
}

