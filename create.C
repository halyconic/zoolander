#include "catalog.h"


const Status RelCatalog::createRel(const string & relation,
				   const int attrCnt,
				   const attrInfo attrList[])
{
    Status status;
    RelDesc rd;
    AttrDesc ad;

    if (relation.empty() || attrCnt < 1)
    {
        return BADCATPARM;
    }
    if (relation.length() >= sizeof rd.relName)
    {
        return NAMETOOLONG;
    }
    // check if the passed relation string already exists as a relation
    status = getInfo(relation, rd);
    if(status == OK)
    {
        return RELEXISTS;
    }
    //set up RelDesc and add it to relcat
    strcpy(rd.relName, relation.c_str());
    rd.attrCnt = attrCnt;
    status = addInfo(rd);
    if(status != OK)
    {
        return status;
    }
    //set up all AttrDescs and add them to attrcat
    for(int i = 0; i < attrCnt; i++)
    {
        attrInfo ai = attrList[i];

        strcpy(ad.relName, ai.relName);
        strcpy(ad.attrName, ai.attrName);
        ad.attrLen = ai.attrLen;
        ad.attrType = ai.attrType;
        /*int intAttrValue = (int)ai.attrValue;
        int intAttrCat = (int)attrCat;
        ad.attrOffset = intAttrValue - intAttrCat;*/
        attrCat->addInfo(ad);
    }
    //create a HeapFile instance to hold tuples of the relation
    status = createHeapFile(relation);
    if(status != OK)
    {
        return status;
    }
    return OK;
}

