#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "Epics.h"
#include "Structs.h"

using namespace std;

Epics::Epics () {
    // modify the PATH variable such that caRepeater can be
    // found by EPICS. This avoids also a "defunct" thread
    stringstream mypath;
    mypath << getenv("PATH") << ":" << EPICS_BIN_PATH;
    if(setenv("PATH", mypath.str().c_str(), 1) != 0) {
        perror("Cannot set PATH variable for EPICS");
    }
  
    ca_context_create( ca_enable_preemptive_callback );
    ca_add_exception_event( exceptionCallback, NULL );
    ca_poll();
    t0 = epicsTime::getCurrent();      
    _watch.Start();
    //cout << "EPICS ctor" << endl;
}

Epics::~Epics () {
    for (map<string, PV*>::iterator it = pvs.begin(); it != pvs.end(); ++it ) {
        removePV(it->first);
    }
    pvs.clear();
    ca_context_destroy();
    
    //cout << "EPICS dtor" << endl;
}

void Epics::exceptionCallback( exception_handler_args args ) {
    string pvname = ( args.chid ? ca_name( args.chid ) : "unknown" );
    cerr << "CA Exception: " << endl
         << "Status: " << ca_message(args.stat) << endl
         << "Channel: " + pvname << endl;
}

void Epics::deleteDataItem(DataItem* i) {
    // well, the void* data should be freed
    // (see the callbacks below where these items are generated)
    switch (i->type) {
    case Epics::NewValue:
        free(i->data);
        break;
    case Epics::NewProperties:
        free(i->data);
        break;
    default:
        // all other types currently 
        // don't fill something
        // in the i->data
        break;
    }            
    // delete the current item, 
    // this hopefully calls destructors of nested items
    delete i;    
}

void Epics::fillList(DataItem *head, vector<DataItem*>& list)
{
    // scan the linked list
    list.reserve(100);
    DataItem* d = head; // start from the head
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
    string pvname = ( args.chid ? ca_name( args.chid ) : "unknown" );
    //cout << "Connection " << pvname << endl;
    appendToList((PV*)ca_puser(args.chid), pNew);
}

void Epics::eventCallback( event_handler_args args ) {
    if ( args.status != ECA_NORMAL ) {
        cerr << "Error in EPICS event callback, ignoring event." << endl;
        return;
    } 
    
    // since the content of dbr is only valid within
    // this callback, we create some new memory space here
    // and hardcopy it
    
    PV* pv = (PV*)args.usr;
    DataItem* pNew = new DataItem;        
    
    if(args.type == DBR_TIME_DOUBLE) {
        vec2_t* data = new vec2_t;
        dbr_time_double* dbr = (dbr_time_double*)args.dbr; // Convert void* to correct data type
        
        // we use the timestamp to see if this
        // value is from a time before the start
        // of the program. If it isn't, we assume
        // that the change of the value happened "right now"
        epicsTime time(dbr->stamp);
        double t = time - Epics::I().t0;
        data->x= t<0 ? t : Epics::I().GetCurrentTime();
        // y-value is easy
        data->y = dbr->value;
        
        // pack it together
        pNew->type = NewValue;
        pNew->data = data;   
    }
    else {
        
        pNew->type = NewProperties;
        
        // determine the "attr" name of the property.
        pNew->attr = "_UNKNOWN";
        for(size_t i=0; i<pv->channels.size(); i++) {
            if(args.chid == pv->channels[i]._chid) {
                pNew->attr = pv->channels[i]._attr;
                break;
            }
        }
        // copy the content, since args is only valid within this callback
        size_t nBytes = dbr_size_n(args.type, args.count);
        //cout << "ATTR: " << pNew->attr << " nBytes: " << nBytes << endl;
        pNew->data = malloc(nBytes);
        memcpy(pNew->data, args.dbr, nBytes);       
    }
    
    appendToList(pv, pNew);    
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
    if(pv->auto_call)
        processNewDataForPV(pv);
        
}

void Epics::addPV(const string &pvname, EpicsCallback cb, bool autoCall)
{    
    PV* pv = initPV();
    pv->cb = cb;
    pv->auto_call = autoCall;
    
    // subscribe to value and control
    subscribe(pvname, pv);
    
    // save the pv
    pvs[pvname] = pv;
    cout << "PV " << pvname << " registered" << endl;
    
    
}

Epics::PV* Epics::initPV() {
    // create the one and only data structure,
    // initialize the linked list (atomically modified by EPICS threads)
    PV* pv = new PV;
    DataItem* head = new DataItem; // create the first dummy item
    head->prev = NULL; // ensure it points to nothing before  
    pv->head = head; // save the pointer in the pv as a starting point
    pv->head_last = NULL;
    return pv;    
} 

void Epics::subscribe_channel(const string &pvname, PV* pv, 
                              const string &attr, chtype type ) {
    PV_channel_t channel;
    channel._attr = attr; // remember the attribute, see Epics::eventCallback
    int ca_rtn = ca_create_channel( (pvname+"."+attr).c_str(),      // PV name including attr
                                    connectionCallback,  // name of connection callback function
                                    pv,               // 
                                    CA_PRIORITY_DEFAULT, // CA Priority
                                    &channel._chid );    // Unique channel id
    SEVCHK(ca_rtn, "ca_create_channel failed");
    ca_rtn = ca_create_subscription( type,          // CA data type
                                     1,                        // number of elements
                                     channel._chid,            // unique channel id
                                     DBE_VALUE,    // event mask (change of value)
                                     eventCallback,            // name of event callback function
                                     pv,
                                     &channel._evid );         // unique event id needed to clear subscription
    SEVCHK(ca_rtn, "ca_create_subscription failed");
    
    pv->channels.push_back(channel);
}

void Epics::subscribe(const string &pvname, PV* pv) {
    
    // create/subscribe for value
    subscribe_channel(pvname, pv, "VAL", DBR_TIME_DOUBLE);
    
    
    // create/subscribe for interesting properties: alarms, operating ranges, unit
    // you may use any DBR_* type except DBR_TIME_DOUBLE, see Epics::eventCallback
    // we don't use the fairly new DBR_CTRL* types to monitor the properties,
    // since many records don't propagate changes correctly...
    subscribe_channel(pvname, pv, "HIHI", DBR_DOUBLE);
    subscribe_channel(pvname, pv, "HIGH", DBR_DOUBLE);
    subscribe_channel(pvname, pv, "LOW",  DBR_DOUBLE);
    subscribe_channel(pvname, pv, "LOLO", DBR_DOUBLE);
    subscribe_channel(pvname, pv, "SEVR", DBR_ENUM);    
    subscribe_channel(pvname, pv, "HOPR", DBR_DOUBLE);
    subscribe_channel(pvname, pv, "LOPR", DBR_DOUBLE);
    subscribe_channel(pvname, pv, "EGU",  DBR_STRING);
    subscribe_channel(pvname, pv, "PREC", DBR_SHORT);
    
    ca_poll();
}

void Epics::removePV(const string& pvname)
{
    // hopefully, the pvname exists :)
    PV* pv = pvs[pvname];
    
    // cancel the subscription/channels
    for(size_t i=0;i<pv->channels.size();i++) {
        int ca_rtn = ca_clear_subscription (pv->channels[i]._evid);
        SEVCHK(ca_rtn, "ca_clear_subscription failed");
    
        ca_rtn = ca_clear_channel(pv->channels[i]._chid);
        SEVCHK(ca_rtn, "ca_clear_channel failed");
    }
    
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
    _watch.Stop();
    return _watch.TimeElapsed();
}

void Epics::processNewDataForPV(const string& pvname) {
    processNewDataForPV(pvs[pvname]);
}

void Epics::processNewDataForPV(PV* pv)
{
      
    // get the pointer to the most recent item 
    // in the list (snapshot of current state)
    DataItem* head = pv->head;
    
    // is there something new in
    // the linked list since the last 
    // call?
    if(pv->head_last != NULL && pv->head_last == head) 
        return;   
    
    // scan the linked list
    typedef vector<DataItem*> list_t;
    list_t list;
    Epics::fillList(head, list);    
    
    // the very first call, _head_last is NULL, thus everything is new!
    bool newData = pv->head_last == NULL;
    
    // go thru the vector in positive time direction (ie reverse direction)
    // note that the linked list (scanned above) starts from the head (most recent item!)    
    for(list_t::reverse_iterator it=list.rbegin(); // reverse begin
        it!=list.rend(); // reverse end
        ++it) {
        
        DataItem* i = (*it);
        if(i->prev == pv->head_last) {
            newData = true;
        }      
        
        if(!newData)
            continue;
        
        // if new, let it be processed
        (pv->cb)(i);
        
    }
    // remember the last head for the next call
    pv->head_last = head;
    
    // do not delete the last two elements, 
    // which are still needed to build the list atomically
    for(list_t::reverse_iterator it=list.rbegin(); // reverse begin
        it<list.rend()-2; // reverse end, but not the last two!
        ++it) {
        // delete the current one properly
        DataItem* cur = *it;
        deleteDataItem(cur);   
        // and tell the next, that it's not pointing backwards to
        // us anymore
        DataItem* next = *(it+1);
        next->prev = NULL;
    }
}



