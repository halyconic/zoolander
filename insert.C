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
	rec.data = NULL;
	rec.length = 0;

	/*
	 * Execute scan
	 */

	status = ifs->insertRecord(rec, rid);
	if (status != OK) return status;

	// part 6
	return OK;
}

