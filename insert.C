
#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.
 *
 * relation - the table being inserted into
 * attrCnt - how many attributes are in the inserted tuple
 * attrList[] - the actual values and other related data of the inserted tuple
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation,
	const int attrCnt,
	const attrInfo attrList[])
{
    // part 6
    InsertFileScan* ifs;
    Status status;
    Record insertRec;
    RID rid;
    int reclen = 0;



    // find how many attributes relation has, and make sure it matches up correctly
    // with attrCnt
    AttrDesc* attrs;
    int numAttrsInCat;
    status = attrCat->getRelInfo(relation, numAttrsInCat, attrs);
    if(numAttrsInCat != attrCnt)
    {
        free(attrs);
        return ATTRTYPEMISMATCH;
    }
    // calculate the tuple length for the relation
    for(int i = 0; i < numAttrsInCat; i ++)
    {
    	reclen += attrs[i].attrLen;
    }

    // set up the record we'll be adding as a new tuple
    char outputData[reclen];
    insertRec.data = (void *) outputData;
    insertRec.length = reclen;

    // for each attrDesc in attrs
    // find the passed attrInfo that corresponds to it (could be in wrong order), and insert into Record
    int outputOffset = 0;
    for(int i = 0; i < numAttrsInCat; i++)
    {
        AttrDesc ithAttr = attrs[i];
        int test = -1;
        for(int j = 0; j < attrCnt; j++)
        {
            attrInfo jthAttr = attrList[j];
            if(jthAttr.attrValue == 0)
            {
                free(attrs);
                return ATTRTYPEMISMATCH;
            }
            // does attrList[j] match attrs[i]?  if yes, attrList[j] is next attribute to add
            // to the record
            if((strcmp(ithAttr.attrName,jthAttr.attrName) == 0) && ithAttr.attrType == jthAttr.attrType)
            {
            	j = attrCnt;
                test = 0;
                //make sure numbers are the actual numbers and not strings representing numbers
                if(ithAttr.attrType == INTEGER)
                {
                	int intVal = atoi((char*)jthAttr.attrValue);
                	memcpy(outputData + outputOffset, &intVal, ithAttr.attrLen);
                }
                else if(ithAttr.attrType == FLOAT)
                {
                	float floatVal = atof((char*)jthAttr.attrValue);
                	memcpy(outputData + outputOffset, &floatVal, ithAttr.attrLen);
                }
                else
                {
                	 memcpy(outputData + outputOffset, (char*)jthAttr.attrValue, ithAttr.attrLen);
                }
                outputOffset += ithAttr.attrLen;
            }
        }
        // if we passed through attrList[] and found no matches for attrs[i]
        if(test == -1)
        {
            free(attrs);
            return ATTRTYPEMISMATCH;
        }
    }
     // set up InsertFileScan and insert the constructed record
    ifs = new InsertFileScan(relation, status);
    status = ifs->insertRecord(insertRec, rid);
    if(status != OK)
    {
        delete ifs;
        free(attrs);
        return status;
    }

    delete ifs;
    free(attrs);
    return OK;

}


