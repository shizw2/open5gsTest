#include <stdio.h>
#include <curl/curl.h>

int main() {
    printf("libcurl version: %s\n", curl_version());
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, "https://127.0.0.10:7777");
      curl_easy_setopt(curl, CURLOPT_TLS13_CIPHERS, "DEFAULT"); // 设置 TLSv1.3 密码套件
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

