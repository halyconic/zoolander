#include <sys/types.h>
#include <functional>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "error.h"
#include "utility.h"
#include "catalog.h"

// define if debug output wanted


//
// Retrieves and prints information from the catalogs about the for the
// user. If no relation is given (relation is NULL), then it lists all
// the relations in the database, along with the width in bytes of the
// relation, the number of attributes in the relation, and the number of
// attributes that are indexed.  If a relation is given, then it lists
// all of the attributes of the relation, as well as its type, length,
// and offset, whether it's indexed or not, and its index number.
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::help(const string & relation)
{
  Status status;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;

  if (relation.empty()) return UT_Print(RELCATNAME);

  // make sure relation is actually a table
  if ((status = relCat->getInfo(relation, rd)) != OK)
    return status;

  // get all of the relation's attributes
  status = attrCat->getRelInfo(relation, attrCnt, attrs);
  if(status != OK)
    return status;

  cout << "Relation name: " << relation << endl;
  for(int i = 0; i < attrCnt; i++)
  {
    //print all the necessary info for each attribute
    AttrDesc attr = attrs[i];
    cout << "Attribute " << i << ": " << attr.attrName << endl;
    switch(attr.attrType)
    {
        case INTEGER:
            cout << "Type: int" << endl;
            break;
        case FLOAT:
            cout << "Type: float" << endl;
            break;
        case STRING:
            cout << "Type: string" << endl;
            break;
        default:
            break;
    }
    cout << "Length: " << attr.attrLen << endl;
    cout << "Offset: " << attr.attrOffset << endl;
  }
  free(attrs);
  return OK;
}
