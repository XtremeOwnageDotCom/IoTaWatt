/*
  IotaLog.h - Library for IotaLog SD log files
  Created by Bob Lemaire, May 2016
*/

#ifndef IotaLog_h
#define IotaLog_h
#include "SPI.h"
#include "SD.h"

/*******************************************************************************************************
********************************************************************************************************
Class IotaLog
Maintains a log of fixed length, consecutive entries.
All entries must be written with increasing keys.
Entries are read by key value.
When reading by key, the entry with the requested or next lower key is returned with the requested key.
********************************************************************************************************
********************************************************************************************************/
struct IotaLogRecord {
      uint32_t UNIXtime;        // Time period represented by this record
      uint32_t serial;        // record number in file
      double logHours;        // Total hours of monitoring logged to date in this log 
      struct channels {
        double accum1;
        channels(){accum1 = 0;}
      } channel[30];
      IotaLogRecord(){UNIXtime=0; serial=0; logHours=0;};
    };

class IotaLog
{
  public:

	IotaLog(int interval=5, uint32_t days = 365) {
		_interval = interval;
		_recordSize = sizeof(IotaLogRecord);
		_maxFileSize = days * _recordSize * (86400 / _interval);
		_lastReadKey = 0;
		_lastReadSerial = 0;
		_readKeyIO = 0;
		_wrap = 0;
		_firstKey = 0;
		_firstSerial = 0;
		_lastKey = 0;
		_lastSerial = -1;
		_entries = 0;
	}
	
	~IotaLog(){
		IotaFile.close();
	}
	      
    int begin (const char* /* filepath */);
    int write (IotaLogRecord* /* pointer to record to be written*/);
    int readKey (IotaLogRecord* /* pointer to caller's buffer */);
    int readSerial(IotaLogRecord* callerRecord, uint32_t serial); 
    int readNext(IotaLogRecord* /* pointer to caller's buffer */);
    int end();
    
    boolean  isOpen();
    uint32_t firstKey();
    uint32_t firstSerial();
    uint32_t lastKey();
    uint32_t lastSerial();
    uint32_t fileSize();
	uint32_t readKeyIO();
	uint32_t interval();
	 	
      
    void     dumpFile();

  private:
        
	File 	 IotaFile;
	String    ndxPath;						// For backward compatibility during transition
       
    uint16_t _interval;	                    // Posting interval to log. Currently tested only using 5.
    uint16_t _recordSize;      	  		    // Size of a log record
    uint32_t _fileSize;                     // Size of file in bytes
    uint32_t _entries;                      // Number of entries (fileSize / recsize(256))
    uint32_t _maxFileSize;					// Maximum filesize in bytes
    uint32_t _firstKey;                     // Key of first logical record
    uint32_t _firstSerial;                  // Serial of...
    uint32_t _lastKey;                      // Key of last logical record
    uint32_t _lastSerial;                   // Serial of...
    uint32_t _wrap;                         // Offset of logical record zero (0 if file not wrapped)
  
    uint32_t _lastReadKey;           	    // Key of last record read with readKey
    uint32_t _lastReadSerial;         	    // Serial of last...
    uint32_t _readKeyIO;              	    // Running count of I/Os for keyed reads
    
    uint32_t  findWrap(uint32_t highPos, uint32_t highKey, uint32_t lowPos, uint32_t lowKey);
    void      searchKey(IotaLogRecord* callerRecord, const uint32_t key,
                        const uint32_t lowKey, const uint32_t lowSerial, 
                        const uint32_t highKey, const uint32_t highSerial);
      
};

#endif