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
    int countAttrLen = 0;
    for(int i = 0; i < attrCnt; i++)
    {
        countAttrLen += attrList[i].attrLen;
    }
    if(countAttrLen >= PAGESIZE)
    {
        return ATTRTOOLONG;
    }
    //make sure no duplicate attribute names
    for(int i = 0; i < attrCnt; i++)
    {
        for(int j = 0; j < attrCnt; j++)
        {
            if(i != j)
            {
                attrInfo attrInfoI = attrList[i];
                attrInfo attrInfoJ = attrList[j];
                if(strcmp(attrInfoI.attrName, attrInfoJ.attrName) == 0)
                {
                    return DUPLATTR;
                }
            }
        }
    }


    // check if the passed relation string already exists as a relation
    status = getInfo(relation, rd);
    if(status == OK)
    {
        return RELEXISTS;
    }
    // add a tuple to the relcat relation
    // set up RelDesc and add it to relcat
    strcpy(rd.relName, relation.c_str());
    rd.attrCnt = attrCnt;
    status = addInfo(rd);
    if(status != OK)
    {
        return status;
    }
    //set up all AttrDescs and add them to attrcat
    int offset = 0;
    for(int i = 0; i < attrCnt; i++)
    {
        attrInfo ai = attrList[i];

        strcpy(ad.relName, ai.relName);
        strcpy(ad.attrName, ai.attrName);
        ad.attrLen = ai.attrLen;
        ad.attrType = ai.attrType;
        ad.attrOffset = offset;
        offset += ad.attrLen;
        status = attrCat->addInfo(ad);
        if(status != OK)
            return status;
    }
    //create a HeapFile instance to hold tuples of the relation
    status = createHeapFile(relation);
    if(status != OK)
    {
        return status;
    }
    return OK;
}

