#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include "page.h"
#include "buf.h"


#define ASSERT(c)  { if (!(c)) { \
		       cerr << "At line " << __LINE__ << ":" << endl << "  "; \
                       cerr << "This condition should hold: " #c << endl; \
                       exit(1); \
		     } \
                   }

//----------------------------------------
// Constructor of the class BufMgr
//----------------------------------------

BufMgr::BufMgr(const int bufs)
{
    numBufs = bufs;

    bufTable = new BufDesc[bufs];
    memset(bufTable, 0, bufs * sizeof(BufDesc));
    for (int i = 0; i < bufs; i++)
    {
        bufTable[i].frameNo = i;
        bufTable[i].valid = false;
    }

    bufPool = new Page[bufs];
    memset(bufPool, 0, bufs * sizeof(Page));

    int htsize = ((((int) (bufs * 1.2))*2)/2)+1;
    hashTable = new BufHashTbl (htsize);  // allocate the buffer hash table

    clockHand = bufs - 1;
}


BufMgr::~BufMgr() {

    // flush out all unwritten pages
    for (int i = 0; i < numBufs; i++)
    {
        BufDesc* tmpbuf = &bufTable[i];
        if (tmpbuf->valid == true && tmpbuf->dirty == true) {

#ifdef DEBUGBUF
            cout << "flushing page " << tmpbuf->pageNo
                 << " from frame " << i << endl;
#endif

            tmpbuf->file->writePage(tmpbuf->pageNo, &(bufPool[i]));
        }
    }

    delete [] bufTable;
    delete [] bufPool;
}


const Status BufMgr::allocBuf(int & frame)
{





	return OK;
}

/*
* Function: readPage
*
* Inputs: File* file - Pointer to File to have page allocated in
* const int PageNo - the page number of the file that we want to read
* Page*& page - Pointer to the buffer frame allocated for the page
*
* Returns: OK - if no errors occured
* UNIXERR - if a unix error occurs
* BUFFEREXCEEDED - if all buffer frames are pinned
* HASHTBLERROR - if a hash table error occured
*
* Description: This method reads a page from disk into the buffer pool.
* If the page is already in the buffer pool, we just update appropriate values.
*/
const Status BufMgr::readPage(File* file, const int PageNo, Page*& page)
{

	//check whether page is already in buffer pool
    int frameNo = -1;
    Status s = hashTable->lookup(file, PageNo, frameNo);
    if(s == HASHNOTFOUND)
    {
        //CASE 1: Page not in buffer pool
        // allocate a buffer frame, read page into buffer frame,
        // insert page into hashtable, and set it up

        s = allocBuf(frameNo);
        if(s != OK)
        {
            return s;
        }
        s = file->readPage(PageNo, page);
        if(s != OK)
        {
            return s;
        }

        s = hashTable->insert(file, PageNo, frameNo);
        if(s != OK)
        {
            return s;
        }

        //set up proper frame
        BufDesc* tmpbuf = &(bufTable[frameNo]);
        tmpbuf->Set(file, PageNo);

        //return frame pointer
        page = &(bufPool[frameNo]);
    }
    else
    {
        //CASE 2: Page is in buffer pool
        //set refbit, increment pinCnt
        BufDesc* tmpbuf = &(bufTable[frameNo]);
        tmpbuf->refbit = true;
        tmpbuf->pinCnt++;

        //return frame pointer
        page = &(bufPool[frameNo]);
    }
	return OK;
}


const Status BufMgr::unPinPage(File* file, const int PageNo,
			       const bool dirty)
{
    int frameNo = -1;
    Status s = hashTable->lookup(file, PageNo, frameNo);
    if (s == HASHNOTFOUND)
    {
    	return s;
    }

	BufDesc* tmpbuf = &(bufTable[frameNo]);
	if (tmpbuf->pinCnt <= 0)
	{
		return PAGENOTPINNED;
	}
	tmpbuf->pinCnt--;

	if (dirty)
	{
	    tmpbuf->dirty = true;
	}

	return OK;
}

/*
* Function: allocPage
*
* Inputs: File* file - Pointer to File to have page allocated in
* int& pageNo - Address to int for return of the page number
* Page*& page - Pointer to the buffer frame allocated for the page
*
* Returns: OK - if no errors occured
* UNIXERR - if a unix error occurs
* BUFFEREXCEEDED - if all buffer frames are pinned
* HASHTBLERROR - if a hash table error occured
*
* Description: Allocates an empty page in a file using File's allocatePage().
* Then allocBuf() is called to obtain a buffer pool frame. An entry is
* inserted into the hash table and Set() invoked to properly set it up.
* Then pageNo and page returns are set. Then the function returns as
* indicated above.
*
*/
const Status BufMgr::allocPage(File* file, int& pageNo, Page*& page)
{
    Status status = OK;

    // allocate new page in file, pageNo is set for return at this point
    status = file->allocatePage(pageNo);

    // check for error, if error return error
    if (status != OK)
    {
        return status;
    }

    int frame;
    // allocate buffer frame
    status = allocBuf(frame);

    // check for error, if error return error
    if (status != OK)
    {
        return status;
    }

    // insert allocated page into hash table at allocated frame
    status = hashTable->insert(file, pageNo, frame);

    // check for error, if error return error
    if (status != OK)
    {
        return status;
    }

    // set page for returning;
    page = &(bufPool[frame]);

    // set file correctly
    //bufTable->Set(file, *pageNo);
    BufDesc* tmpbuf = &(bufTable[frame]);
    tmpbuf->Set(file, *pageNo);

    return status;
}

const Status BufMgr::disposePage(File* file, const int pageNo)
{
    // see if it is in the buffer pool
    Status status = OK;
    int frameNo = 0;
    status = hashTable->lookup(file, pageNo, frameNo);
    if (status == OK)
    {
        // clear the page
        bufTable[frameNo].Clear();
    }
    status = hashTable->remove(file, pageNo);

    // deallocate it in the file
    return file->disposePage(pageNo);
}

const Status BufMgr::flushFile(const File* file)
{
  Status status;

  for (int i = 0; i < numBufs; i++) {
    BufDesc* tmpbuf = &(bufTable[i]);
    if (tmpbuf->valid == true && tmpbuf->file == file) {

      if (tmpbuf->pinCnt > 0)
	  return PAGEPINNED;

      if (tmpbuf->dirty == true) {
#ifdef DEBUGBUF
	cout << "flushing page " << tmpbuf->pageNo
             << " from frame " << i << endl;
#endif
	if ((status = tmpbuf->file->writePage(tmpbuf->pageNo,
					      &(bufPool[i]))) != OK)
	  return status;

	tmpbuf->dirty = false;
      }

      hashTable->remove(file,tmpbuf->pageNo);

      tmpbuf->file = NULL;
      tmpbuf->pageNo = -1;
      tmpbuf->valid = false;
    }

    else if (tmpbuf->valid == false && tmpbuf->file == file)
      return BADBUFFER;
  }

  return OK;
}


void BufMgr::printSelf(void)
{
    BufDesc* tmpbuf;

    cout << endl << "Print buffer...\n";
    for (int i=0; i<numBufs; i++) {
        tmpbuf = &(bufTable[i]);
        cout << i << "\t" << (char*)(&bufPool[i])
             << "\tpinCnt: " << tmpbuf->pinCnt;

        if (tmpbuf->valid == true)
            cout << "\tvalid\n";
        cout << endl;
    };
}


