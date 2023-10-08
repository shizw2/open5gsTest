#include <stdio.h>
#include <mongoc.h>
#include <unistd.h>

int main() {
    mongoc_init();
    // 创建MongoDB客户端
    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017,localhost:27018,localhost:27019/?replicaSet=my-replica-set");
   

    // 获取数据库和集合
    mongoc_collection_t *collection = mongoc_client_get_collection(client, "mydb", "mycollection");

    // 获取当前连接的主节点信息
    mongoc_server_description_t *description = mongoc_client_get_server_description(client, 1);
    const mongoc_host_list_t *host = mongoc_server_description_host(description);
    if (host != NULL){
        printf("Current primary:%s:%u\r\n",host->host,host->port);
    }

    while (1) {
        // 创建查询操作
        bson_t *query = bson_new();
        bson_t *opts = BCON_NEW("limit", BCON_INT32(1));

        // 查找文档
        const bson_t *doc;
        mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);

        // 如果找到了文档，则更新文档
        if (mongoc_cursor_next(cursor, &doc)) {
            bson_t *update = BCON_NEW("$inc", "{", "age", BCON_INT32(1), "}");
            mongoc_collection_update_one(collection, query, update, NULL, NULL, NULL);
            bson_destroy(update);
            printf("Updated a document.\n");
        }
        // 如果没有找到文档，则插入新的文档
        else {
            bson_t *doc = BCON_NEW("name", BCON_UTF8("John"), "age", BCON_INT32(0));
            mongoc_collection_insert_one(collection, doc, NULL, NULL, NULL);
            bson_destroy(doc);
            printf("Inserted a document.\n");
        }

        // 释放资源
        bson_destroy(query);
        bson_destroy(opts);
        mongoc_cursor_destroy(cursor);

        // 暂停1秒
        sleep(1);
    }

    // 释放资源
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return 0;
}

