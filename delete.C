#include "catalog.h"
#include "query.h"


/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation,
		       const string & attrName,
		       const Operator op,
		       const Datatype type,
		       const char *attrValue)
{
// part 6
    Status status;
    HeapFileScan*  hfs;
    RID rid;
    AttrDesc record;

    attrCat->getInfo(relation, attrName, record);
    hfs = new HeapFileScan(relation, status);
    if (status != OK) return status;

    if(type == INTEGER)
    {
    	int intVal = atoi(attrValue);
    	status = hfs->startScan(record.attrOffset, record.attrLen, type,
    	              (char*)(&intVal), op);
    }
    else if(type == FLOAT)
    {
    	float floatVal = atof(attrValue);
    	status = hfs->startScan(record.attrOffset, record.attrLen, type,
    	              (char*)(&floatVal), op);
    }
    else
    {
    	status = hfs->startScan(record.attrOffset, record.attrLen, type,
    	              attrValue, op);
    }
    if (status != OK)
    {
        delete hfs;
        return status;
    }

    // find all tuples that match the search
    while((status = hfs->scanNext(rid)) != FILEEOF)
    {
        // remove the tuple
        status = hfs->deleteRecord();
        if(status != OK) return status;
    }

    delete hfs;
    return OK;
}


