//******************************************************************************
//! @file    CtrlSysAccessInterfaceEpics.cxx
//!                       
//! @author  F. Feldbauer
//!
//! @brief   Singleton class for the interface between Cooling control and
//!          Slow Control or Simulation, respectively
//!          Derived class for EPICS interfacing
//!
//! @version 1.0.0
//******************************************************************************

//_____ I N C L U D E S ________________________________________________________
// ANSI C/C++ headers
#include <fstream>
#include <iostream>
#include <cmath>
#include <stdint.h>
#include <ctime>
#include <cerrno>

// BOOST headers
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// EPICS CA headers
#include "cadef.h"

// Local headers
#include "Exception.h"
#include "interface/ChillerStatus.h"
#include "interface/CtrlSysAccessInterfaceEpics.h"
#include "interface/CtrlSysAccessInterfaceConstants.h"

using namespace ChillerRegulator;

//_____ D E F I N I T I O N S __________________________________________________
struct ChillerRegulator::PV {
  chid            mychid;
  evid            myevid;
  unsigned int    typeAndPos;
  dbr_time_double dbrval;
  bool            connected;
};
 
//_____ F U N C T I O N S ______________________________________________________

//------------------------------------------------------------------------------
//! @fn      exceptionCallback
//!
//! @brief   EPICS Callback when an exception occured
//------------------------------------------------------------------------------
static void exceptionCallback( exception_handler_args args ) {
  std::string pvname = ( args.chid ? ca_name( args.chid ) : "unknown" );
  throw BadCA( "CA Exception:\n Status: " + std::string( ca_message(args.stat) )
               + "\n Channel: " + pvname );
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
    std::cerr << "CtrlSysAccessInterface has lost connection to Channel "
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
CtrlSysAccessInterfaceEpics::CtrlSysAccessInterfaceEpics ()
  : CtrlSysAccessInterface()
{
  // to be sure these pointers are NULL if not configured in init function
  chiller_front_setpoint_ = NULL;
  chiller_main_setpoint_ = NULL;
  chiller_air_setpoint_ = NULL;
  chiller_front_pumpstage_ = NULL;
  chiller_main_pumpstage_ = NULL;
  chiller_air_pumpstage_ = NULL;
  chiller_front_switch_ = NULL;
  chiller_main_switch_ = NULL;
  chiller_air_switch_ = NULL;
  reg_setVal_ = NULL;
  reg_mode_ = NULL;
  mean_front_ = NULL;
  mean_back_ = NULL;  
}

//------------------------------------------------------------------------------

CtrlSysAccessInterfaceEpics::CtrlSysAccessInterfaceEpics ( const CtrlSysAccessInterfaceEpics& other ) {
}

//------------------------------------------------------------------------------

CtrlSysAccessInterfaceEpics::~CtrlSysAccessInterfaceEpics () {
}

//------------------------------------------------------------------------------
//! @fn      create
//!
//! @brief   Create an instance of CtrlSysAccessInterfaceEpics if there exists no
//!          instance of any CtrlSysAccessInterface class
//------------------------------------------------------------------------------
void CtrlSysAccessInterfaceEpics::create() {
  if ( NULL != pinstance_ )
    std::cerr << "CtrlSysAccessInterface Singleton has already been created" << std::endl;
  else
    pinstance_ = new CtrlSysAccessInterfaceEpics();
  return;
}

//------------------------------------------------------------------------------
//! @fn      init
//!
//! @brief   Initialize CA Client
//!          Read in XML config file
//!          Use only base name for chillers in XML file!
//------------------------------------------------------------------------------
void CtrlSysAccessInterfaceEpics::init( std::string& filename ) {
  using boost::spirit::ascii::no_case;
  using boost::spirit::qi::symbols;
  using boost::property_tree::ptree;

  if( isInitialized_ ) {
    std::cout << "CtrlSysAccessInterface has already been initialized." << std::endl;
    return;
  }

  // populate tree structure pt
  ptree pt;
  symbols<char, unsigned int> senType;
  senType.add
    ("chiller", Internal::type_chiller)
    ("temperature", Internal::type_temperature)
    ("humidity", Internal::type_humidity)
    ("setvalue", Internal::type_setvalue)
    ("mode", Internal::type_mode)
    ("meanvalue", Internal::type_mean)
    ;

  symbols<char, unsigned int> senPos;
  senPos.add
    ("front", Internal::pos_front)
    ("back", Internal::pos_back)
    ("air", Internal::pos_air)
    ;

  std::ifstream input( filename.c_str(), std::ios_base::in );
  if ( !input.is_open() || input.fail() )
    throw BadConfig( "Could not open config file for interface '" + filename + "': " + strerror( errno ) );
  
  read_xml( input, pt );

  epicsTimeStamp timetemp = { 0, 0 };
  dbr_time_double tempDbr = { 0, 0, timetemp, 0, 0 };
  
  // Configure EPICS Channel Access  
  ca_context_create( ca_enable_preemptive_callback );
  ca_add_exception_event( exceptionCallback, NULL );

  BOOST_FOREACH( ptree::value_type const& v, pt.get_child( "pvs" ) ) {
    PV* puser = new PV;
    std::string type, pos;
    std::string pvname, basename;
    unsigned int parsed_type(0), parsed_pos(0);

    puser->connected = false;
    puser->dbrval = tempDbr;

    if( v.first == "channel" ) {
      basename = v.second.get<std::string>("name");

      if ( basename.length() >= MAX_PV_NAME_LEN )
        throw BadConfig( "PV name is too long! " + pvname );

      type = v.second.get<std::string>("type");
      if ( !parse( type.begin(), type.end(), no_case[ senType ], parsed_type ) )
        throw BadConfig( "Invalid option for type: '" + type + "', Channel: " + basename );

      pos = v.second.get<std::string>("pos", "front");
      if ( !parse( pos.begin(), pos.end(), no_case[ senPos ], parsed_pos ) )
        throw BadConfig( "Invalid option for position: '" + pos + "', Channel: " + basename );

      puser->typeAndPos = parsed_type | parsed_pos;

      switch ( parsed_type ) {
      case Internal::type_chiller:
        pvname = basename + ":BathTEMP:Tmom";
        subscribe( pvname, puser );
	puser = new PV;
	puser->connected = false;
	puser->dbrval = tempDbr;
        puser->typeAndPos = parsed_type | parsed_pos;
        pvname = basename + ":HeatPower";
        subscribe( pvname, puser );
	puser = new PV;
	puser->connected = false;
	puser->dbrval = tempDbr;
        puser->typeAndPos = parsed_type | parsed_pos;
        pvname = basename + ":PumpStage";
        subscribe( pvname, puser );
	puser = new PV;
	puser->connected = false;
	puser->dbrval = tempDbr;
        puser->typeAndPos = parsed_type | parsed_pos;
        pvname = basename + ":StatusNumber"; // for better comparison use number instead of string
        subscribe( pvname, puser );
	puser = new PV;
	puser->connected = false;
	puser->dbrval = tempDbr;
        puser->typeAndPos = parsed_type | parsed_pos;
        pvname = basename + ":Tset";
        subscribe( pvname, puser );

	puser = new PV;
	puser->connected = false;
	puser->dbrval = tempDbr;
        puser->typeAndPos = parsed_type | parsed_pos;
        pvname = basename + ":setTset";
        createChannel( pvname, puser, Tset );
	puser = new PV;
	puser->connected = false;
	puser->dbrval = tempDbr;
        puser->typeAndPos = parsed_type | parsed_pos;
        pvname = basename + ":setPumpStage";
        createChannel( pvname, puser, PumpSet );
	puser = new PV;
	puser->connected = false;
	puser->dbrval = tempDbr;
        puser->typeAndPos = parsed_type | parsed_pos;
        pvname = basename + ":setCircCon";
        createChannel( pvname, puser, Switch );
        break;

      case Internal::type_temperature:
      case Internal::type_humidity:
      case Internal::type_setvalue:
      case Internal::type_mode:
        pvname = basename;
        subscribe( pvname, puser );
        break;

      case Internal::type_mean:
        pvname = basename;
        createChannel( pvname, puser, Mean );
        break;

      default:
        throw BadConfig( "invalid type of PV" );
      } 
    }
  }
  
  input.close();
  
  ca_pend_event(1.e-12);

  isInitialized_ = true;
}

//------------------------------------------------------------------------------
//! @fn      close
//!
//! @brief   Close connection to CA Server
//------------------------------------------------------------------------------
void CtrlSysAccessInterfaceEpics::close() {
  std::vector<PV*>::const_iterator it;
  for ( it = pvs_T_F_.begin(); it != pvs_T_F_.end(); ++it ) {
    ca_clear_subscription ( (*it)->myevid );
    ca_clear_channel( (*it)->mychid );
  }
  for ( it = pvs_T_B_.begin(); it != pvs_T_B_.end(); ++it ) {
    ca_clear_subscription ( (*it)->myevid );
    ca_clear_channel( (*it)->mychid );
  }
  for ( it = pvs_H_.begin(); it != pvs_H_.end(); ++it ) {
    ca_clear_subscription ( (*it)->myevid );
    ca_clear_channel( (*it)->mychid );
  }
  for ( it = pvs_C_F_.begin(); it != pvs_C_F_.end(); ++it ) {
    ca_clear_subscription ( (*it)->myevid );
    ca_clear_channel( (*it)->mychid );
  }
  for ( it = pvs_C_B_.begin(); it != pvs_C_B_.end(); ++it ) {
    ca_clear_subscription ( (*it)->myevid );
    ca_clear_channel( (*it)->mychid );
  }
  
  pvs_T_F_.clear();
  pvs_T_B_.clear();
  pvs_H_.clear();
  pvs_C_F_.clear();
  pvs_C_B_.clear();
  pvs_C_A_.clear();

  ca_clear_subscription ( reg_setVal_->myevid );
  ca_clear_channel( reg_setVal_->mychid );
  ca_clear_subscription ( reg_mode_->myevid );
  ca_clear_channel( reg_mode_->mychid );

  ca_clear_channel( chiller_front_setpoint_->mychid );
  ca_clear_channel( chiller_main_setpoint_->mychid );
  ca_clear_channel( chiller_air_setpoint_->mychid );
  ca_clear_channel( chiller_front_pumpstage_->mychid );
  ca_clear_channel( chiller_main_pumpstage_->mychid );
  ca_clear_channel( chiller_air_pumpstage_->mychid );
  ca_clear_channel( chiller_front_switch_->mychid );
  ca_clear_channel( chiller_main_switch_->mychid );
  ca_clear_channel( chiller_air_switch_->mychid );

  ca_clear_channel( mean_front_->mychid );
  ca_clear_channel( mean_back_->mychid );

  ca_context_destroy();
}

//------------------------------------------------------------------------------
//! @fn      meanTemperature
//!
//! @brief   Calculate mean temperature and return value
//------------------------------------------------------------------------------
double CtrlSysAccessInterfaceEpics::meanTemperature( const eTemperature where ) {
  double ans = 0.;
  int count = 0;
  uint64_t timeLimit = time( NULL ) - 300; // Current timestamp minus 5 min
  timeLimit -= POSIX_TIME_AT_EPICS_EPOCH;  // The EPICS Epoch is 00:00:00 Jan 1, 1990 UTC
  timeLimit *= 1000000000;                 // Convert to nanoseconds

  std::vector<PV*>::const_iterator it;
  switch ( where ) {

  case backplate:
    for ( it = pvs_T_B_.begin(); it != pvs_T_B_.end(); ++it ) {
      uint64_t mytime = (*it)->dbrval.stamp.secPastEpoch;
      mytime = ( mytime * 1000000000 ) + (*it)->dbrval.stamp.nsec;
#ifdef DEBUG
	std::cout << "meanTemperature at Back using PV: "
		  << ca_name( (*it)->mychid )
		  << "\n Value " << (*it)->dbrval.value 
		  << " Status " << (*it)->dbrval.status 
		  << " Severity " << (*it)->dbrval.severity
		  << " Time " << mytime
		  << "\n" << timeLimit << "\n";
#endif
      if ( (*it)->dbrval.status <= 6    &&      // status is ok (do I need this? already handled from severity?
           (*it)->dbrval.severity != 3  &&      // severity is valid (but could be minor or major alarm)
	   ( (*it)->dbrval.value >= -40. && (*it)->dbrval.value <= 40. ) &&  // value within +-40 degC
           mytime >= timeLimit ) {              // value is within last 5 minutes

        ans += (*it)->dbrval.value;
        count++;
      }
    }
    if ( 0 == count )
      throw BadTemp( "No valid temperatures found to calculate mean temperature at back" );
    ans /= count;
    break;

  case front:
    for ( it = pvs_T_F_.begin(); it != pvs_T_F_.end(); ++it ) {
      uint64_t mytime = (*it)->dbrval.stamp.secPastEpoch;
      mytime = ( mytime * 1000000000 ) + (*it)->dbrval.stamp.nsec;
#ifdef DEBUG
	std::cout << "meanTemperature at Front using PV: "
		  << ca_name( (*it)->mychid )
		  << "\n Value " << (*it)->dbrval.value 
		  << " Status " << (*it)->dbrval.status 
		  << " Severity " << (*it)->dbrval.severity
		  << " Time " << mytime
		  << "\n" << timeLimit << "\n";
#endif
      if ( (*it)->dbrval.status <= 6    &&      // status is ok (do I need this? already handled from severity?
           (*it)->dbrval.severity != 3  &&      // severity is valid (but could be minor or major alarm)
	   ( (*it)->dbrval.value >= -40 && (*it)->dbrval.value <= 40 ) && // value within +-40 degC
           mytime >= timeLimit ) {              // value is within last 5 minutes
        ans += (*it)->dbrval.value;
        count++;
      }
    }
    if ( 0 == count )
      throw BadTemp( "No valid temperatures found to calculate mean temperature at front" );
    ans /= count;
    break;

  case gradient:
    break;
  }

  return ans;
}

//------------------------------------------------------------------------------
//! @fn      worstHumidity
//!
//! @brief   Determine worst (highest) relative humidity and return value
//------------------------------------------------------------------------------
double CtrlSysAccessInterfaceEpics::worstHumidity() {
  epicsTimeStamp timetemp = { 0, 0 };
  dbr_time_double temp = { 0, 0, timetemp, 0, 0 };
  
  std::vector<PV*>::const_iterator it;
  for ( it = pvs_H_.begin(); it != pvs_H_.end(); ++it ) {
    uint64_t time1 = (*it)->dbrval.stamp.secPastEpoch * 1000000000 + (*it)->dbrval.stamp.nsec;
    uint64_t time2 = temp.stamp.secPastEpoch * 100000000 + temp.stamp.nsec;
    
    if ( (*it)->dbrval.status <= 6         &&      // status is ok (do I need this? already handled from severity?)
         (*it)->dbrval.severity != 3       &&      // severity is valid (but could be minor or major alarm)
         temp.value < (*it)->dbrval.value  &&      // value of iterator is higher
	 (*it)->dbrval.value >= 0 &&               // ignore negative values
         time2 <= time1 ) {                        // value of iterator is "newer"
      
      temp = (*it)->dbrval;
    }
  }
  return temp.value;
}

//------------------------------------------------------------------------------
//! @fn      chiller
//!
//! @brief   Get all values of one chiller
//------------------------------------------------------------------------------
ChillerStatus* CtrlSysAccessInterfaceEpics::chiller( const eChiller myChiller ) {
  std::vector<PV*>::const_iterator it;
  std::vector<data> samples;

  switch ( myChiller ) {

  case main:
    for ( it = pvs_C_B_.begin(); it != pvs_C_B_.end(); ++it ) {
      uint64_t time = (*it)->dbrval.stamp.secPastEpoch + POSIX_TIME_AT_EPICS_EPOCH;
      time *= 1000000000;
      time += (*it)->dbrval.stamp.nsec;
      data temp = { (*it)->dbrval.status,
                    (*it)->dbrval.severity,
                    time,
                    (*it)->dbrval.value };
      samples.push_back( temp );
    }
    break;

  case front:
    for ( it = pvs_C_F_.begin(); it != pvs_C_F_.end(); ++it ) {
      uint64_t time = (*it)->dbrval.stamp.secPastEpoch + POSIX_TIME_AT_EPICS_EPOCH;
      time *= 1000000000;
      time += (*it)->dbrval.stamp.nsec;
      data temp = { (*it)->dbrval.status,
                    (*it)->dbrval.severity,
                    time,
                    (*it)->dbrval.value };
      samples.push_back( temp );
    }
    break;

  case air:
    for ( it = pvs_C_A_.begin(); it != pvs_C_A_.end(); ++it ) {
      uint64_t time = (*it)->dbrval.stamp.secPastEpoch + POSIX_TIME_AT_EPICS_EPOCH;
      time *= 1000000000;
      time += (*it)->dbrval.stamp.nsec;
      data temp = { (*it)->dbrval.status,
                    (*it)->dbrval.severity,
                    time,
                    (*it)->dbrval.value };
      samples.push_back( temp );
    }
    break;
  }

  ChillerStatus* ans = new ChillerStatus( samples );
  return ans;
}

//------------------------------------------------------------------------------
//! @fn      setValue
//!
//! @brief   Return the set value for the regulator
//------------------------------------------------------------------------------
double CtrlSysAccessInterfaceEpics::setValue() {
  return reg_setVal_->dbrval.value;
}

//------------------------------------------------------------------------------
//! @fn      mode
//!
//! @brief   Return the mode for the regulator
//------------------------------------------------------------------------------
bool CtrlSysAccessInterfaceEpics::mode() {
  return ( reg_mode_->dbrval.value >= 1. ) ? true : false;
}

//------------------------------------------------------------------------------
//! @fn      setTemperature
//!
//! @brief   Set new Setpoint for chiller
//------------------------------------------------------------------------------
void CtrlSysAccessInterfaceEpics::setTemperature( const double front_val,
                                                  const double main_val,
                                                  const double air_val ) {
  uint16_t julaboSwitch = 1;

  // front chiller
  if ( chiller_front_setpoint_ ) { // is configured
    if ( !chiller_front_setpoint_->connected )
      throw BadCA( "CA channel for set temperature of front chiller is not connected but configured" );

    int dummy = ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_front_setpoint_->mychid ) ), 
                         chiller_front_setpoint_->mychid,
                         &front_val );
    
    if ( dummy != ECA_NORMAL )
      throw BadCA( "CtrlSysAccessInterface::setTemperature of front chiller failed because:\n"
                   + std::string( ca_message( dummy ) ) + "\n" );
    
    if( pvs_C_F_.at(3)->dbrval.value != 3 ) { // Julabo status unequal to remote start? 3=remote start
      uint16_t julaboSwitch = 1;
      ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_front_switch_->mychid ) ), 
               chiller_front_switch_->mychid,
               &julaboSwitch ); 
    }
  }

  // main chiller
  if ( chiller_main_setpoint_ ) { // is configured
    if ( !chiller_main_setpoint_->connected )
      throw BadCA( "CA channel for set temperature of main chiller is not connected but configured" );

    int dummy = ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_main_setpoint_->mychid ) ), 
                         chiller_main_setpoint_->mychid,
                         &main_val );
    
    if ( dummy != ECA_NORMAL )
      throw BadCA( "CtrlSysAccessInterface::setTemperature of main chiller failed because:\n"
                   + std::string( ca_message( dummy ) ) + "\n" );
    
    if( pvs_C_B_.at(3)->dbrval.value != 3 ) { // Julabo status unequal to remote start? 3=remote start
      ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_main_switch_->mychid ) ), 
               chiller_main_switch_->mychid,
               &julaboSwitch ); 
    }
  }

  // air chiller
  if ( chiller_air_setpoint_ ) { // is configured
    if ( !chiller_front_setpoint_->connected )
      throw BadCA( "CA channel for set temperature of air chiller is not connected but configured" );
    
    int dummy = ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_air_setpoint_->mychid ) ), 
                         chiller_air_setpoint_->mychid,
                         &air_val );
    
    if ( dummy != ECA_NORMAL )
      throw BadCA( "CtrlSysAccessInterface::setTemperature of air chiller failed because:\n"
                   + std::string( ca_message( dummy ) ) + "\n" );
    
    if( pvs_C_A_.at(3)->dbrval.value != 3 ) { // Julabo status unequal to remote start? 3=remote start
      ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_air_switch_->mychid ) ), 
               chiller_air_switch_->mychid,
               &julaboSwitch ); 
    }
  }

  ca_pend_io( 10. );
}

//------------------------------------------------------------------------------
//! @fn      setPumpstage
//!
//! @brief   Set new pump stage for chiller
//------------------------------------------------------------------------------
void CtrlSysAccessInterfaceEpics::setPumpstage( const double front_val,
                                                const double main_val,
                                                const double air_val ) {
  uint16_t julaboSwitch = 1;

  // front chiller
  if ( chiller_front_pumpstage_ ) { // is configured
    if ( !chiller_front_pumpstage_->connected )
      throw BadCA( "CA channel for set temperature of front chiller is not connected but configured" );

    int dummy = ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_front_pumpstage_->mychid ) ), 
                         chiller_front_pumpstage_->mychid,
                         &front_val );
    
    if ( dummy != ECA_NORMAL )
      throw BadCA( "CtrlSysAccessInterface::setTemperature of front chiller failed because:\n"
                   + std::string( ca_message( dummy ) ) + "\n" );
    
    if( pvs_C_F_.at(3)->dbrval.value != 3 ) { // Julabo status unequal to remote start? 3=remote start
      uint16_t julaboSwitch = 1;
      ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_front_switch_->mychid ) ), 
               chiller_front_switch_->mychid,
               &julaboSwitch ); 
    }
  }

  // main chiller
  if ( chiller_main_pumpstage_ ) { // is configured
    if ( !chiller_main_pumpstage_->connected )
      throw BadCA( "CA channel for set temperature of main chiller is not connected but configured" );

    int dummy = ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_main_pumpstage_->mychid ) ), 
                         chiller_main_pumpstage_->mychid,
                         &main_val );
    
    if ( dummy != ECA_NORMAL )
      throw BadCA( "CtrlSysAccessInterface::setTemperature of main chiller failed because:\n"
                   + std::string( ca_message( dummy ) ) + "\n" );
    
    if( pvs_C_B_.at(3)->dbrval.value != 3 ) { // Julabo status unequal to remote start? 3=remote start
      ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_main_switch_->mychid ) ), 
               chiller_main_switch_->mychid,
               &julaboSwitch ); 
    }
  }

  // air chiller
  if ( chiller_air_pumpstage_ ) { // is configured
    if ( !chiller_front_pumpstage_->connected )
      throw BadCA( "CA channel for set temperature of air chiller is not connected but configured" );
    
    int dummy = ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_air_pumpstage_->mychid ) ), 
                         chiller_air_pumpstage_->mychid,
                         &air_val );
    
    if ( dummy != ECA_NORMAL )
      throw BadCA( "CtrlSysAccessInterface::setTemperature of air chiller failed because:\n"
                   + std::string( ca_message( dummy ) ) + "\n" );
    
    if( pvs_C_A_.at(3)->dbrval.value != 3 ) { // Julabo status unequal to remote start? 3=remote start
      ca_put ( dbf_type_to_DBR( ca_field_type ( chiller_air_switch_->mychid ) ), 
               chiller_air_switch_->mychid,
               &julaboSwitch ); 
    }
  }


  ca_pend_io( 10. );
}

//------------------------------------------------------------------------------
//! @fn      writeMeanValues
//!
//! @brief   WriteMeanValues back to control system
//------------------------------------------------------------------------------
void CtrlSysAccessInterfaceEpics::writeMeanValues( const double front_val,
                                                   const double main_val ) {
 
  if ( !mean_front_->connected || !mean_back_->connected )
    throw BadCA("CA channels for writeMeanValues are not connected");

  int temp_front = ca_put ( dbf_type_to_DBR( ca_field_type ( mean_front_->mychid ) ), 
                            mean_front_->mychid,
                            &front_val ); 

  int temp_back = ca_put ( dbf_type_to_DBR( ca_field_type ( mean_back_->mychid ) ), 
                           mean_back_->mychid,
                           &main_val ); 
  if ( temp_front != ECA_NORMAL || temp_back != ECA_NORMAL )
    throw BadCA("CA put failed for CtrlSysAccessInterfaceEpics::setPumpstage because\n"
                + std::string( ca_message( temp_front ) ) + "\n"
                + std::string( ca_message( temp_back ) ) );

  ca_pend_io( 10. );
}

//------------------------------------------------------------------------------
//! @fn      subscribe
//!
//! @brief   Create CA Channel and Subscription for PV named pvname
//------------------------------------------------------------------------------
void CtrlSysAccessInterfaceEpics::subscribe( std::string& pvname, PV* puser ) {
  int ca_rtn = ca_create_channel( pvname.c_str(),      // PV name
                                  connectionCallback,  // name of connection callback function
                                  puser,               // 
                                  CA_PRIORITY_DEFAULT, // CA Priority
                                  &puser->mychid );    // Unique channel id
  
  if ( ca_rtn != ECA_NORMAL )
    throw BadCA( "CtrlSysAccessInterfaceEpics: Unable to create channel '" + pvname
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
    throw BadCA( "CtrlSysAccessInterfaceEpics: Unable to subscribe to channel '" + pvname
                 + "'\n'ca_create_subscription' returned: "
                 + ca_message( ca_rtn ) );

  switch ( puser->typeAndPos ) {
  case ( Internal::type_chiller | Internal::pos_front ):
    pvs_C_F_.push_back( puser );
    break;
  case ( Internal::type_chiller | Internal::pos_back ):
    pvs_C_B_.push_back( puser );
    break;
  case ( Internal::type_chiller | Internal::pos_air ):
    pvs_C_A_.push_back( puser );
    break;
    
  case ( Internal::type_temperature | Internal::pos_front ):
    pvs_T_F_.push_back( puser );
    break;
  case ( Internal::type_temperature | Internal::pos_back ):
    pvs_T_B_.push_back( puser );
    break;
    
  case Internal::type_humidity:
    pvs_H_.push_back( puser );
    break;

  case Internal::type_setvalue:
    reg_setVal_ = puser;
    break;

  case Internal::type_mode:
    reg_mode_ = puser;
    break;

  default:
    throw BadConfig( "Invalid combination of type and position for '" + pvname = "'" );
  }
}

//------------------------------------------------------------------------------
//! @fn      createChannel
//!
//! @brief   Create CA Channel for Chiller set temperature and pump stage
//------------------------------------------------------------------------------
void CtrlSysAccessInterfaceEpics::createChannel( std::string& pvname, PV* puser, channelType what ) {
  int ca_rtn = ca_create_channel( pvname.c_str(),      // PV name
                                  connectionCallback,  // name of connection callback function
                                  puser,               // 
                                  CA_PRIORITY_DEFAULT, // CA Priority
                                  &puser->mychid );    // Unique channel id
  
  if ( ca_rtn != ECA_NORMAL )
    throw BadCA( "CtrlSysAccessInterfaceEpics: Unable to create channel '" + pvname
                 + "'\n'ca_create_channel' returned: "
                 + ca_message( ca_rtn ) );

  unsigned int pos = puser->typeAndPos & Internal::pos_mask;
  switch ( pos ) {
  case Internal::pos_front:
    switch ( what ) {
    case Tset:     chiller_front_setpoint_  = puser; break;
    case PumpSet:  chiller_front_pumpstage_ = puser; break;
    case Switch:   chiller_front_switch_    = puser; break;
    case Mean:     mean_front_              = puser; break;
    }
    break;
  case Internal::pos_back:
    switch ( what ) {
    case Tset:     chiller_main_setpoint_  = puser; break;
    case PumpSet:  chiller_main_pumpstage_ = puser; break;
    case Switch:   chiller_main_switch_    = puser; break;
    case Mean:     mean_back_              = puser; break;
    }
    break;
  case Internal::pos_air:
    switch ( what ) {
    case Tset:     chiller_air_setpoint_  = puser; break;
    case PumpSet:  chiller_air_pumpstage_ = puser; break;
    case Switch:   chiller_air_switch_    = puser; break;
    default:
      throw BadConfig("Invalid combination of position and type (meanValue and air)");
    }
    break;
  }
}

//******************************************************************************
//! EOF
//******************************************************************************
