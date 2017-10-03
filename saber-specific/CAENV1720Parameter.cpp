#include <iostream>
#include <cstring>

#include "CAENV1720Parameter.h"

using namespace std;


CAENV1720Parameter::CAENV1720Parameter() : VMEBoardParameter(){

    // Sets default values.
    link_number = 0;
    board_number = -1;
    base_addr = 0x32110000;

    /* trigger settings */
    for(int i=0;i<8;i++){
        threshold[i] = 2046;
        tcrossthresh[i] = 0x4;
        dac[i] = 0xfd66;
    }

    trig_overlap = false;
    trig_over_threshold = true;

    ch_enable_mask = 0x0;       // by default no channel enabled

    local_trig_enable = 0x0;    // local trigger disable
    sw_trig_enable = false;
    ext_trig_enable = false;    // external trig and sw trig disabled

    /* acquisition control */
    runmode = REG_CON;

    /* event organization */
    pre_trig_sample = 1024;
    post_trig_sample = 1024;
    enable_custom_size = false;

    // manually set buffer code here
    buff_code =  0x00;
    uint32_t s = pre_trig_sample + post_trig_sample;
    for( unsigned int i = 0; i< 10; i++ ){
        if( s <= (uint32_t(1024)<<i) )
            buff_code = (0x0A - i*0x01);
        break;
    }

    /* front panel and trigger outpout */
    local_fp_trigout = 0x0;
    sw_fp_trigout = false;
    ext_fp_trigout = false;

    logic_level_ttl = true; // TTL and LVDS output by default.
    lvds_io_output = true;
}



CAENV1720Parameter::~CAENV1720Parameter(){;}



void CAENV1720Parameter::Print(){
/*
    VMEBoardParameter::Print();
    for( int i=0; i<8; ++i){
        cout<<"\n\t"<<"Channel "<<i<< ( ((ch_enable_mask & (0x1<<i)) ==0 ) ? " disabled" : " enabled");
        cout<<"\n\t\t"<<"Local trigger "<<i<< (( (local_trig_enable & (0x1<<i)) ==0 ) ? " disabled" : " enabled");
        cout<<"\n\t\t"<<"Local trigger FP out "<<i<< (( (local_fp_trigout & (0x1<<i)) ==0 ) ? " disabled" : " enabled");
        cout<<"\n\t\t"<<"Threshold "<<threshold[i];
        cout<<"\n\t\t"<<"Time cross threshold "<<tcrossthresh[i];
        cout<<"\n\t\t"<<"DAC "<<dac[i];
    }
    cout<<"\n\t"<<"Trigger overlap "<< (( trig_overlap ) ? "enabled" : "disabled");
    cout<<"\n\t"<<"Trigger overthreahold "<< (( trig_over_threshold ) ? "true" : "false");

    cout<<"\n\t"<<"SW trigger "<< (( sw_trig_enable ) ? "enabled" : " disabled");
    cout<<"\n\t"<<"Ext trigger "<< (( ext_trig_enable ) ? "enabled" : "disabled");
    cout<<"\n\t"<<"FP sw trigger out "<< (( sw_fp_trigout ) ? "enabled" : " disabled");
    cout<<"\n\t"<<"FP ext trigger out "<< (( ext_fp_trigout ) ? "enabled" : "disabled");

    cout<<"\n\t"<<"Custom size "<< (( enable_custom_size ) ? "enabled" : "disabled");
    cout<<"\n\t"<<"Pre trigger sample "<< dec << pre_trig_sample;
    cout<<"\n\t"<<"Post trigger sample "<< dec << post_trig_sample;
    cout<<"\n\t"<<"Buffer code "<<hex<<buff_code;
    cout<<"\n\t"<<"Logic level "<< (logic_level_ttl ? "TTL" : "NIM");
    cout<<"\n\t"<<"LVDS IO direction "<< (lvds_io_output ? "OUT" : "IN");
    cout<<endl;
*/
}



void CAENV1720Parameter::SetParamFromConfig( ConfigParser* p, string dir){

    // if didn't find the required directory, return default settings.
    if( ! p->Find(dir) ){
        p->Print( " v1720 : In the parameter file did not find parameters for V1720. Using defaults.\n", ERR);
        return ;
    }

    // dir should be specified as /dir/
    else if( (*dir.rbegin())!='/' ){
        cerr<<"ERROR: "<<dir<<" is in wrong format for specifying directory.\n";
        cerr<<"       Please specify directory as /dir/"<<endl;
        return ;
    }

    SetLinkNumber( p->GetInt( dir+"link_number", 0));
    SetBoardNumber( p->GetInt( dir+"board_number", 0));
    SetBaseAddr( p->GetInt( dir+"address", 0x32110000) );


    /***********************************************************************************/
    /* local channel settings, threshold, DAC, enable mask, local trig, fp out.        */
    /***********************************************************************************/

    string gl = "/module/daq/board*/channel*/";
    string wch = dir+"channel*/";
        // channel wildcard. Settings apply to all channels not explicitly specified.

    // first apply channel global settings.
    //
    // enable trigger mask, false by default.
    ch_enable_mask = 0x0;
    local_trig_enable = 0x0;
    local_fp_trigout = 0x0;

    // if at global level enable is found, all bits are enabled.
    if ( p->GetBool( gl+"enable", false ) )
        ch_enable_mask = 0xff;
    if ( p->GetBool( wch+"enable", false ) )
        ch_enable_mask = 0xff;

    if ( p->GetBool( gl+"local_trigger_enable", false) )
        local_trig_enable = 0xff;
    if ( p->GetBool( wch+"local_trigger_enable", false) )
        local_trig_enable = 0xff;

    if ( p->GetBool( gl+"fp_local_trigger_out_enable", false) )
        local_fp_trigout = 0xff;
    if ( p->GetBool( wch+"fp_local_trigger_out_enable", false) )
        local_fp_trigout = 0xff;

    // settings to each local channel
    
    //ch_enable_mask = 0x0;

    for( int i=0; i<8; ++i){
        stringstream ss;
        ss << dir << "channel" << i << "/";
        string ch = ss.str();

        if( p->GetBool( ch+"enable", false) ){ // only when the channel is enabled.

        // first check global settings, then check individual channel to overwrite changes.
            threshold[i] = p->GetInt(gl+"threshold", threshold[i]);
            threshold[i] = p->GetInt(wch+"threshold", threshold[i]);
            threshold[i] = p->GetInt(ch+"threshold", threshold[i]);

            tcrossthresh[i] = p->GetInt( gl+"time_cross_threshold", tcrossthresh[i]);
            tcrossthresh[i] = p->GetInt( wch+"time_cross_threshold", tcrossthresh[i]);
            tcrossthresh[i] = p->GetInt( ch+"time_cross_threshold", tcrossthresh[i]);

            dac[i] = p->GetInt( gl+"DAC", dac[i]);
            dac[i] = p->GetInt( wch+"DAC", dac[i]);
            dac[i] = p->GetInt( ch+"DAC", dac[i]);

            ch_enable_mask |= (0x1<<i);
        }
        
        // local channel as trigger source
        if( p->Find( ch+"local_trigger_enable")){
            if( p->GetBool( ch+"local_trigger_enable", false))
                local_trig_enable |= (0x1<<i);
            else
                local_trig_enable &= (~(0x1<<i));
        }
        //
        // local trigger front panel output
        if( p->Find( ch+"fp_local_trigger_out_enable")){
            if( p->GetBool( ch+"fp_local_trigger_out_enable", false))
                local_fp_trigout |= (0x1<<i);
            else
                local_fp_trigout &= (~(0x1<<i));
        }
    }


    /* trigger over threshold and trigger overlap */
    trig_over_threshold = p->GetBool( "/board*/trigger_over_threshold", trig_over_threshold);
    trig_over_threshold = p->GetBool( dir+"trigger_over_threshold", trig_over_threshold);

    trig_overlap = p->GetBool( "/board*/trigger_overlap", trig_overlap);
    trig_overlap = p->GetBool( dir+"trigger_overlap", trig_overlap);


    /* software trigger and external trigger */
    sw_trig_enable = p->GetBool( "/board*/software_trigger_enable", sw_trig_enable);
    sw_trig_enable = p->GetBool( dir+"software_trigger_enable", sw_trig_enable);
    
    ext_trig_enable = p->GetBool( "/board*/external_trigger_enable", ext_trig_enable);
    ext_trig_enable = p->GetBool( dir+"external_trigger_enable", ext_trig_enable);


    /* software and external front-panel trigger output */
    ext_fp_trigout = p->GetBool( "/board*/fp_external_trigger_out_enable", ext_fp_trigout);
    ext_fp_trigout = p->GetBool( dir+"fp_external_trigger_out_enable", ext_fp_trigout);

    sw_fp_trigout = p->GetBool( "/board*/fp_software_trigger_out_enable", sw_fp_trigout);
    sw_fp_trigout = p->GetBool( dir+"fp_software_trigger_out_enable", sw_fp_trigout);
    cout << "FP : " << sw_fp_trigout << endl;


    /* front panel logic type and LVDS IO direction*/
    logic_level_ttl = p->GetBool( "/board*/logic_TTL", logic_level_ttl);
    logic_level_ttl = p->GetBool( dir+"logic_TTL", logic_level_ttl);

    lvds_io_output = p->GetBool( "/board*/LVDS_IO_out", lvds_io_output);
    lvds_io_output = p->GetBool( dir+"LVDS_IO_out", lvds_io_output);


    /* acquisition control */
    string mode = p->GetString( "/run_mode", "register_controlled");
        // first check run mode in global root space.
    mode = p->GetString( "/board*/run_mode", mode);
    mode = p->GetString( dir+"run_mode", mode);
    if(mode=="first_trigger_controlled")
        runmode = FIRST_TRIG_CON;
    else
        runmode = REG_CON;


    /* customer size enable/disable */
    enable_custom_size = p->GetBool( "/custom_size_enable", enable_custom_size);
    enable_custom_size = p->GetBool( "/board*/custom_size_enable", enable_custom_size);
    enable_custom_size = p->GetBool( dir+"custom_size_enable", enable_custom_size);


    /* event size */
    float pre(0), post(0);
    pre = p->GetFloat( "/pre_trigger_window_us", 0.1);
    pre = p->GetFloat( "/board*/pre_trigger_window_us", pre);
    pre = p->GetFloat( dir+"pre_trigger_window_us", pre);
    post = p->GetFloat( "/post_trigger_window_us", 0.9);
    post = p->GetFloat( "/board*/post_trigger_window_us", post);
    post = p->GetFloat( dir+"post_trigger_window_us", post);

    pre_trig_sample = 4*(int(pre*250)/4);
    post_trig_sample = 4*(int(post*250)/4);

    // manually set buffer code here
    buff_code =  0x00;
    uint32_t sum = pre_trig_sample + post_trig_sample;

    if( sum <= 1024)
        buff_code = 0x0a;
    else if( sum<= (1024<<1))
        buff_code = 0x09;
    else if( sum<= (1024<<2))
        buff_code = 0x08;
    else if( sum<= (1024<<3))
        buff_code = 0x07;
    else if( sum<= (1024<<4))
        buff_code = 0x06;
    else if( sum<= (1024<<5))
        buff_code = 0x05;
    else if( sum<= (1024<<6))
        buff_code = 0x04;
    else if( sum<= (1024<<7))
        buff_code = 0x03;
    else if( sum<= (1024<<8))
        buff_code = 0x02;
    else if( sum<= (1024<<9))
        buff_code = 0x01;
    else
        buff_code = 0x0;

    //for( unsigned int i = 0; i<10; i++ )
    //    if( sum <= (1024<<i) ) param.buff_code = (0x0A - i*0x01);
    //cout<<"Total sample size is: "<<dec<<sum<<"\tBuffer code is: "<<param.buff_code<<endl;
    return;
}



int CAENV1720Parameter::GetNChanEnabled(){
    int c = 0;  uint32_t val = ch_enable_mask;
    for(int i=0;i<8;i++){
        if( (val&0x1)==0x1 ) c++;
        val = (val>>1);
    }
    return c;
}



uint32_t CAENV1720Parameter::GetEvtSizeInSamp(){
    int n = GetNChanEnabled();
    if(enable_custom_size)
        return n * (pre_trig_sample + post_trig_sample);
    else
        return n* (1024*1024)/(0x1<<buff_code);
}



uint32_t CAENV1720Parameter::GetEvtSizeInWord(){
    return GetEvtSizeInSamp()/2+4;
}



uint32_t CAENV1720Parameter::GetEvtSizeInByte(){
    return GetEvtSizeInWord()*4;
}


int CAENV1720Parameter::GetHeaderSize(){
    int bytes = 0;

    bytes += 3*8*sizeof(threshold[0]);
        // threshold, tcrossthresh, dac
    bytes += 16*sizeof(ch_enable_mask);

    return bytes;
}


void CAENV1720Parameter::Serialize( char* p){

    vector<uint32_t> data;

    data.push_back( base_addr);

    for( int i=0; i<8; ++i){
        data.push_back( threshold[i] );
        data.push_back( tcrossthresh[i]);
        data.push_back( dac[i]);
    }

    data.push_back( ch_enable_mask);
    data.push_back( trig_over_threshold);
    data.push_back( trig_overlap);
    data.push_back( enable_custom_size);
    data.push_back( pre_trig_sample);
    data.push_back( post_trig_sample);
    data.push_back( buff_code);
    data.push_back( runmode);
    data.push_back( lvds_io_output);
    data.push_back( logic_level_ttl);
    data.push_back( sw_trig_enable);
    data.push_back( sw_fp_trigout);
    data.push_back( ext_trig_enable);
    data.push_back( ext_fp_trigout);
    data.push_back( local_trig_enable);
    data.push_back( local_fp_trigout);

    for( unsigned int i=0; i<data.size(); ++i){
        memcpy( p+i*sizeof(uint32_t), &data[i], sizeof( uint32_t) );
    }
}


void CAENV1720Parameter::Deserialize( char* p, bool flip){

    int offset = 0;
    vector<uint32_t> data;
    uint32_t temp = 0;

    for( int i=0; i<GetHeaderSize(); ++i){
        memcpy( &temp, p+i*sizeof(uint32_t), sizeof( uint32_t) );
        data.push_back(temp);
    }

    base_addr = data[0];    ++offset;
    for( int i=0; i<8; ++i){
        threshold[i] = data[1+3*i];
        ++offset;
        tcrossthresh[i] = data[2+3*i];
        ++offset;
        dac[i] = data[3+3*i];
        ++offset;
    }

    ch_enable_mask = data[offset++];
    trig_over_threshold = data[offset++];
    trig_overlap = data[offset++];
    enable_custom_size = data[offset++];
    pre_trig_sample = data[offset++];
    post_trig_sample = data[offset++];
    buff_code = data[offset++];
    runmode = static_cast<V1720_RUNMODE>( data[offset++]);
    lvds_io_output = data[offset++];
    logic_level_ttl = data[offset++];
    sw_trig_enable = data[offset++];
    sw_fp_trigout = data[offset++];
    ext_trig_enable = data[offset++];
    ext_fp_trigout = data[offset++];
    local_trig_enable = data[offset++];
    local_fp_trigout = data[offset++];
}
