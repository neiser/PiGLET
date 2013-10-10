//******************************************************************************
//! @file    Epics.cpp
//!                       
//! @author  F. Feldbauer, A. Neiser
//!
//! @brief   Class for EPICS interfacing
//!
//! @version 1.0.0
//******************************************************************************

#include <iostream>
#include <cadef.h>
#include "Epics.h"

using namespace std;
using namespace PiGLPlot;


struct PiGLPlot::PV {
  chid            mychid;
  evid            myevid;
  dbr_time_double dbrval;
  bool            connected;
};

//------------------------------------------------------------------------------
//! @fn      exceptionCallback
//!
//! @brief   EPICS Callback when an exception occured
//------------------------------------------------------------------------------
static void exceptionCallback( exception_handler_args args ) {
  std::string pvname = ( args.chid ? ca_name( args.chid ) : "unknown" );
  throw string( "CA Exception:\n Status: " + string( ca_message(args.stat) )
               + "\n Channel: " + pvname);
}

//------------------------------------------------------------------------------
//! @fn      connectionCallback
//!
//! @brief   EPICS Callback when PV gets (re-)connected or disconnected
//------------------------------------------------------------------------------
static void connectionCallback( connection_handler_args args ) {
#ifdef DEBUG
  std::cout << "ConnectionCallback:"
            << "\n PV:       " << ca_name( args.chid )
            << "\n Type:     " << ca_field_type( args.chid )
            << "\n Elements: " << ca_element_count( args.chid )
            << "\n Host:     " << ca_host_name( args.chid )
            << "\n Event:    " << ( args.op == CA_OP_CONN_UP ? "Connected" : " Disconnected" )
            << "\n";
#endif
  
  if ( args.op == CA_OP_CONN_UP ) {
    // channel has connected
    PV* puser = (PV*)ca_puser( args.chid );
    puser->connected = true;

    // Create Subscription here or in init function?
    // PRO: 100% sure to use the correct datatype
    // CONTRA: how to invoke ca_pend_event()? should only be invoked after ALL subscribtions are made

    // ca_create_subscription( dbf_type_to_DBR_TIME( ca_field_type ( args.chid ) ), // CA data type
    //                         ca_element_count ( args.chid ),                      // number of elements
    //                         args.chid,                                           // unique channel id
    //                         DBE_VALUE | DBE_ALARM,                               // event mask (change of value and alarm)
    //                         eventCallback,                                       // name of event callback function
    //                         puser,
    //                         &puser->myevid );                                    // unique event id needed to clear subscription

  } else { // args.op == CA_OP_CONN_DOWN
    // channel has disconnected
    std::cerr << "Epics has lost connection to Channel "
              << ca_name( args.chid )
              << "\n\n";
    PV* puser = (PV*)ca_puser( args.chid );
    puser->connected = false;
  }
}

//------------------------------------------------------------------------------
//! @fn      eventCallback
//!
//! @brief   EPICS Callback on an event in CA, e.g. updated value
//------------------------------------------------------------------------------
static void eventCallback( event_handler_args args ) {
#ifdef DEBUG
  std::cout << "eventCallback:"
	    << "\n PV:      " << ca_name( args.chid )
	    << "\n Type     " << ca_field_type( args.chid )
	    << "\n Elements " << ca_element_count( args.chid )
	    << "\n Host     " << ca_host_name( args.chid )
	    << "\n";
#endif

  if ( args.status != ECA_NORMAL ) {
    // some error
  } else {
    dbr_time_double* temp = (dbr_time_double*)args.dbr; // Convert void* to correct data type
    PV* puser = (PV*)ca_puser( args.chid );             // get pointer to corresponding PV struct
    puser->dbrval = *temp;                              // Store the new value
  }
}

//------------------------------------------------------------------------------
//! @fn      constructors and destructors
//------------------------------------------------------------------------------
PiGLPlot::Epics::Epics ()
{

}


//------------------------------------------------------------------------------

PiGLPlot::Epics::~Epics () {
}

//------------------------------------------------------------------------------
//! @fn      init
//!
//! @brief   Initialize CA Client
//------------------------------------------------------------------------------
void PiGLPlot::Epics::init() {

  if( isInitialized_ ) {
    std::cout << "Epics has already been initialized." << std::endl;
    return;
  }

  ca_pend_event(1.e-12);

  isInitialized_ = true;
}

//------------------------------------------------------------------------------
//! @fn      close
//!
//! @brief   Close connection to CA Server
//------------------------------------------------------------------------------
void PiGLPlot::Epics::close() {
  std::vector<PV*>::const_iterator it;
  for ( it = pvs.begin(); it != pvs.end(); ++it ) {
    ca_clear_subscription ( (*it)->myevid );
    ca_clear_channel( (*it)->mychid );
  }
  
  pvs.clear();
  ca_context_destroy();
}


//------------------------------------------------------------------------------
//! @fn      subscribe
//!
//! @brief   Create CA Channel and Subscription for PV named pvname
//------------------------------------------------------------------------------
void PiGLPlot::Epics::subscribe( std::string& pvname, PV* puser ) {
  int ca_rtn = ca_create_channel( pvname.c_str(),      // PV name
                                  connectionCallback,  // name of connection callback function
                                  puser,               // 
                                  CA_PRIORITY_DEFAULT, // CA Priority
                                  &puser->mychid );    // Unique channel id
  
  if ( ca_rtn != ECA_NORMAL )
    throw string( "Epics: Unable to create channel '" + pvname
                 + "'\n'ca_create_channel' returned: "
                 + ca_message( ca_rtn ) );
  
  ca_rtn = ca_create_subscription( DBR_TIME_DOUBLE,          // CA data type
                                   1,                        // number of elements
                                   puser->mychid,            // unique channel id
                                   DBE_VALUE | DBE_ALARM,    // event mask (change of value and alarm)
                                   eventCallback,            // name of event callback function
                                   puser,
                                   &puser->myevid );         // unique event id needed to clear subscription
    
  if ( ca_rtn != ECA_NORMAL )
    throw string( "Epics: Unable to subscribe to channel '" + pvname
                 + "'\n'ca_create_subscription' returned: "
                 + ca_message( ca_rtn ) );


  pvs.push_back( puser ); 
}
