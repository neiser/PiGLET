#include <iostream>
#include <iomanip>
#include "Epics.h"
#include "Structs.h"

using namespace std;

void Epics::exceptionCallback( exception_handler_args args ) {
    string pvname = ( args.chid ? ca_name( args.chid ) : "unknown" );
    cerr << "CA Exception: " << endl
         << "Status: " << ca_message(args.stat) << endl
         << "Channel: " + pvname << endl;
}

void Epics::deleteDataItem(DataItem* i) {
    // well, the void* data must be deleted as well
    // but we need to know the data type 
    // (see the callbacks below where these items are generated)
    switch (i->type) {
    case Epics::NewValue:
        delete static_cast<vec2_t*>(i->data);
        break;
    default:
        // all other types currently don't fill something
        // in the i->data
        break;
    }            
    // delete the current item
    delete i;    
}

void Epics::fillList(Epics::DataItem *head, vector<Epics::DataItem*>& list)
{
    // scan the linked list
    list.reserve(100);
    Epics::DataItem* d = head; // start from the head
    while(d->prev != NULL) {
        list.push_back(d);
        d = d->prev;
    }
}

void Epics::connectionCallback( connection_handler_args args ) { 
    DataItem* pNew = new DataItem;
    
    if ( args.op == CA_OP_CONN_UP ) {
        // channel has connected
        pNew->type = Connected;
    } 
    else { // args.op == CA_OP_CONN_DOWN
        pNew->type = Disconnected;
    }
      
    appendToList((PV*)ca_puser(args.chid), pNew);
}

void Epics::eventCallback( event_handler_args args ) {
    if ( args.status != ECA_NORMAL ) {
        cerr << "Error in EPICS event callback" << endl;
        return;
    } 
        
    // since the content of dbr is only valid within
    // this callback, we create some new memory space here
    // and hardcopy it
    
    vec2_t* data = new vec2_t;
    dbr_time_double* dbr = (dbr_time_double*)args.dbr; // Convert void* to correct data type
    epicsTime time(dbr->stamp);
    data->x= time - Epics::I().t0;
    data->y = dbr->value;
    
    // pack it together
    DataItem* pNew = new DataItem;    
    pNew->type = NewValue;
    pNew->data = data;   
    
    appendToList((PV*)ca_puser(args.chid), pNew);    
}

void Epics::appendToList(Epics::PV* pv, Epics::DataItem *pNew)
{
    while (1) { // concurrency loop
        DataItem* pOld = pv->head;  // copy the state of the world. We operate on the copy
        pNew->prev = pOld; // chain the new node to the current head of recycled items
        if (__sync_bool_compare_and_swap(&pv->head, pOld, pNew))  // switch head of recycled items to new node
            break; // success
    }
    //cout << "Something was appended. Contents:" << endl;
}

Epics::DataItem** Epics::addPV(const string &pvname)
{
    // check if we already know that PV
    if(pvs.count(pvname)>0) {
        return NULL;
    }
    
    // create the one and only data structure,
    // initialize the linked list (atomically modified by EPICS threads)
    PV* pv = new PV;
    DataItem* head = new DataItem; // create the first dummy item
    head->prev = NULL; // ensure it points to nothing before  
    pv->head = head; // save the pointer in the pv as a starting point
       
    // subscribe 
    subscribe(pvname, DBR_TIME_DOUBLE, pv);
    
    // save the pv
    pvs[pvname] = pv;
    cout << "PV " << pvname << " registered" << endl;
    
    return &pv->head; // tell where to find the head (for reading the list)
}

void Epics::subscribe(const string &pvname, chtype catype, PV* pv) {
    
    int ca_rtn = ca_create_channel( pvname.c_str(),      // PV name
                                    connectionCallback,  // name of connection callback function
                                    pv,               // 
                                    CA_PRIORITY_DEFAULT, // CA Priority
                                    &pv->mychid );    // Unique channel id
    
    SEVCHK(ca_rtn, "ca_create_channel failed");
    
    ca_rtn = ca_create_subscription( catype,          // CA data type
                                     1,                        // number of elements
                                     pv->mychid,            // unique channel id
                                     DBE_VALUE | DBE_ALARM,    // event mask (change of value and alarm)
                                     eventCallback,            // name of event callback function
                                     pv,
                                     &pv->myevid );         // unique event id needed to clear subscription
    
    SEVCHK(ca_rtn, "ca_create_subscription failed");
    
    // dont know if this is really meaningful here (also done in ctor)
    ca_poll();
}

void Epics::removePV(const string& pvname)
{
    // hopefully, the pvname exists :)
    PV* pv = pvs[pvname];
    
    // cancel the subscription
    ca_clear_subscription (pv->myevid);
    ca_clear_channel(pv->mychid);
    ca_poll();
    
    // properly delete the linked list
    typedef vector<DataItem*> list_t;
    list_t list;
    fillList(pv->head, list);
    for(list_t::iterator it = list.begin(); it != list.end(); ++it) {
        deleteDataItem(*it);
    }
    
    // the item itself
    delete pv;    
    
    // remove it from container
    pvs.erase(pvname);    
    cout << "PV " << pvname << " unregistered" << endl;
}

double Epics::GetCurrentTime()
{
    return epicsTime::getCurrent()-t0;
}


Epics::Epics () {
    ca_context_create( ca_enable_preemptive_callback );
    ca_add_exception_event( exceptionCallback, NULL );
    ca_poll();
    t0 = epicsTime::getCurrent();        
    cout << "EPICS ctor" << endl;
}

Epics::~Epics () {
    for (map<string, PV*>::iterator it = pvs.begin(); it != pvs.end(); ++it ) {
        removePV(it->first);
    }
    pvs.clear();
    ca_context_destroy();
    
    cout << "EPICS dtor" << endl;
}
