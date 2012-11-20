#include "catalog.h"
#include <stdio.h>
#include <stdlib.h>

//
// Destroys a relation. It performs the following steps:
//
// 	removes the catalog entry for the relation
// 	destroys the heap file containing the tuples in the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::destroyRel(const string & relation)
{
  Status status;

  if (relation.empty() ||
      relation == string(RELCATNAME) ||
      relation == string(ATTRCATNAME))
    return BADCATPARM;

    // removes schema info from relcat and attrcat relations
    status = removeInfo(relation);
    if(status != OK)
    {
        return status;
    }
    status = attrCat->dropRelation(relation);
    if(status != OK)
    {
        return status;
    }

    //destroys the heap file containing the tuples in the relation
    status = destroyHeapFile(relation);
    if(status != OK)
    {
        return status;
    }
    return OK;
}

//
// Drops a relation. It performs the following steps:
//
// 	removes the catalog entries for the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status AttrCatalog::dropRelation(const string & relation)
{
  Status status;
  AttrDesc *attrs;
  int attrCnt, j;

  if (relation.empty()) return BADCATPARM;

  status = getRelInfo(relation, attrCnt, attrs);
  if(status != OK)
    return status;

  HeapFileScan* hfs = new HeapFileScan(ATTRCATNAME, status);
  char* relChars = (char*)relation.c_str();
  status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ);
  if(status != OK)
    return status;
  for(j = 0; j < attrCnt; j++)
  {
      RID rid;
      status = hfs->scanNext(rid);
      if(status != OK)
        return status;
      status = hfs->deleteRecord();
      if(status != OK)
        return status;
  }
  delete hfs;
  free(attrs);
  return OK;

/*  for (int i = 0; i < attrCnt; i++)
  {
	  HeapFileScan* hfs = new HeapFileScan(relation, status);
	  if (status != OK) return status;
	  status = hfs->startScan(0, sizeof(attrs[i].attrName), STRING, attrs[i].attrName, EQ);
	  if (status != OK) return status;

	  RID rid;
	  status = hfs->scanNext(rid);
	  if (status != OK) return status;
	  status = hfs->deleteRecord();
	  if (status != OK) return status;

	  delete hfs;
  }

  delete attrs;*/
}


