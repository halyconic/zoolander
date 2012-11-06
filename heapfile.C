#include "heapfile.h"
#include "error.h"

/*
* Function: createHeapFile
*
* Dependancies: db.createFile(), db.createFile(), bufMgr.allocPage() 
*
* Input: const string fileName - the name of the file to be created
*
* Returns:  OK - if no error
*           FILEEXISTS - if a file exists in the working directory 
*                           with the same name as fileName
*           Any other errors from dependant subfunctions
*
* Description: This function creates a heapfile with the given fileName(if it 
*               already exists it throws an error). Then it initializes the file
*               by creating the header page and an empty, but initialized data 
*               page.
*/
const Status createHeapFile(const string fileName)
{
    File* 		file;
    Status 		status;
    FileHdrPage*	hdrPage;
    int			hdrPageNo;
    int			newPageNo;
    Page*		newPage;

    // try to open the file. This should return an error
    status = db.openFile(fileName, file);
    if (status != OK){

        // create the file if error return the error
        status = db.createFile(fileName);
        if(status != OK){
            return status;
        }

        // open the newly created file if error return error
        status = db.openFile(fileName, file);
        if(status != OK){
            return status;
        }

        // allocate page in file for FileHdrPage and return errors
        status = bufMgr->allocPage(file, hdrPageNo,newPage);
        if(status != OK){
            return status;
        }

        // cast newly allocated page and initialize it as a FileHdrPage
        // note: firstPage and lastPage to be allocated later
        hdrPage = (FileHdrPage*) newPage;
        strcpy(hdrPage->fileName, fileName.c_str());
        hdrPage->pageCnt = 1; //assuming header as 0 first data page as 1
        hdrPage->recCnt = 0;

        // allocate page in file as first data page and initialize as page
        status = bufMgr->allocPage(file, newPageNo,newPage);
        if(status != OK){
            return status;
        }
        newPage->init(newPageNo);

        // store first page as first and last page of file
        hdrPage->firstPage = newPageNo;
        hdrPage->lastPage = newPageNo;

        // unpin both pages and set as dirty
        bufMgr->unPinPage(file, hdrPageNo, true);
        bufMgr->unPinPage(file, newPageNo, true);

<<<<<<< HEAD
        return OK;
=======
        status = db.closeFile(file);
        if(status != OK)
        {
        	return status;
        }

        return OK;
>>>>>>> 95b75358208ee114b01699100e253c818feacfef
    }
    return (FILEEXISTS);
}

/*
* Function: destroyHeapFile
*
* Input: const string fileName - the name of the file to be destroyed
*
* Returns:  OK - if no errors
*           FILEOPEN - if file is open
*           BADFILE - if fileName doesn't exist
* 
* Description: Deletes the heap file from disk. The method requires that the
*               file be closed before calling.
*/
const Status destroyHeapFile(const string fileName)
{
	return (db.destroyFile (fileName));
}

/*
* Function: HeapFile
*
* Input: const string& fileName - address of the name of the file to be opened *                                    as a heapFile
*        Status& returnStatus - address for return status
*
* Returns:  no returns but returnStatus is set to the following prior to 
*          completing the method:
*            OK - if no error
*            ANY IO or Unix errors associated with opening files
*
* Description: Opens the given file as a heap file. If an error occurs it gets
*                set in the returnStatus variable.
*/
HeapFile::HeapFile(const string & fileName, Status& returnStatus)
{
	Status 	status;
    Page*	pagePtr;

    cout << "opening file " << fileName << endl;

    // open the file and read in the header page and the first data page
    if ((status = db.openFile(fileName, filePtr)) == OK)
    {
    	// initialize the private data members headerPage, headerPageNo, and hdrDirtyFlag
    	status = filePtr->getFirstPage(headerPageNo);
        if (status != OK)
        {
            cerr << "error in getting header page number";
        }
		status = bufMgr->readPage(filePtr, headerPageNo, pagePtr);
        if (status != OK)
        {
            cerr << "error in reading header page";
        }
		headerPage = (FileHdrPage*)pagePtr;
		hdrDirtyFlag = false;

		// read and pin the first page of the file into the buffer pool
		curPageNo = headerPage->firstPage;
		status = bufMgr->readPage(filePtr, curPageNo, curPage);

		// initialize curPage, curPageNo, curDirtyFlag, and curRec appropriately
		curDirtyFlag = false;
		curRec = NULLRID;
		returnStatus = OK;
    }
    else
    {
    	cerr << "open of heap file failed\n";
		returnStatus = status;
		return;
    }
}

/*
* Function ~HeapFile()
*
* Description: Closes the heap file. Unpinning any pinned pages and writing the
*               dirty pages page to disk.
*/
HeapFile::~HeapFile()
{
    Status status;
    cout << "invoking heapfile destructor on file " << headerPage->fileName << endl;

    // see if there is a pinned data page. If so, unpin it
    if (curPage != NULL)
    {
    	status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
		curPage = NULL;
		curPageNo = 0;
		curDirtyFlag = false;
		if (status != OK) cerr << "error in unpin of date page\n";
    }

	 // unpin the header page
    status = bufMgr->unPinPage(filePtr, headerPageNo, hdrDirtyFlag);
    if (status != OK) cerr << "error in unpin of header page\n";

	// status = bufMgr->flushFile(filePtr);  // make sure all pages of the file are flushed to disk
	// if (status != OK) cerr << "error in flushFile call\n";
	// before close the file
	status = db.closeFile(filePtr);
    if (status != OK)
    {
		cerr << "error in closefile call\n";
		Error e;
		e.print (status);
    }
}

/*
* Function:getRecCnt();
*
* Return: int - the number of records in the file
*
*/
const int HeapFile::getRecCnt() const
{
  return headerPage->recCnt;
}

/*
* Function: getRecord()
*
* Inputs: const RID & rid - RID of record to be return.
*         Record & rec - place holder to catch the returned record
*
* Output: OK - if no error and record in &rec variable
*         Any errors encountered attempting to get to the record.
*
* Description: Retrieve an arbitrary record from a file. If record is not on 
*                the currently pinned page, the current page is unpinned and 
*                the required page is read into the buffer pool and pinned.
*                Returns a pointer to the record via the rec parameter
*/
const Status HeapFile::getRecord(const RID & rid, Record & rec)
{
    Status status;

    // cout<< "getRecord. record (" << rid.pageNo << "." << rid.slotNo << ")" << endl;

    // if the desired record is on the currently pinned page
    if (rid.pageNo == curPageNo)
    {
    	curPage->getRecord(rid, rec);
    }
    else
    {
    	// unpin the currently pinned page (assumes a page is pinned)
    	status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
        if(status != OK)
        {
            cerr << "error in unpinning page";
            return status;
        }

    	// use the pageNo field of the RID to read the page into the buffer pool
    	status = bufMgr->readPage(filePtr, rid.pageNo, curPage);
        if(status != OK)
        {
            cerr << "error in reading page";
            return status;
        }
        curPageNo = rid.pageNo;
        curPage->getRecord(rid, rec);
    }
    return OK;
}


HeapFileScan::HeapFileScan(const string & name,
			   Status & status) : HeapFile(name, status)
{
    filter = NULL;
}


const Status HeapFileScan::startScan(const int offset_,
				     const int length_,
				     const Datatype type_,
				     const char* filter_,
				     const Operator op_)
{
    if (!filter_) {                        // no filtering requested
        filter = NULL;
        return OK;
    }

    if ((offset_ < 0 || length_ < 1) ||
        (type_ != STRING && type_ != INTEGER && type_ != FLOAT) ||
        (type_ == INTEGER && length_ != sizeof(int)
         || type_ == FLOAT && length_ != sizeof(float)) ||
        (op_ != LT && op_ != LTE && op_ != EQ && op_ != GTE && op_ != GT && op_ != NE))
    {
        return BADSCANPARM;
    }

    offset = offset_;
    length = length_;
    type = type_;
    filter = filter_;
    op = op_;

    return OK;
}

/*
* Function: endScan()
*
* Output: OK - if no error
*         Any errors encountered during scanning
*
* Description: This method terminates a scan over a file but does not delete the
*               scan object.  This will allow the scan object to be reused for
*               another scan. 
*/
const Status HeapFileScan::endScan()
{
    Status status;
    // generally must unpin last page of the scan
    if (curPage != NULL)
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
        curPage = NULL;
        curPageNo = 0;
		curDirtyFlag = false;
        return status;
    }
    return OK;
}

HeapFileScan::~HeapFileScan()
{
    endScan();
}

/*
* Function: markScan()
*
* Output: OK - if no error
*         Any errors encountered during the save of the scan
*
* Description: Saves the current position of the scan by preserving the values
*               of curPageNo and curRec in the private data members markedPageNo
*               and markedRec, respectively. 
*/
const Status HeapFileScan::markScan()
{
    // make a snapshot of the state of the scan
    markedPageNo = curPageNo;
    markedRec = curRec;
    return OK;
}

/*
* Function: resetScan()
*
* Output: OK - if no error
*         Any errors encountered during the reset of the scan
*
* Description: Resets the position of the scan to the position when the scan was
*               last marked by restoring the values of curPageNo and curRec from
*               markedPageNo and markedRec, respectively. Unless the page number
*               of the currently pinned page is the same as the marked page
*               number, unpin the currently pinned page, then read markedPageNo
*               from disk and set curPageNo, curPage, curRec, and curDirtyFlag
*               appropriately.
*/
const Status HeapFileScan::resetScan()
{
    Status status;
    if (markedPageNo != curPageNo)
    {
		if (curPage != NULL)
		{
			status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
			if (status != OK) return status;
		}
		// restore curPageNo and curRec values
		curPageNo = markedPageNo;
		curRec = markedRec;
		// then read the page
		status = bufMgr->readPage(filePtr, curPageNo, curPage);
		if (status != OK) return status;
		curDirtyFlag = false; // it will be clean
    }
    else curRec = markedRec;
    return OK;
}

/*
* Function: scanNext()
*
* Input: RID& outRid - return for the RID of next matching record
*
* Output: OK - if no error
*         Any errors encountered during the scan
*
* Description:Returns (via the outRid parameter) the RID of the next record that
*               satisfies the scan predicate.
*/
const Status HeapFileScan::scanNext(RID& outRid)
{
    Status 	status = OK;
    RID		nextRid;
    RID		tmpRid;
    int 	nextPageNo;
    Record      rec;

    //weird error case
    if (curPage == NULL)
    {
        return TROGDOR;
    }
    if(headerPage->recCnt == 0)
    {
    	return FILEEOF;
    }

    // jump to the last record we returned (curRec)
    // if no last returned record, we need to get first record from first page
    if(curRec.pageNo == -1 && curRec.slotNo == -1)
    {
        // pin whatever is the current thing and read in the first page
        status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
        if (status != OK && status != PAGENOTPINNED)
        {
            cerr << "error in unpinning curPage 4\n";
            return status;
        }
        status = bufMgr->readPage(filePtr, headerPage->firstPage, curPage);
        if (status != OK)
        {
            cerr << "error in reading in page";
            return status;
        }
        //update values
        curPageNo = headerPage->firstPage;
        curDirtyFlag = false;
        // get first record from first page
        status = curPage->firstRecord(tmpRid);
        if(status != OK)
        {
            cerr << "e1";
            return status;
        }
    }
    else
    {
        // jump to next record
        status = curPage->nextRecord(curRec, tmpRid);
        if(status != OK && status != ENDOFPAGE)
        {
            return status;
        }
        // uh-oh.  next record is on next page
        if(status == ENDOFPAGE)
        {
            // find next page
            status = curPage->getNextPage(nextPageNo);
    		if(status != OK)
    		{
    			cerr << "error in getting next page";
    			return status;
    		}
    		//uh-oh.  there is no next page
    		if(nextPageNo == -1)
    		{
                return FILEEOF;
    		}

            // unpin old page
            status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
            if(status != OK)
            {
                cerr << "error in unpinning old page";
                return status;
            }

            // read in next page check for errors
            status = bufMgr->readPage(filePtr, nextPageNo, curPage);
            if (status != OK)
            {
                cerr << "error in reading in next page";
                return status;
            }

            //update values	
            curPage->firstRecord(tmpRid);
            curPageNo = nextPageNo;
            curDirtyFlag = false;

        }
    }

    // now move forward in the scan until something is found or end is reached
    bool foundRetRec = false;
    while(!foundRetRec)
    {
        //turn the rid into a record
        status = curPage->getRecord(tmpRid, rec);
        if(status != OK)
        {
            return status;
        }

        // check if it satisfies filter
        if(matchRec(rec) == true)
        {
            foundRetRec = true;
            curRec = tmpRid;
            outRid = curRec;
            return OK;
        }

        // if not, get next record
        status = curPage->nextRecord(tmpRid, tmpRid);
        if(status != OK && status != ENDOFPAGE)
        {
            return status;
        }
        if(status == ENDOFPAGE)
        {
            // find next page
            status = curPage->getNextPage(nextPageNo);
    		if(status != OK)
    		{
    			cerr << "error in getting next page";
    			return status;
    		}
    		//uh-oh.  there is no next page
    		if(nextPageNo == -1)
    		{
                return FILEEOF;
    		}

            // unpin old page
            status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
            if(status != OK)
            {
                cerr << "error in unpinning old page";
                return status;
            }

            // read in next page check for errors
            status = bufMgr->readPage(filePtr, nextPageNo, curPage);
            if (status != OK)
            {
                cerr << "error in reading in next page";
                return status;
            }

            //update values	
            curPage->firstRecord(tmpRid);
            curPageNo = nextPageNo;
            curDirtyFlag = false;

        }
    }

    /*

    // Check all pages
    while (nextPageNo != -1)
    {
    	while (status != ENDOFPAGE)
    	{

    		// Get record
    		curPage->getRecord(tmpRid, rec);

    		if (matchRec(rec) == true)
    		{
    			curRec = tmpRid;
    			outRid = curRec;
    			return OK;
    		}

    		// Get next record
    		status = curPage->nextRecord(tmpRid, tmpRid);
    	}

    	// Get next page
    	status = curPage->getNextPage(nextPageNo);
        if(status != OK)
        {
            cerr << "error in getting next page";
            return status;
        }
        bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
		if(nextPageNo == -1)
		{
			return TROGDOR;
		}
    	bufMgr->readPage(filePtr, nextPageNo, curPage);
    }*/
    return OK;
}

/*
* Function: getRecord()
*
* Input: Record& rec - return for the record of the RID stored in curRec
*
* Output: OK - if no error
*         BADPAGENO - if page containing curRec is not pinned
*         Any errors encountered during the 
*
* Description:Returns (via the outRid parameter) the RID of the next record that
*               satisfies the scan predicate. Page is left pinned and the scan
*               logic is required to unpin the page.
*/
const Status HeapFileScan::getRecord(Record & rec)
{
    return curPage->getRecord(curRec, rec);
}

/*
* Function: deleteRecord()
*
* Output: OK - if no error
*         BADPAGENO - if page containing curRec is not pinned
*
* Description: Deletes the record with RID whose rid is stored in curRec from
*               the file. The record must be a record on the "current" page of
*               the scan. Returns BADPAGENO if the page number of the record
*               (i.e. curRec.pageNo) does not match curPageNo, otherwise OK. 
*/
const Status HeapFileScan::deleteRecord()
{
    Status status;

    // delete the "current" record from the page
    status = curPage->deleteRecord(curRec);
    curDirtyFlag = true;

    // reduce count of number of records in the file
    headerPage->recCnt--;
    hdrDirtyFlag = true;
    return status;
}


/*
* Function: markDirty()
*
* Output: OK - if no error
*         Any error encountered in marking page dirty
*
* Description: Marks page as dirty. 
*/
const Status HeapFileScan::markDirty()
{
    curDirtyFlag = true;
    return OK;
}

const bool HeapFileScan::matchRec(const Record & rec) const
{
    // no filtering requested
    if (!filter) return true;

    // see if offset + length is beyond end of record
    // maybe this should be an error???
    if ((offset + length -1 ) >= rec.length)
	return false;

    float diff = 0;                       // < 0 if attr < fltr
    switch(type) {

    case INTEGER:
        int iattr, ifltr;                 // word-alignment problem possible
        memcpy(&iattr,
               (char *)rec.data + offset,
               length);
        memcpy(&ifltr,
               filter,
               length);
        diff = iattr - ifltr;
        break;

    case FLOAT:
        float fattr, ffltr;               // word-alignment problem possible
        memcpy(&fattr,
               (char *)rec.data + offset,
               length);
        memcpy(&ffltr,
               filter,
               length);
        diff = fattr - ffltr;
        break;

    case STRING:
        diff = strncmp((char *)rec.data + offset,
                       filter,
                       length);
        break;
    }

    switch(op) {
    case LT:  if (diff < 0.0) return true; break;
    case LTE: if (diff <= 0.0) return true; break;
    case EQ:  if (diff == 0.0) return true; break;
    case GTE: if (diff >= 0.0) return true; break;
    case GT:  if (diff > 0.0) return true; break;
    case NE:  if (diff != 0.0) return true; break;
    }

    return false;
}

InsertFileScan::InsertFileScan(const string & name,
                               Status & status) : HeapFile(name, status)
{
  //Do nothing. Heapfile constructor will bread the header page and the first
  // data page of the file into the buffer pool
}

InsertFileScan::~InsertFileScan()
{
    Status status;
    // unpin last page of the scan
    if (curPage != NULL)
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, true);
        curPage = NULL;
        curPageNo = 0;
        if (status != OK && status != PAGENOTPINNED) cerr << "error in unpin of data page\n";
    }
}

/*
* Function: insertRecord()
*
* Inputs: const Record & rec - record to insert
*         RID& outRid - RID to insert it at
*
* Output: OK - if no error
*         Any error encountered in marking page dirty
*
* Description: Inserts the record described by rec into the file returning the 
*               RID of the inserted record in outRid. 
*/
const Status InsertFileScan::insertRecord(const Record & rec, RID& outRid)
{
    Page*	newPage;
    int		newPageNo;
    Status	status, unpinstatus;
    RID		rid;
    Page* iterPage;

    // check for very large records
    if ((unsigned int) rec.length > PAGESIZE-DPFIXED)
    {
        // will never fit on a page, so don't even bother looking
        return INVALIDRECLEN;
    }

    //get first page into buffer
    unpinstatus = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
    if (unpinstatus != OK && unpinstatus != PAGENOTPINNED)
    {
        cerr << "error in unpinning curPage 4\n";
        return unpinstatus;
    }
    status = bufMgr->readPage(filePtr, headerPage->firstPage, iterPage);
    if (status != OK)
    {
        cerr << "error in reading in page";
        return status;
    }

    curPage = iterPage;
    curPageNo = headerPage->firstPage;
    curDirtyFlag = false;

    // go through linked list, looking for enough space to insert record
    bool foundOpenPage = false;
    while(!foundOpenPage)
    {
        //if page has room, enter the record into that page
        if((unsigned int) rec.length <= iterPage->getFreeSpace())
        {
            iterPage->insertRecord(rec,rid);
            outRid = rid;
            foundOpenPage = true;

            headerPage->recCnt++;
            hdrDirtyFlag = true;

            // implict: curPage = iterPage;
            // implict: curPageNo = iterPage->curPage;
            curDirtyFlag = true;

            if((unpinstatus = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag)) != OK)
            {
                cerr << "error in unpinning curPage 1\n";
                return status;
            }
        }
        // if not, move to next page and try again
        else
        {
            //get next page into buffer
            int nextPageNo;
            iterPage->getNextPage(nextPageNo);
            if (nextPageNo > 0)
            {
                //unpin current page
            	unpinstatus = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
                if (unpinstatus != OK && unpinstatus != PAGENOTPINNED)
                {
                    cerr << "error in unpinning curPage 2\n";
                    return status;
                }

                status = bufMgr->readPage(filePtr, nextPageNo, iterPage);
                if(status != OK)
                {
                    cerr << "error in reading page into buffer";
                    return status;
                }

                curPage = iterPage;
                curPageNo = nextPageNo;
                curDirtyFlag = false;
            }
            else
            {
                // end of linked list; need to make new page to insert record,
                // since no page had enough room
                foundOpenPage = true;
                status = bufMgr->allocPage(filePtr, newPageNo, newPage);
                if(status != OK)
                {
                    cerr << "error in allocating new page";
                    return status;
                }
                bufMgr->readPage(filePtr, newPageNo, newPage);
                newPage->init(newPageNo);
                newPage->insertRecord(rec, rid);
                outRid = rid;

                // Set link list correctly
                curPage->setNextPage(newPageNo);
                curDirtyFlag = true;

                //unpin prev page
            	unpinstatus = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
                if (unpinstatus != OK && unpinstatus != PAGENOTPINNED)
                {
                    cerr << "error in unpinning prev page\n";
                    return status;
                }

                curPage = newPage;
                curPageNo = newPageNo;
                curDirtyFlag = true;

                headerPage->lastPage = curPageNo;
                headerPage->pageCnt++;
                headerPage->recCnt++;
                hdrDirtyFlag = true;

                unpinstatus = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag);
                if (unpinstatus != OK && unpinstatus != PAGENOTPINNED)
                {
                    cerr << "error in unpinning curPage 3\n";
                    return status;
                }
            }
        }
    }
    return OK;
}


