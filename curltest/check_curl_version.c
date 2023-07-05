#include <stdio.h>
#include <curl/curl.h>

int main() {
    printf("libcurl version: %s\n", curl_version());
    const curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);

    printf("libcurl version: %s\n", info->version);
    printf("libcurl version number: %d\n", info->version_num);
    printf("host: %s\n", info->host);
    printf("features: %d\n", info->features);
    printf("SSL/TLS library: %s\n", info->ssl_version);
    printf("SSL/TLS library number: %ld\n", info->ssl_version_num);
    printf("zlib version: %s\n", info->libz_version);
    printf("ares version: %s\n", info->ares);
    printf("ares version number: %d\n", info->ares_num);
    printf("IDN string library: %s\n", info->libidn);
    printf("iconv library version number: %d\n", info->iconv_ver_num);
    
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, "https://127.0.0.10:7777");
      res = curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3);
      if (res != CURLE_OK) {
        fprintf(stderr,"Failed to CURL_SSLVERSION_TLSv1_3: %s\n", curl_easy_strerror(res));     
      }
      res = curl_easy_setopt(curl, CURLOPT_TLS13_CIPHERS, "DEFAULT"); // 设置 TLSv1.3 密码套件
      if (res != CURLE_OK)
        fprintf(stderr, "CURLOPT_TLS13_CIPHERS failed: %s\n", curl_easy_strerror(res));
    
      res = curl_easy_setopt(curl, CURLOPT_TLS13_CIPHERS, "TLS_AES_128_GCM_SHA256"); 
      if (res != CURLE_OK) {
       fprintf(stderr, "Failed to TLS_AES_128_GCM_SHA256: %s\n", curl_easy_strerror(res));
      //ogs_error("Error details: %s\n", errorBuffer);
      }
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      // 其他可选设置
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // 启用详细输出，用于调试目的

      res = curl_easy_perform(curl);
      if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

      curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

