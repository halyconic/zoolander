#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "stdio.h"
#include "stdlib.h"

// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Inputs:
 *  result - address of the name of the result file
 *  projCnt - the number of projections
 *  projNames - an array of the attrInfo for the profections
 *  attr - the type for the selection attribute
 *  op - the operation to consider in selection
 *  attrValue -the actual attribute value for the selection
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[],
		       const attrInfo *attr, 
		       const Operator op, 
		       const char *attrValue)
{
    // Qu_Select sets up things and then calls ScanSelect to do the actual work
    cout << "Doing QU_Select " << endl;
	Status status;

	// go through the projection list and look up each in the 
    // attr cat to get an AttrDesc structure (for offset, length, etc)
    AttrDesc attrDescArray[projCnt];
    for (int i = 0; i < projCnt; i++)
    {
        Status status = attrCat->getInfo(projNames[i].relName,
                                         projNames[i].attrName,
                                         attrDescArray[i]);
        if (status != OK)
        {
            return status;
        }
    } 
    
	if(attr != NULL){
		// get AttrDesc structure for the select attribute
    	AttrDesc attrDescTemp;
    	status = attrCat->getInfo(attr->relName,
                                     attr->attrName,
                                     attrDescTemp);
    	if (status != OK)
    	{
    	    return status;
    	}
	
	}
	else{
		AttrDesc attrDescTemp = NULL;
	}

	// get output record length from attrdesc structures
    int reclen = 0;
    for (int i = 0; i < projCnt; i++)
    {
        reclen += attrDescArray[i].attrLen;
    }	
	status = ScanSelect(result, projCnt, attrDescArray, &attrDescTemp, op, attrValue, reclen);
		
	if (status != OK)
    {
        return status;
    }
	
	return OK;
}


const Status ScanSelect(const string & result,
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{
    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;
	Status status;
	int resultTupCnt = 0;

	// open the result table
    InsertFileScan resultRel(result, status);
    if (status != OK){
		return status; 
	}

	// setup output data
	char outputData[reclen];
    Record outputRec;
    outputRec.data = (void *) outputData;
    outputRec.length = reclen;
	
	if(attrDesc == NULL){
		HeapFileScan scan(string(attrDesc->relName), status);
    	if (status != OK){
			return status; 
		}
	
    	status = scan.startScan(attrDesc->attrOffset, // Offset
                            attrDesc->attrLen, // Length
                            (Datatype)(attrDesc->attrType), // data type
                            filter, // filter
                            op); //Operator
    	if (status != OK) {
			return status; 
		}
	}
	else{
		HeapFileScan scan(projNames[0].relName, status);
    	if (status != OK){
			return status; 
		}
    	status = scan.startScan(0, 0, STRING, NULL, EQ);
    	if (status != OK) {
			return status; 
		}
	}
	
    RID scanRID;
    Record scanRec;
	
	while (scan.scanNext(scanRID) == OK)
    {
        status = scan.getRecord(scanRec);
        ASSERT(status == OK);

        // we have a match, copy data into the output record
        int outputOffset = 0;
        for (int i = 0; i < projCnt; i++)
        {
        // copy the data out of the proper input file
            memcpy(outputData + outputOffset, (char *)scanRec.data +
				   projNames[i].attrOffset, projNames[i].attrLen);
        
		outputOffset += projNames[i].attrLen;
        
		// add the new record to the output relation
        RID outRID;
        status = resultRel.insertRecord(outputRec, outRID);
        ASSERT(status == OK);
        resultTupCnt++;
        }
    }
    status = scan.endScan();
     if (status != OK) {
		return status; 
	}
    
	printf("Select produced %d result tuples \n", resultTupCnt);
    return OK;
}
