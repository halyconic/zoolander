#include "catalog.h"


RelCatalog::RelCatalog(Status &status) :
	 HeapFile(RELCATNAME, status)
{
// nothing should be needed here
}

/*
 * Gets relation descriptor for a given relation
 * by scanning through relcat.
 *
 * Returns: OK if info retrieved
 *          Error status otherwise
 */
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
  memcpy(&record,rec.data,rec.length);
  delete hfs;
  return OK;
}

/*
 * Adds info to the catalog.  In this case, a new tuple (record)
 * is put into relcat.
 *
 * Returns: OK if it worked
 *          Error status if not
 */
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

/*
 * Removes a tuple (identified by relation) from relcat.
 *
 * Returns: OK if it worked
 * 			Error status otherwise
 */
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

/*
 * Retrieves the tuple identified by relation and attrName
 * from attrCat
 *
 * Returns: OK if works
 *          Error Status otherwise
 */
const Status AttrCatalog::getInfo(const string & relation,
				  const string & attrName,
				  AttrDesc &record)
{
  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;
  AttrDesc tempRecord;

  if (relation.empty() || attrName.empty()) return BADCATPARM;
  // creates a scan to find all tuples with relName == relation
  hfs = new HeapFileScan(ATTRCATNAME, status);
  if(status != OK)
    return status;
  char* relChars = (char*)relation.c_str();
  status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ);
  if(status != OK)
    return status;

  //scans through all of the found tuples,
  //and checks to see if the given attrName matches
  while((status = hfs->scanNext(rid)) != FILEEOF)
  {
    if (status != OK)
        return status;
    status = hfs->getRecord(rec);
    if (status != OK)
        return status;
    memcpy(&tempRecord, rec.data, rec.length);
    char* attrChars = (char*)attrName.c_str();
    if (strcmp(attrChars, record.attrName) == 0)
    {
        record = tempRecord;
        delete hfs;
        return OK;
    }
  }
  return FILEEOF;
}

/*
 * Adds a tuple to attrCat.
 *
 * Returns: OK if works
 *          Error status otherwise
 */
const Status AttrCatalog::addInfo(AttrDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;
  //makes an insert scan to insert the tuple
  ifs = new InsertFileScan(ATTRCATNAME, status);
  if (status != OK) return status;

  //constructs a Record format from the AttrDesc given
  Record rec;
  rec.data = (void*)&record;
  rec.length = sizeof(AttrDesc);
  //inserts the Record into the heap file
  status = ifs->insertRecord(rec, rid);
  if (status != OK) return status;
  delete ifs;
  return OK;
}

/*
 * Removes a tuple from attrCat
 *
 * Returns: OK if worked
 *          Error status otherwise
 */
const Status AttrCatalog::removeInfo(const string & relation,
			       const string & attrName)
{
  Status status;
  Record rec;
  RID rid;
  AttrDesc record;
  HeapFileScan*  hfs;

  if (relation.empty() || attrName.empty()) return BADCATPARM;
  // create new scan to search for attributes belonging to the Relation relation
  hfs = new HeapFileScan(ATTRCATNAME, status);
  if(status != OK)
    return status;
  char* relChars = (char*)relation.c_str();
  char* attrChars = (char*)attrName.c_str();

  status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ);
  if(status != OK)
    return status;

  // for each attribute belonging to relation,
  // check to see if attrName matches up
  // Removes the record from the heap file if so
  while((status = hfs->scanNext(rid)) != FILEEOF)
  {
    if(status != OK)
        return status;
    status = hfs->getRecord(rec);
    if(status != OK)
        return status;
    memcpy(&record, rec.data, rec.length);
    if(strcmp(attrChars, record.attrName) == 0)
    {
        status = hfs->deleteRecord();
        if(status != OK)
            return status;
        delete hfs;
        return OK;
    }
  }
  return FILEEOF;
}

/*
 * Get all attributes of a relation from attrCat (as opposed
 * to only 1 attribute in getInfo())
 *
 * Returns: OK if worked
 * 			Error status otherwise
 */
const Status AttrCatalog::getRelInfo(const string & relation,
				     int &attrCnt,
				     AttrDesc *&attrs)
{
  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;

  if (relation.empty()) return BADCATPARM;

  // start scan to find tuples with relName == relation
  hfs = new HeapFileScan(ATTRCATNAME, status);
  if (status != OK) return status;
  char* relChars = (char*)relation.c_str();
  status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ);
  if (status != OK) return status;
  
  // count how many attributes there will be, so that we can
  // allocate the correct amount of space
  attrCnt = 0;
  while (hfs->scanNext(rid) != FILEEOF)
  {
	if(status != OK)
		return status;
	status = hfs->getRecord(rec);
	if (status != OK)  return status;
	++attrCnt;
  }
  if((status = hfs->endScan()) != OK)
  {
	  return status;
  }

  // start scan again, now that we know how many there will be
  if((status = hfs->startScan(0, sizeof(relChars), STRING, relChars, EQ)))
  {
	  return status;
  }
  attrs = new AttrDesc[attrCnt];	// our accurate allocation
  int i = 0;
  // for each attribute belonging to relation,
  // add it's AttrDesc to the output array
  while(hfs->scanNext(rid) != FILEEOF)
  {
	  status = hfs->getRecord(rec);
	  if(status != OK)
		  return status;
	  AttrDesc desc;
	  memcpy(&desc,rec.data,rec.length);
	  attrs[i] = desc;
	  i++;
  }
  delete hfs;
  return OK;
}


AttrCatalog::~AttrCatalog()
{
// nothing should be needed here
}

