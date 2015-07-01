/*  */

/* mongo client */

#include <mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#define _POSIX_C_SOURCE 200809L

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define VALUE_TYPE_TEMP_CENTIGRADE 1
#define VALUE_TYPE_RELATIVE_HUMIDITY_PERCENT 2


#ifdef REAL_TIME
intmax_t get_current_time_in_ms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
clock_serv_t cclock;
mach_timespec_t mts;
host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
clock_get_time(cclock, &mts);
mach_port_deallocate(mach_task_self(), cclock);
spec.tv_sec = mts.tv_sec;
spec.tv_nsec = mts.tv_nsec;

#else
clock_gettime(CLOCK_MONOTONIC, &spec);
#endif

    s  = spec.tv_sec*1000 + round(spec.tv_nsec / 1.0e6);
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return (intmax_t)s;
}
#else
intmax_t get_current_time_in_ms (void)
{
	// fake implementation for performance tests
	static int t = 0;
	return t++;
}

#endif

#define BUFSIZE 1000

int
main (int   argc,
      char *argv[])
{
  int value, value_type, time_ms, sensor_id;
   mongoc_client_t *client;
   mongoc_collection_t *collection;
   mongoc_cursor_t *cursor;
   bson_error_t error;
   const char *uristr = "mongodb://127.0.0.1/:27017";
   const char *collection_name = "test";
   bson_oid_t oid;
   bson_t *doc;
   char *str;
   mongoc_bulk_operation_t *bulk;
   
   mongoc_init ();

   if (argc > 1) {
      uristr = argv [1];
   }

   if (argc > 2) {
      collection_name = argv [2];
   }

   client = mongoc_client_new (uristr);

   if (!client) {
      fprintf (stderr, "Failed to parse URI.\n");
      return EXIT_FAILURE;
   }
   collection = mongoc_client_get_collection (client, "test", "test");

    
int iValues = 100;
int iSensors = 1000;

if (argc > 4) {
	// usage: mongoc_bulk_flexible uri collection batches batch_size
	iValues=atoi(argv[3]); // Number of batches
	iSensors=atoi(argv[4]); // Batch size
}

int i,j;
bool ret;
bson_t reply;

intmax_t t;
 	for(i=0;i<iValues;i++) {
	    bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);
		
 		t = get_current_time_in_ms();
	for(j=0;j<iSensors;j++) {
		value = i+j;
		value_type = VALUE_TYPE_TEMP_CENTIGRADE;
		time_ms = t;
		sensor_id = j;
        doc = BCON_NEW ("value", BCON_INT32 (value));
		    bson_oid_init (&oid, NULL);
		    BSON_APPEND_OID (doc, "_id", &oid);
		    BSON_APPEND_INT32 (doc, "time_ms", time_ms);
		    BSON_APPEND_INT32 (doc, "value_type", value_type);
		    BSON_APPEND_INT32 (doc, "sensor_id", sensor_id);

	        mongoc_bulk_operation_insert (bulk, doc);
	         bson_destroy (doc);
 	}
    ret = mongoc_bulk_operation_execute (bulk, &reply, &error);
	
    mongoc_bulk_operation_destroy (bulk);
   }
 



   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);

   mongoc_cleanup ();

   return EXIT_SUCCESS;
}
