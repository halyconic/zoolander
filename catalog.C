#include "catalog.h"


RelCatalog::RelCatalog(Status &status) :
	 HeapFile(RELCATNAME, status)
{
// nothing should be needed here
}


const Status RelCatalog::getInfo(const string & relation, RelDesc &record)
{
  if (relation.empty())
    return BADCATPARM;

  Status status;
  Record rec;
  RID rid;
  HeapFileScan* hfs;

// Opens scan on relcat relation
  hfs = new HeapFileScan(RELCATNAME, status);
  if(status != OK)
    return status;
  char* relChars = (char*)relation.c_str();
  status = hfs->startScan(0, sizeof(relChars), STRING,relChars, EQ);
  if(status != OK)
    return status;

    //get the desired tuple
  status = hfs->scanNext(rid);
  if(status == FILEEOF)
  {
      return RELNOTFOUND;
  }
  else if(status != OK)
  {
    return status;
  }
  status = hfs->getRecord(rec);
  if(status != OK)
    return status;

  //memcpy() the tuple out of the buffer pool into record
  memcpy(&record,&rec,rec.length);
  delete hfs;
  return OK;
}


const Status RelCatalog::addInfo(RelDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;

//create an InsertFileScan object on the relation catalog table
  ifs = new InsertFileScan(RELCATNAME, status);
  if(status != OK)
  {
      return status;
  }

  // create a record
  Record rec;
  rec.data = (void*)&record;
  rec.length = sizeof(RelDesc);
  //insert the record into the table
  status = ifs->insertRecord(rec, rid);
  if(status != OK)
  {
      return status;
  }
  delete ifs;
  return OK;
}

const Status RelCatalog::removeInfo(const string & relation)
{
  Status status;
  RID rid;
  HeapFileScan*  hfs;

  if (relation.empty()) return BADCATPARM;

  //start a filter scan on relcat to locate the rid of the desired tuple
  hfs = new HeapFileScan(RELCATNAME,status);
  if(status != OK)
    return status;
  char* relChars = (char*)relation.c_str();
  status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ);
  if(status != OK)
    return status;
  status = hfs->scanNext(rid);
  if(status == FILEEOF)
  {
      return RELNOTFOUND;
  }
  else if(status != OK)
  {
      return status;
  }
  // remove the tuple
  status = hfs->deleteRecord();
  if(status != OK)
    return status;

  delete hfs;
  return OK;
}


RelCatalog::~RelCatalog()
{
// nothing should be needed here
}


AttrCatalog::AttrCatalog(Status &status) :
	 HeapFile(ATTRCATNAME, status)
{
// nothing should be needed here
}


const Status AttrCatalog::getInfo(const string & relation,
				  const string & attrName,
				  AttrDesc &record)
{

  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;

  if (relation.empty() || attrName.empty()) return BADCATPARM;

  hfs = new HeapFileScan(ATTRCATNAME, status);
  if (status != OK)  return status;
  char* relChars = (char*)relation.c_str();
  status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ);
  if (status != OK)  return status;
  status = hfs->scanNext(rid);
  if (status != OK)  return status;
  status = hfs->getRecord(rec);
  if (status != OK)  return status;
  memcpy(&record,&rec,rec.length);

  return OK;
}


const Status AttrCatalog::addInfo(AttrDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;

  ifs = new InsertFileScan(RELCATNAME, status);
  if (status != OK) return status;
  Record rec;
  rec.data = (void*)&record;
  rec.length = sizeof(RelDesc);
  status = ifs->insertRecord(rec, rid);
  if (status != OK) return status;
  delete ifs;

  return OK;
}


const Status AttrCatalog::removeInfo(const string & relation,
			       const string & attrName)
{
  Status status;
  Record rec; // unused
  RID rid;
  AttrDesc record; // unused
  HeapFileScan*  hfs;

  if (relation.empty() || attrName.empty()) return BADCATPARM;

  // Two unused variables here - wrong?

  hfs = new HeapFileScan(ATTRCATNAME, status);
  if (status != OK) return status;
  char* relChars = (char*)relation.c_str();
  status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ);
  if (status != OK) return status;
  status = hfs->scanNext(rid);
  if (status != OK) return status;
  status = hfs->deleteRecord();
  if (status != OK) return status;

  return OK;
}

#include <vector>
const Status AttrCatalog::getRelInfo(const string & relation,
				     int &attrCnt,
				     AttrDesc *&attrs)
{
  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;

  if (relation.empty()) return BADCATPARM;

  hfs = new HeapFileScan(ATTRCATNAME, status);
  if (status != OK) return status;
  char* relChars = (char*)relation.c_str();
  status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ);
  if (status != OK) return status;

  // std::vector guarantees contiguous storage, can recast
  std::vector<AttrDesc>* attrs_vector = new std::vector<AttrDesc>();
  attrs = &((*attrs_vector)[0]);

  attrCnt = 0;
  while (hfs->scanNext(rid) != FILEEOF)
  {
    status = hfs->scanNext(rid);
	if (status != OK)  return status;
	status = hfs->getRecord(rec);
	if (status != OK)  return status;
	++attrCnt;
	AttrDesc desc;
	memcpy(&desc,&rec,rec.length);
	attrs_vector->push_back(desc);
  }

  return OK;
}


AttrCatalog::~AttrCatalog()
{
// nothing should be needed here
}

