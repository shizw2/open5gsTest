#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mongoc.h>

#define DATABASE_NAME "test"
#define COLLECTION_NAME "subscriber"
#define NUM_RECORDS 100000
int num_threads;
typedef struct {
    char imsi[16];
    char msisdn[21];
    char slice[31];
    char security[51];
} subscriber_t;

typedef struct {
    int start;
    int end;
} thread_args_t;


bson_t *test_db_new_simple(char * imsi)
{
    bson_t *doc = NULL;

    doc = BCON_NEW(
            "imsi", BCON_UTF8(imsi),
            "msisdn", "[",
                BCON_UTF8("123456789012"),
                BCON_UTF8("123456789013"),
            "]",
            "ambr", "{",
                "downlink", "{",
                    "value", BCON_INT32(1),
                    "unit", BCON_INT32(3),
                "}",
                "uplink", "{",
                    "value", BCON_INT32(1),
                    "unit", BCON_INT32(3),
                "}",
            "}",
            "slice", "[", "{",
                "sst", BCON_INT32(1),
                "default_indicator", BCON_BOOL(true),
                "session", "[", "{",
                    "name", BCON_UTF8("internet"),
                    "type", BCON_INT32(3),
                    "ambr", "{",
                        "downlink", "{",
                            "value", BCON_INT32(1),
                            "unit", BCON_INT32(3),
                        "}",
                        "uplink", "{",
                            "value", BCON_INT32(1),
                            "unit", BCON_INT32(3),
                        "}",
                    "}",
                    "qos", "{",
                        "index", BCON_INT32(9),
                        "arp", "{",
                            "priority_level", BCON_INT32(8),
                            "pre_emption_vulnerability", BCON_INT32(1),
                            "pre_emption_capability", BCON_INT32(1),
                        "}",
                    "}",
                "}", "]",
            "}", "]",
            "security", "{",
                "k", BCON_UTF8("12312314324234234324323"),
                "opc", BCON_UTF8("1111111111111111111111"),
                "amf", BCON_UTF8("8000"),
                "sqn", BCON_INT64(64),
            "}",
            "subscribed_rau_tau_timer", BCON_INT32(12),
            "network_access_mode", BCON_INT32(0),
            "subscriber_status", BCON_INT32(0),
            "access_restriction_data", BCON_INT32(32)
          );
    //ogs_assert(doc);

    return doc;
}


void insert_records(mongoc_collection_t *collection,thread_args_t *args) {
    int i;
    struct timespec start, end;

    subscriber_t subscriber;
    bson_t *doc;
    bson_error_t error;
	int64_t count = 0;

    printf("Inserting records...\n");

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = args->start; i < args->end; i++) {
        /* Generate random subscriber */
        sprintf(subscriber.imsi, "%015d", i);
    
		doc = test_db_new_simple(subscriber.imsi);
		
        //if (!mongoc_collection_insert_one(collection, doc, NULL, NULL, &error)) {
        //    fprintf(stderr, "Failed to insert document: %s\n", error.message);
        //}
		
		//bson_destroy(doc);
		
		bson_t *key;
		key = BCON_NEW("imsi", BCON_UTF8(subscriber.imsi));

		
		count = mongoc_collection_count (
			collection, MONGOC_QUERY_NONE, key, 0, 0, NULL, &error);
		if (count) {
			if (mongoc_collection_remove(collection,
					MONGOC_REMOVE_SINGLE_REMOVE, key, NULL, &error) != true) {
				printf("mongoc_collection_remove() failed");
				bson_destroy(key);
				return ;
			}
		}
		bson_destroy(key);

		if (mongoc_collection_insert(collection,
					MONGOC_INSERT_NONE, doc, NULL, &error) != true) {
			printf("mongoc_collection_insert() failed");
			bson_destroy(doc);
			return ;
		}
		bson_destroy(doc);

		key = BCON_NEW("imsi", BCON_UTF8(subscriber.imsi));
		//ogs_assert(key);
		do {
			count = mongoc_collection_count(
				collection, MONGOC_QUERY_NONE, key, 0, 0, NULL, &error);
		} while (count == 0);
		bson_destroy(key);
        
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double duration = (end.tv_sec - start.tv_sec) + (double) (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Inserted %d records in %.3f seconds\n", NUM_RECORDS, duration);
}

void find_records(mongoc_collection_t *collection,thread_args_t *args) {
    int i;
    struct timespec start, end;

    subscriber_t subscriber;
    bson_t query;
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_error_t error;

    printf("Finding records...\n");

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = args->start; i < args->end; i++) {
        /* Generate random imsi */
        sprintf(subscriber.imsi, "%015d", i);

        /* Find document */
        bson_init(&query);
        BSON_APPEND_UTF8(&query, "imsi", subscriber.imsi);
        //cursor = mongoc_collection_find_with_opts(collection, &query, NULL, NULL);
		#if MONGOC_MAJOR_VERSION >= 1 && MONGOC_MINOR_VERSION >= 5
			cursor = mongoc_collection_find_with_opts(
					collection, &query, NULL, NULL);
		#else
			cursor = mongoc_collection_find(collection,
					MONGOC_QUERY_NONE, 0, 0, 0, &query, NULL, NULL);
		#endif
        while (mongoc_cursor_next(cursor, &doc)) {}

        bson_destroy(&query);
        mongoc_cursor_destroy(cursor);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double duration = (end.tv_sec - start.tv_sec) + (double) (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Found %d records in %.3f seconds\n", NUM_RECORDS, duration);
}

#if 0
void update_data_by_index(mongoc_collection_t *collection,thread_args_t *args) {
    bson_error_t error;
    bson_t *query, *update, *doc;
    mongoc_cursor_t *cursor;
    const bson_t *old_doc;
    mongoc_update_flags_t flags = MONGOC_UPDATE_NONE;
    char imsi[16], security[51];
    struct timeval start_time, end_time;
    double duration;
	int i;

    gettimeofday(&start_time, NULL);
    for (i = args->start; i < args->end; i++) {
        sprintf(imsi, "%015d", i);
        query = BCON_NEW("imsi", BCON_UTF8(imsi));
		#if MONGOC_MAJOR_VERSION >= 1 && MONGOC_MINOR_VERSION >= 5
			cursor = mongoc_collection_find_with_opts(
					collection, query, NULL, NULL);
		#else
			cursor = mongoc_collection_find(collection,
					MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
		#endif
        if (mongoc_cursor_next(cursor, &old_doc)) {
            doc = bson_copy(old_doc);
            //sprintf(security, "Security%d", i); // 更新字段为Security{i}
            //update = BCON_NEW("$set", "{", "security", BCON_UTF8(security), "}");
			
			update = BCON_NEW("$inc",
            "{",
                "security.sqn", BCON_INT64(32),
            "}");
			
            //if (!mongoc_collection_update(collection, doc, update, NULL, NULL, &error)) {
                fprintf(stderr, "%s\n", error.message);
            //}
			
			if (!mongoc_collection_update(collection,
				MONGOC_UPDATE_NONE, query, update, NULL, &error)) {
			    printf("mongoc_collection_update() failure: %s", error.message);
			}

			return;
		}
            bson_destroy(update);
            bson_destroy(doc);
        } else {
            printf("No document found with IMSI: %s\n", imsi);
        }
        bson_destroy(query);
        mongoc_cursor_destroy(cursor);
    }
    gettimeofday(&end_time, NULL);
    duration = (double)(end_time.tv_sec - start_time.tv_sec) + (double)(end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    printf("Updated %d documents in %.3f seconds\n", NUM_RECORDS, duration);
}
#endif

void update_data_by_index2(mongoc_collection_t *collection,thread_args_t *args) {
    bson_error_t error;
    bson_t *query, *update, *doc;
    mongoc_cursor_t *cursor;
    const bson_t *old_doc;
    mongoc_update_flags_t flags = MONGOC_UPDATE_NONE;
    char imsi[16], security[51];
    struct timeval start_time, end_time;
    double duration;
	int i;

    gettimeofday(&start_time, NULL);
    for (i = args->start; i < args->end; i++) {
        sprintf(imsi, "%015d", i);
        query = BCON_NEW("imsi", BCON_UTF8(imsi));
           
		update = BCON_NEW("$set",
				"{",
					"security.sqn", BCON_INT64(32),
				"}");

		if (!mongoc_collection_update(collection,
				MONGOC_UPDATE_NONE, query, update, NULL, &error)) {
			printf("mongoc_collection_update() failure: %s", error.message);

			return;
		}

		if (query) bson_destroy(query);
		if (update) bson_destroy(update);
	
    }
    gettimeofday(&end_time, NULL);
    duration = (double)(end_time.tv_sec - start_time.tv_sec) + (double)(end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    printf("Updated %d documents in %.3f seconds\n", NUM_RECORDS, duration);
}

void clear_data(mongoc_collection_t *collection,thread_args_t *args)
{
    // remove all documents from the collection, and time the operation
    bson_error_t error;
    struct timeval start, end;
    gettimeofday(&start, NULL);
	char imsi[16];
    //if (!mongoc_collection_delete_many(collection, bson_new(), NULL, NULL, &error))
    //{
    //    printf("Clear data failed: %s\n", error.message);
    //    return;
    //}
	bson_t *key;
	int i;
	
	for (i = args->start; i < args->end; i++) {
        sprintf(imsi, "%015d", i);
		key = BCON_NEW("imsi", BCON_UTF8(imsi));
		//ogs_assert(key);
		if (mongoc_collection_remove(collection,
				MONGOC_REMOVE_SINGLE_REMOVE, key, NULL, &error) != true) {
			printf("mongoc_collection_remove() failed");
			bson_destroy(key);
			return ;
		}
		bson_destroy(key);
	}
    gettimeofday(&end, NULL);
    long seconds_elapsed = (end.tv_sec - start.tv_sec);
    long microseconds_elapsed = ((seconds_elapsed * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("Clear data took %ld seconds and %ld microseconds\n", seconds_elapsed, microseconds_elapsed);
}



void *db_proc(void *thread_args) {
	mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_t doc;
    bson_error_t error;
    int i;

    /* Get thread arguments */
    thread_args_t *args = (thread_args_t *) thread_args;
	
    /* Create a new client instance */
    client = mongoc_client_new("mongodb://localhost:27017/?appname=subscriber_test");

    /* Get the collection */
    collection = mongoc_client_get_collection(client, DATABASE_NAME, COLLECTION_NAME);

    printf("Thread #%ld db_proc %d to %d...\n", pthread_self(), args->start, args->end);

	insert_records(collection,args);
    find_records(collection,args);
    update_data_by_index2(collection,args);
	clear_data(collection,args);


    /* Cleanup */
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);

    pthread_exit(NULL);	
}

int main(int argc, char *argv[]) {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
	thread_args_t *thread_args;
	pthread_t *threads;
	int i;
    /* Check for correct number of arguments */
    if (argc != 2) {
        printf("Usage: %s [0-9]\n", argv[0]);
        return EXIT_FAILURE;
    }
	
    /* Check for correct number of arguments */
    if (argc != 2) {
        printf("Usage: %s [num_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Convert argument to integer */
    num_threads = atoi(argv[1]);

    /* Allocate memory for threads and thread arguments */
    threads = malloc(sizeof(pthread_t) * num_threads);
    thread_args = malloc(sizeof(thread_args_t) * num_threads);
	

    /* Initialize MongoDB driver */
    mongoc_init();

    /* Create a new client instance */
    client = mongoc_client_new("mongodb://localhost:27017/?appname=subscriber_test");

    /* Get the collection */
    collection = mongoc_client_get_collection(client, DATABASE_NAME, COLLECTION_NAME);

    /* Create unique index on imsi field */
	bson_t keys;
	bson_init (&keys);
	BSON_APPEND_INT32(&keys, "imsi", 1);

	mongoc_index_opt_t opt;
	mongoc_index_opt_init(&opt);
	opt.unique = true;

	if (!mongoc_collection_create_index(collection, &keys, &opt, &error)) {
	   fprintf (stderr, "%s\n", error.message);
	}

	bson_destroy (&keys);
    /* Cleanup */
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
	
	
    for (i = 0; i < num_threads; i++) {
        /* Calculate start and end indices */
        int start = i * NUM_RECORDS / num_threads;
        int end = (i + 1) * NUM_RECORDS / num_threads;

        /* Set thread arguments */
        thread_args[i].start = start;
        thread_args[i].end = end;

        /* Create thread */
        pthread_create(&threads[i], NULL, db_proc, (void *) &thread_args[i]);
    }

    /* Wait for threads to finish */
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Cleanup */
    free(threads);
    free(thread_args);   

    mongoc_cleanup();

    return 0;
}
